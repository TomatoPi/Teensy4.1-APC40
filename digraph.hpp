/**
 * 
 */

#ifndef DEF_DIGRAPH_HPP
#define DEF_DIGRAPH_HPP

namespace digraph
{
    template <typename StepIn, typename Rest, typename StepOut>
    struct node;

    template <typename FromNode, typename AtTraversal, typename ToNode>
    struct edge;

    template <typename ...Args>
    struct digraph;

    template <typename StepIn, typename Rest, typename StepOut>
    struct node
    {
        using type = node<StepIn, Rest, StepOut>;

        using step_in_type = StepIn;    ///< Object type called when stepping into the node
        using rest_type = Rest;         ///< Object type called when resting on the node
        using step_out_type = StepOut;  ///< Object type called when stepping out of the node


        template <typename AtTraversal, typename ToNode>;
        using out_edge_type = edge<type, AtTraversal, ToNode>;

        template <typename FromNode, typename AtTraversal>;
        using in_edge_type = edge<FromNode, AtTraversal, type>;
    };


} /* endof namespace digraph */

#endif /* DEF_DIGRAPH_HPP */
