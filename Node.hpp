#ifndef NODE_HPP
#define  NODE_HPP

#include <iostream>
#include <vector>



class Node
{

private:
	std::vector<Node*> neighboors;
	int label;
public:
	Node(int);
	void addNeighboor(Node* n);
	int getLabel() {return label;}
	std::vector<Node*> getNeighboor() const;
	//~Node();

};

#endif
