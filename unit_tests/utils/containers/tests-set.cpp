
#include "set.hpp"

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

using namespace containers;

struct value_type
{
    size_t val;
    template <typename OutputFn>
    void dump(OutputFn outfn) const
        { outfn(" %lu", val); }
};

struct value_compare
{
    bool operator() (const value_type& lhs, const value_type& rhs) const
        { return lhs.val < rhs.val; }

    bool operator() (const value_type* lhs, const value_type* rhs) const
        { return lhs->val < rhs->val; }
};

using set_t = containers::fixed_size_set<int, std::equal_to<int>, size_t, 128>;

int main(int argc, char* const argv[])
{
    
    set_t set;
    std::set<int> stdset;

    std::cout << "\n===== BEGIN AUTO TESTS =====\n" << std::endl;

    std::cout << "Testing random set" << std::endl;

    std::random_device rand;
    std::uniform_int_distribution<size_t> dist(0, 100);

    for (size_t test_i = 0; test_i < 16; ++test_i)
    {
        std::cout << "\trunning pass: " << test_i << std::endl;
        set.clear();
        stdset.clear();

        for (size_t i=0; i<set_t::MaxSize; ++i)
        {
            int rnd = dist(rand);
            std::cout << " " << rnd;
            assert(set.size() == stdset.size());
            set.add(rnd);
            stdset.emplace(rnd);
            assert(set.size() == stdset.size());
            
            assert(set.size() != 0);
        }
        std::cout << std::endl;

        for (auto x: set)
            { assert(stdset.find(x) != stdset.end()); }
        for (auto x: stdset)
            { assert(set.find(x) != set.end()); }

        std::cout << "Elements count in both sets: " << set.size() << std::endl;
    }

    std::cout << "\n===== ALL TESTS PASSED =====\n" << std::endl;

    return EXIT_SUCCESS;
}