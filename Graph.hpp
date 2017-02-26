#ifndef GRAPH_HPP
#define  GRAPH_HPP
#define INF 10000

#include "Node.hpp"
#include "common.hpp"
#include <stdlib.h> //rand
#include <fstream>
#include <cmath>
#include <string>
#include <string.h>
//#include <graphviz>

class Graph
{

private:
	std::vector<Node*> nodes;
	unsigned int N;
  unsigned int** distances;
  unsigned int diameter;
  bool* subset;
	std::vector<bool*> stableSet;
	std::vector<int> stableSetClassification;
	//std::ofstream* exportFile;

	std::vector<Node*>* neigbours;
  //int* classificationCounter;
	int k[6];

	bool (Graph::*fct[2])(int);

public:
	Graph();
	void init();

  inline void addNode(Node* n);
  std::vector<Node*> getNodes() { return nodes; }

  void setDistance(int i,int j, int d);
  int getDistance(int i,int j) { return distances[i][j]; }
  void computeDistance();

  void generateRandomly(unsigned int proba,unsigned int n);
  void generateLatticeGraph(unsigned int,unsigned int);
  void generateDistanceGraph();
	void importCol(const char* filename);


  void exportPicture();
  void exportText(const char* filename);
	void exportTextStable(const char* filename, bool mode,bool dist);

	void generateStableSet();
  void findStableSet(unsigned int,int);
	void addSubset();
	bool verifyStable(int level);
	bool verifyMaxStable(int level);
	int verifyMaximun();

	int classifySubset();

	~Graph();

};


#endif
