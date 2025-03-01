// gen_vcxproj.cpp

#include <cassert>
#include <cstdlib>

#include "inja/inja.hpp"
#include "stduuid/uuid.h"
#include "argparse/argparse.hpp"

import std;
using json = nlohmann::json;
namespace fs = std::filesystem;

namespace {
    fs::path g_stencils_dir{"."};

    void process_file(fs::path const & stencil, std::string_view proj_name, json const & data) {
        std::stringstream buffer;
        buffer << std::ifstream(g_stencils_dir / stencil).rdbuf();
        auto const body { inja::render(buffer.str(), data)};
        auto replace_filename{[] (auto const &stencil, auto const &proj_name) {
                std::string result{stencil.string()};
                auto const & dot_pos{ result.find('.') };
                return result.replace(0, dot_pos, proj_name);
            } 
        };
        auto stencil_dup{stencil};
        auto const & outname = replace_filename(stencil_dup, proj_name);

        if (fs::exists(outname)) {
            throw std::runtime_error(std::format("'{}' already exists", outname));
        }

        if (!std::ofstream(outname).write(body.data(), body.size()))
            throw std::runtime_error(std::format("Error writing '{}'", outname));

        std::cout << stencil << " -> " << outname << '\n';
    }
}

int main(int argc, char* argv[]) {
    try {
        argparse::ArgumentParser program("gen_vcxproj");

        program.add_argument("proj_name")
            .help("name of new project");

        program.add_argument("-d", "--dir")
            .required()
            .help("Path to folder with stencils");

        program.parse_args(argc, argv);

        if (program.is_used("--dir") || program.is_used("-d")) {
            g_stencils_dir = program.get("--dir");
        }

        auto const proj_name{ program.get("proj_name") };
        std::string root_namespace{ proj_name };
        uuids::uuid const id = uuids::uuid_system_generator{}();
        json data;

        std::transform(root_namespace.begin(), root_namespace.end(), root_namespace.begin(), 
                       [](char c) { return ::isalnum(c) ? ::tolower(c) : '_'; });

        data["ProjectGuid"] = uuids::to_string(id);
        data["RootNamespace"] = root_namespace;
        data["ProjectName"] = proj_name;

        for (auto const &stencil : { "stencil.cpp", "stencil.vcxproj", "stencil.vcxproj.filters", "stencil.vcxproj.user" }) {
            process_file(stencil, proj_name, data);
        }

    }
    catch (std::exception const & e) {
        std::cerr << typeid(e).name() << ": " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}