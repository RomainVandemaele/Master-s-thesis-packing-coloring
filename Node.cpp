#include "Node.hpp"


Node::Node(int name)  : label(name) {
  //neighboors = std::vector<Node>();
}

void Node::addNeighboor(Node* n) {
  neighboors.push_back(n);
}

std::vector<Node*> Node::getNeighboor() const {
  return neighboors;
}

//Node::~Node() {}
