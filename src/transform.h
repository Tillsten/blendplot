#include <blend2d.h>
#include <vector>

class Transform
{
};

class Node
{
public:
    static bool constexpr is_affine {false};
    static bool constexpr is_bbox {false};
    static bool constexpr pass_through {false};

    bool valild {false};
    
    Transform transform;
    parents =
    std::vector<Node> children;
    
    void add_child(Node node) {
        children.push_back(node);
    };

    void invalidate() {
        for (auto &&i : children)
        {
            i.invalidate();
        }
    }



};

class TransformTree
{
    Node root;
};
