
#include "utils/containers/double_linked_list.hpp"

#include <array>
#include <cstddef>
#include <iostream>

using namespace containers;

struct value_type
{
    int val;
    template <typename OutputFn>
    void dump(OutputFn outfn) const
        { outfn("%d", val); }
};

struct context_type
{
    template <typename ...Args>
    static void trace(Args... args)
        { }

    template <typename ...Args>
    static void assert_error(Args... args)
        { }
};



using node_t = double_linked_node<value_type, context_type>;

constexpr size_t NODES_COUNT = 16;

int main(int argc, char* const argv[])
{
    std::array<value_type, NODES_COUNT> values;
    std::array<node_t, NODES_COUNT> nodes;

    for (auto& node: nodes)
        {
            node.dump(printf);    
        }

    std::cout << "\nTesting value instertion" << std::endl;

    for (size_t i=0; i<NODES_COUNT; ++i)
        {
            values[i].val = i;
            nodes[i] = node_t(values.data() + i);
            nodes[i].dump(printf);
        }

    std::cout << "\nTesting Node Insertion" << std::endl;
    
    node_t anchor[2];
    for (size_t i=0; i<NODES_COUNT; ++i)
        {
            anchor[i%2].push_back(&nodes[i]);
            anchor[i%2].dump(printf);

            nodes[i] = node_t(values.data() + NODES_COUNT - i - 1);
        }

    std::cout << "\nTestting list cat" << std::endl;

    std::cout << "List A" << std::endl;
    anchor[0].dump(printf);

    std::cout << "List B" << std::endl;
    anchor[1].dump(printf);

    std::cout << "List CAT" << std::endl;
    anchor[0].push_back(&anchor[1]);

    std::cout << "List A" << std::endl;
    anchor[0].dump(printf);

    std::cout << "List B" << std::endl;
    anchor[1].dump(printf);


    return EXIT_SUCCESS;
}