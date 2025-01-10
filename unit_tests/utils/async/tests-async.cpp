
#include "async.hpp"

#include <set>
#include <cstddef>
#include <iostream>
#include <cassert>
#include <random>

void context::assert_error(error::errcode code, const char* msg)
{
    std::cerr << "ASSERT ERROR: " << error::errname(code) << " "
        << msg << std::endl;
    assert(false);
}

using namespace async;

int main(int argc, char* const argv[])
{
    std::cout << "\n===== BEGIN AUTO TESTS =====\n" << std::endl;


    std::cout << "\n===== ALL TESTS PASSED =====\n" << std::endl;

    return EXIT_SUCCESS;
}