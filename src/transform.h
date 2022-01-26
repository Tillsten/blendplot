#include <array>
#include <blend2d.h>
#include <memory>
#include <vector>
#include <catch2>
class Node {
public:
  static bool constexpr is_affine{false};
  static bool constexpr is_bbox{false};
  static bool constexpr pass_through{false};

  bool valild{false};

  parents = std::array<std::weak_ptr<Node>>{};
  std::vector<std::shared_ptr<Node>> children;

  void add_child(Node node) { children.push_back(node); };

  void invalidate() {
    for (auto &&i : children) {
      i.invalidate();
    }
  }
};

class TransformTree {
  Node root;
};

class Transform : public Node {
public:
};

class AffineTransform : public Transform {
public:
  static bool constexpr is_affine{true};
  static bool constexpr is_bbox{false};
  static bool constexpr pass_through{false};

  BLMatrix2D mat = BLMatrix2D::makeIdentity();

  AffineTransform(const BLMatrix2D &other) { mat = BLMatrix2D::reset(other); }


  void translate(double x, double y) { 
    mat.translate(x, y);
    invalidate();
  }

  void scale(double x, double y) {
    mat.scale(x, y);
    invalidate();}

  void skew(double x, double y) {
    mat.skew(x, y);
    invalidate();
  }

  void rotate(double x, double y) {
    mat.rotate();
    invalidate();
    return &this;
  }

  void transform(const BLMatrix2D& other_mat) {
    mat.transform(other_mat);
    invalidate();
    return &this;
  }

  AffineTransform copy()
  

};
