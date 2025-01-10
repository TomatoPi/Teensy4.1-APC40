
#include "double_linked_list.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <cassert>

using namespace containers;

void context::assert_error(error::errcode code, const char* msg)
{
    std::cerr << "ASSERT ERROR: " << error::errname(code) << " "
        << msg << std::endl;
    assert(false);
}

struct value_type
{
    int val;
    template <typename OutputFn>
    void dump(OutputFn outfn) const
        { outfn("%d", val); }
};

using node_t = double_linked_node<value_type>;
using list_t = anchored_list<value_type>;

constexpr size_t NODES_COUNT = 16;

int main(int argc, char* const argv[])
{
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
                nodes[i].reinit(values.data() + i);
                nodes[i].dump(printf);
            }

        std::cout << "\nTesting Node Insertion" << std::endl;
        
        node_t anchor[2];
        for (size_t i=0; i<NODES_COUNT; ++i)
            {
                anchor[i%2].push_back(nodes[i]);
                anchor[i%2].dump(printf);

                nodes[i] = values.data() + NODES_COUNT - i - 1;
            }

        std::cout << "\nTestting list cat" << std::endl;

        std::cout << "List A" << std::endl;
        anchor[0].dump(printf);

        std::cout << "List B" << std::endl;
        anchor[1].dump(printf);

        std::cout << "List CAT" << std::endl;
        anchor[0].push_back(anchor[1]);

        std::cout << "List A" << std::endl;
        anchor[0].dump(printf);

        std::cout << "List B" << std::endl;
        anchor[1].dump(printf);
    }

    std::cout << "\n===== RUNNING AUTO TESTS =====\n" << std::endl;

    value_type valueA{0}, valueB{1}, valueC{2};
    value_type valueX{7}, valueY{8}, valueZ{9};

    node_t nodeA{&valueA}, nodeB{&valueB}, nodeC{&valueC};
    node_t nodeX{&valueX}, nodeY{&valueY}, nodeZ{&valueZ};

    list_t lst, lstW;

    auto clear_all = [&]() -> void
        {
            lst.fast_clear(); assert(lst.is_empty());
            lstW.fast_clear(); assert(lstW.is_empty());

            nodeA.reinit(&valueA); assert(nodeA.is_orphan()); assert(nodeA.value() == &valueA);
            nodeB.reinit(&valueB); assert(nodeB.is_orphan()); assert(nodeB.value() == &valueB);
            nodeC.reinit(&valueC); assert(nodeC.is_orphan()); assert(nodeC.value() == &valueC);
            nodeX.reinit(&valueX); assert(nodeX.is_orphan()); assert(nodeX.value() == &valueX);
            nodeY.reinit(&valueY); assert(nodeY.is_orphan()); assert(nodeY.value() == &valueY);
            nodeZ.reinit(&valueZ); assert(nodeZ.is_orphan()); assert(nodeZ.value() == &valueZ);
        };

    clear_all();
    std::cout << "Testing push_back" << std::endl;

    /* B <> A */
    nodeA.push_back(nodeB);
    
    assert(nodeA.front() == nodeA.back());
    assert(nodeA.front() == &nodeB);
    assert(nodeB.front() == nodeB.back());
    assert(nodeB.front() == &nodeA);

    /* B <> C <> A */
    nodeA.push_back(nodeC);
    assert(nodeA.front() == &nodeB);
    assert(nodeB.front() == &nodeC);
    assert(nodeC.front() == &nodeA);

    assert(nodeA.back() == &nodeC);
    assert(nodeB.back() == &nodeA);
    assert(nodeC.back() == &nodeB);

    clear_all();
    std::cout << "Testing push_front" << std::endl;

    /* l <> A */
    lst.push_front(nodeA);

    assert(lst.front() == &nodeA);
    assert(nodeA.front() == lst.anchor());

    assert(lst.back() == &nodeA);
    assert(nodeA.back() == lst.anchor());

    /* l <> B <> A */
    lst.push_front(nodeB);

    assert(lst.front() == &nodeB);
    assert(nodeB.front() == &nodeA);
    assert(nodeA.front() == lst.anchor());

    assert(lst.back() == &nodeA);
    assert(nodeA.back() == &nodeB);
    assert(nodeB.back() == lst.anchor());

    /* l <> C <> B <> A */
    lst.push_front(nodeC);

    assert(lst.front() == &nodeC);
    assert(nodeC.front() == &nodeB);
    assert(nodeB.front() == &nodeA);
    assert(nodeA.front() == lst.anchor());

    assert(lst.back() == &nodeA);
    assert(nodeA.back() == &nodeB);
    assert(nodeB.back() == &nodeC);
    assert(nodeC.back() == lst.anchor());

    /* do not clear */
    std::cout << "Testing push_back chain instertion" << std::endl;

    /* Y <> X */
    nodeX.push_back(nodeY);
    /* > X <> Y <> Z < */
    nodeX.push_back(nodeZ);

    assert(nodeX.front() == &nodeY);
    assert(nodeY.front() == &nodeZ);
    assert(nodeZ.front() == &nodeX);

    assert(nodeX.back() == &nodeZ);
    assert(nodeY.back() == &nodeX);
    assert(nodeZ.back() == &nodeY);

    /**
     *  l <> C <> B <> A
     * >>>
     *  X <> Y <> Z <> l <> C <> B <> A
     */
    lst.push_back(nodeX);

    assert(lst.front() == &nodeC);
    assert(nodeC.front() == &nodeB);
    assert(nodeB.front() == &nodeA);
    
    assert(nodeA.front() == &nodeX);
    assert(nodeX.front() == &nodeY);
    assert(nodeY.front() == &nodeZ);

    assert(nodeZ.front() == lst.anchor());

    assert(lst.back() == &nodeZ);
    assert(nodeZ.back() == &nodeY);
    assert(nodeY.back() == &nodeX);

    assert(nodeX.back() == &nodeA);
    assert(nodeA.back() == &nodeB);
    assert(nodeB.back() == &nodeC);

    assert(nodeC.back() == lst.anchor());

    clear_all();
    std::cout << "Testing push_front chain instertion" << std::endl;

    lst.push_back(nodeA);
    lst.push_back(nodeB);
    lst.push_back(nodeC);

    nodeX.push_back(nodeY);
    nodeX.push_back(nodeZ);

    lst.push_front(nodeX);

    assert(lst.front() == &nodeX);
    assert(nodeX.front() == &nodeY);
    assert(nodeY.front() == &nodeZ);
    
    assert(nodeZ.front() == &nodeA);
    assert(nodeA.front() == &nodeB);
    assert(nodeB.front() == &nodeC);

    assert(nodeC.front() == lst.anchor());

    assert(lst.back() == &nodeC);
    assert(nodeC.back() == &nodeB);
    assert(nodeB.back() == &nodeA);

    assert(nodeA.back() == &nodeZ);
    assert(nodeZ.back() == &nodeY);
    assert(nodeY.back() == &nodeX);

    assert(nodeX.back() == lst.anchor());


    clear_all();
    std::cout << "Testing push_back list instertion" << std::endl;

    lst.push_back(nodeA);
    lst.push_back(nodeB);
    lst.push_back(nodeC);

    lstW.push_back(nodeX);
    lstW.push_back(nodeY);
    lstW.push_back(nodeZ);

    lst.push_back(lstW);
    
    assert(lst.front() == &nodeA);
    assert(nodeA.front() == &nodeB);
    assert(nodeB.front() == &nodeC);

    assert(nodeC.front() == &nodeX);
    assert(nodeX.front() == &nodeY);
    assert(nodeY.front() == &nodeZ);

    assert(nodeZ.front() == lst.anchor());

    assert(lst.back() == &nodeZ);
    assert(nodeZ.back() == &nodeY);
    assert(nodeY.back() == &nodeX);

    assert(nodeX.back() == &nodeC);
    assert(nodeC.back() == &nodeB);
    assert(nodeB.back() == &nodeA);

    assert(nodeA.back() == lst.anchor());

    clear_all();
    std::cout << "Testing push_front list instertion" << std::endl;


    lst.push_back(nodeA);
    lst.push_back(nodeB);
    lst.push_back(nodeC);

    lstW.push_back(nodeX);
    lstW.push_back(nodeY);
    lstW.push_back(nodeZ);

    lst.push_front(lstW);

    assert(lstW.is_empty());

    assert(lst.front() == &nodeX);
    assert(nodeX.front() == &nodeY);
    assert(nodeY.front() == &nodeZ);
    
    assert(nodeZ.front() == &nodeA);
    assert(nodeA.front() == &nodeB);
    assert(nodeB.front() == &nodeC);

    assert(nodeC.front() == lst.anchor());

    assert(lst.back() == &nodeC);
    assert(nodeC.back() == &nodeB);
    assert(nodeB.back() == &nodeA);

    assert(nodeA.back() == &nodeZ);
    assert(nodeZ.back() == &nodeY);
    assert(nodeY.back() == &nodeX);

    assert(nodeX.back() == lst.anchor());

    clear_all();
    std::cout << "Testing swap" << std::endl;

    lst.push_back(nodeA);
    lst.push_back(nodeB);
    lst.push_back(nodeC);

    assert(lst.front() == &nodeA);
    assert(nodeA.front() == &nodeB);
    assert(nodeB.front() == &nodeC);
    assert(nodeC.front() == lst.anchor());

    assert(lst.back() == &nodeC);
    assert(nodeC.back() == &nodeB);
    assert(nodeB.back() == &nodeA);
    assert(nodeA.back() == lst.anchor());

    /* swap orphan nodes is a no-op */
    swap(nodeX, nodeY);
    assert(nodeX.value() == &valueX);
    assert(nodeY.value() == &valueY);
    assert(nodeX.is_empty());
    assert(nodeY.is_empty());

    /* swap empty on non empty */
    swap(nodeA, nodeX);

    assert(lst.front() == &nodeX);
    assert(nodeX.front() == &nodeB);
    assert(nodeB.front() == &nodeC);
    assert(nodeC.front() == lst.anchor());

    assert(lst.back() == &nodeC);
    assert(nodeC.back() == &nodeB);
    assert(nodeB.back() == &nodeX);
    assert(nodeX.back() == lst.anchor());

    /* swap non empty on empty */
    swap(nodeY, nodeB);

    assert(lst.front() == &nodeX);
    assert(nodeX.front() == &nodeY);
    assert(nodeY.front() == &nodeC);
    assert(nodeC.front() == lst.anchor());

    assert(lst.back() == &nodeC);
    assert(nodeC.back() == &nodeY);
    assert(nodeY.back() == &nodeX);
    assert(nodeX.back() == lst.anchor());

    /* swap between two lists */
    clear_all();

    lst.push_back(nodeA);
    lst.push_back(nodeB);
    lst.push_back(nodeC);

    assert(lst.front() == &nodeA);
    assert(nodeA.front() == &nodeB);
    assert(nodeB.front() == &nodeC);
    assert(nodeC.front() == lst.anchor());

    assert(lst.back() == &nodeC);
    assert(nodeC.back() == &nodeB);
    assert(nodeB.back() == &nodeA);
    assert(nodeA.back() == lst.anchor());

    lstW.push_back(nodeX);
    lstW.push_back(nodeY);
    lstW.push_back(nodeZ);

    assert(lstW.front() == &nodeX);
    assert(nodeX.front() == &nodeY);
    assert(nodeY.front() == &nodeZ);
    assert(nodeZ.front() == lstW.anchor());

    assert(lstW.back() == &nodeZ);
    assert(nodeZ.back() == &nodeY);
    assert(nodeY.back() == &nodeX);
    assert(nodeX.back() == lstW.anchor());

    swap(nodeA, nodeX);

    assert(lst.front() == &nodeX);
    assert(nodeX.front() == &nodeB);
    assert(nodeB.front() == &nodeC);
    assert(nodeC.front() == lst.anchor());

    assert(lst.back() == &nodeC);
    assert(nodeC.back() == &nodeB);
    assert(nodeB.back() == &nodeX);
    assert(nodeX.back() == lst.anchor());

    assert(lstW.front() == &nodeA);
    assert(nodeA.front() == &nodeY);
    assert(nodeY.front() == &nodeZ);
    assert(nodeZ.front() == lstW.anchor());

    assert(lstW.back() == &nodeZ);
    assert(nodeZ.back() == &nodeY);
    assert(nodeY.back() == &nodeA);
    assert(nodeA.back() == lstW.anchor());

    std::cout << "\n===== AUTO TESTS PASSED =====\n" << std::endl;

    return EXIT_SUCCESS;
}