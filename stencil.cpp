// {{ProjectName}}.cpp

import std;
#include <cassert>
#include <cstdlib>

int main(int argc, char* argv[]) {
    try {
    }
    catch (std::exception const & e) {
        std::cerr << typeid(e).name() << ": " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}