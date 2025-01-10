
#include "queues.hpp"

#include <array>
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

using stack_t = Stack<value_type>;
using queue_t = Queue<value_type>;
using heap_t = Heap<value_type, value_compare, 16>;

using node_type = typename stack_t::node_type;

int main(int argc, char* const argv[])
{
    std::array<value_type, 16> values;
    std::array<node_type, 16> nodes;

    std::array<heap_t::node_type, 64> hnodes;

    stack_t stack;
    queue_t queue;
    heap_t heap;

    auto clear_all = [&]() -> void
        {
            for (size_t i=0; i<nodes.size(); ++i)
                {
                    values[i].val = i;
                    nodes[i].reinit(values.data() +i);
                    assert(nodes[i].is_empty());
                }

            heap.deep_clear();

            assert(stack.is_empty());
            assert(queue.is_empty());
            assert(heap.is_empty());
        };

    std::cout << "\n===== BEGIN AUTO TESTS =====\n" << std::endl;
    clear_all();

    std::cout << "Testing stack" << std::endl;

    for (size_t i=0; i<nodes.size(); ++i)
        {
            stack.push(nodes[i]);
            assert(stack.next()->value()->val == i);
        }
    assert(!stack.is_empty());

    for (size_t i=nodes.size()-1; 0 < i ; --i)
        {
            assert(stack.next()->value()->val == i);
            stack.pop();
            assert(stack.next()->value()->val == i-1);
        }
    assert(stack.next()->value()->val == 0);
    stack.pop();
    assert(stack.is_empty());
    stack.pop();
    assert(stack.is_empty());

    std::cout << "Testing queue" << std::endl;
    clear_all();

    for (size_t i=0; i<nodes.size(); ++i)
        {
            queue.push(nodes[i]);
            assert(queue.next()->value()->val == 0);
        }
    assert(!queue.is_empty());

    for (size_t i=0; i<nodes.size()-1; ++i)
        {
            assert(queue.next()->value()->val == i);
            queue.pop();
            assert(queue.next()->value()->val == i+1);
        }
    assert(queue.next()->value()->val == nodes.size()-1);
    queue.pop();
    assert(queue.is_empty());
    queue.pop();
    assert(queue.is_empty());

    std::cout << "Testing random heap" << std::endl;

    std::random_device rand;
    std::uniform_int_distribution<size_t> dist(0, 100);


    clear_all();
    for (size_t test_i = 0; test_i < 16; ++test_i)
    {
        std::cout << "\trunning pass: " << test_i << std::endl;

        for (auto& node: hnodes)
            {
                if (!heap.is_full())
                    { heap.push(node); }
                assert(heap.check());
            }

        // std::cout << "\t\tinsertion ok ..." << std::endl;

        for (auto& node: hnodes)
            {
                size_t v = dist(rand);
                node = value_type{v};
                assert(heap.check());
            }
        // std::cout << "\t\tedition ok ..." << std::endl;

        for (size_t i=0; i<hnodes.size() / 2; ++i)        
            {
                heap.pop();
                assert(heap.check());
            }

        // std::cout << "\t\tpop ok ..." << std::endl;
    }

    std::cout << "\n===== ALL TESTS PASSED =====\n" << std::endl;

    return EXIT_SUCCESS;
}