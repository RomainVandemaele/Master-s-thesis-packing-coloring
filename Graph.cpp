#include "Graph.hpp"


Graph::Graph()  : diameter(0) {
  init();
}


void Graph::init() {
  fct[0] = &Graph::verifyStable;
  fct[1] = &Graph::verifyMaxStable;
}

inline void Graph::addNode(Node* N) { nodes.push_back(N); }

void Graph::setDistance(int i,int j, int d) {
    distances[i][j] = d;
    distances[j][i] = d;
}

void Graph::computeDistance() {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      if(getDistance(j,i) < INF) {
        for (size_t k = 0; k < N; ++k) {
          if(k!=i && k!=j && getDistance(i,k) < INF) {
            if(  getDistance(j,i) + getDistance(i,k)  < getDistance(j,k)) {
              setDistance(j,k,getDistance(j,i) + getDistance(i,k));
            }
          }
        }
      }
    }
  }
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      if(distances[i][j] > diameter && distances[i][j] < INF ) {
        diameter = distances[i][j];
      }
    }
  }

}


void Graph::importCol(const char* filename) {
  char line[500];
  FILE * file;
  file = fopen( filename , "r");
  if (file) {
    fscanf(file, "%s", line);
    while(strcmp(line,"p") != 0) {
      fscanf(file, "%s", line);
    }
    fscanf(file, "%s", line); //pass word edge
    int e; //number of edges
    fscanf(file, "%u %i", &N,&e);
    //fscanf(file, "%u %i", &N,&e, line);

    distances = new unsigned int*[N];
    subset = new bool[N];
    for (size_t i = 0; i < N; ++i) {
      Node* v = new Node(i);
      this->addNode(v);
      distances[i] = new unsigned int[N];
      for (size_t j = 0; j < N; ++j) {
        distances[i][j] = INF;
      }
      subset[i] = false;
    }

    //edges
    int e1,e2;
    while (fscanf(file, "%s", line)!=EOF) {
      fscanf(file, "%i", &e1);
      fscanf(file, "%i", &e2);
      this->nodes[e1-1]->addNeighboor(nodes[e2-1]);
      this->nodes[e2-1]->addNeighboor(nodes[e1-1]);
      setDistance(e1-1,e2-1,1);
    }
        //printf("%s",str);
    fclose(file);
    computeDistance();

    this->neigbours = new std::vector<Node*>[N];
    for (size_t k = 0; k < N; k++) {
      neigbours[k] = nodes[k]->getNeighboor();
    }

  }else {
     std::cout << "Unable to open file col"  << std::endl;
  }
}

void Graph::generateRandomly(unsigned int proba,unsigned int n) {
  N = n;
  distances = new unsigned int*[N];
  subset = new bool[N];
  for (size_t i = 0; i < N; ++i) {
    Node* v = new Node(i);
    this->addNode(v);
    distances[i] = new unsigned int[N];
    setDistance(i,i,INF);
    subset[i] = false;
  }

  for (size_t i = 0; i < N; ++i) {
    for (size_t j = i+1; j < N; ++j) {
      unsigned int p = rand() % 100;
      if (p < proba ) { //edge between i and j
        this->nodes[i]->addNeighboor(nodes[j]);
        this->nodes[j]->addNeighboor(nodes[i]);
        setDistance(i,j,1);
      }else {
        setDistance(i,j,INF);
      }
    }
  }
  computeDistance();
  this->neigbours = new std::vector<Node*>[N];
  for (size_t k = 0; k < N; k++) {
    neigbours[k] = nodes[k]->getNeighboor();
  }
}

void Graph::generateLatticeGraph(unsigned int row,unsigned int column) { //generate Pr × Pc : N rows m columns
  N = row*column;
  //exportFile << N << std::endl;
  subset = new bool[N];
  distances = new unsigned int*[N];
  for (size_t i = 0; i < N; ++i) {
    distances[i] = new unsigned int[N];
    subset[i] = false;
    for (size_t j = 0; j < N; ++j) {
      distances[i][j] = INF;
    }
  }
  for (size_t r = 0; r < row; ++r) {
    for (size_t c = 0; c < column; ++c) {
      Node* v = new Node(r*row + c);
      this->addNode(v);
      if(r > 0) {
        nodes[r*row + c ]->addNeighboor(nodes[r*(row-1) + c]);
        nodes[r*(row-1) + c]->addNeighboor(nodes[r*row + c]);
        setDistance(r*row + c,r*(row-1) + c,1);
      }

      if(c>0) {
        nodes[r*row + c   ]->addNeighboor(nodes[r*row + c -1]);
        nodes[r*row + c -1]->addNeighboor(nodes[r*row + c]);
        setDistance(r*row + c,r*row + c -1,1);
      }
    }
  }
  this->neigbours = new std::vector<Node*>[N];
  for (size_t k = 0; k < N; k++) {
    neigbours[k] = nodes[k]->getNeighboor();
  }
  computeDistance();
}


void Graph::exportText(const char* filename) {
  std::ofstream myfile;
  myfile.open (filename);
  myfile << N << std::endl;
  myfile << diameter << std::endl;
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      myfile << distances[i][j] << " ";
    }
    myfile << std::endl;
  }
  myfile.close();
}

void Graph::exportTextStable(const char* filename, bool mode,bool dist) {

  //std::cout << "OK STABLE " << std::endl;
  std::cout << "OK STABLE 2" << std::endl;
  std::ofstream myfile(filename, std::ios::out | std::ios::binary);
  unsigned int N = nodes.size();
  //myfile.open (filename);

  //main data
  myfile << N << std::endl;
  myfile << diameter << std::endl;

  myfile << stableSet.size() << std::endl;

  for (size_t j = 0; j < stableSet.size() ; ++j) {
    myfile << stableSetClassification[j] << " ";
  }
  myfile << std::endl;

  //element by subset
  short int s = 7;
  unsigned char  k = 0;

  for (size_t i = 0; i < stableSet.size() ; ++i)  {
    for (size_t j = 0; j < nodes.size(); j++) {
      if(mode) {
        myfile << stableSet[i][j] << " ";
      }else {
        k = k | (stableSet[i][j] << s);
        if(s ==0) {
          myfile << k ;
          //myfile << (int) k << " ";
          s = 8;
          k = 0;
        }
        s--;
      }
    }
    //Si N%8 != 0 write last bits
    if(s != 7) {myfile << k  ;}
    s = 7;
    k = 0;
    myfile << std::endl;

  }
  if(dist) {
    std::cout << "distance 0 1 " << distances[0][1] << '\n';
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        myfile << distances[i][j] << " ";
      }
      myfile << std::endl;
    }
  }

  myfile.close();

}


void Graph::exportPicture() {
  std::ofstream myfile;
  myfile.open ("graph.dot");

  myfile << "Graph G {" << std::endl;
  for (size_t i = 0; i < N; i++) {
    std::vector<Node*> v =  nodes[i]->getNeighboor();
    for (size_t j = 0; j < v.size(); j++) {
      if (v[j]->getLabel() > nodes[i]->getLabel() ) {
        myfile << nodes[i]->getLabel() << " -- " << v[j]->getLabel() <<";" << std::endl;
      }
    }
  }
  myfile << "}" << std::endl;
  myfile.close();
  system("dot -Tsvg  graph.dot -o graph.svg");
}

/*
Add basic stabvle set (size 1)
*/
void Graph::generateStableSet() {

  for (size_t i = 0; i < nodes.size(); i++) {
    subset[i] = true;
    addSubset();
    subset[i] = false;
  }
  std::cout << "GENERATED " << stableSet.size() << '\n';
}

void Graph::addSubset() {
  if(verifyMaximun()) { //maximum for its category
    bool* copy = new bool[nodes.size()];
    for (size_t i = 0; i < nodes.size(); i++) {
      copy[i] = subset[i];
    }
    stableSet.push_back(copy);
  }else {
    stableSetClassification.pop_back();
  }
}

void Graph::findStableSet(unsigned int level,int fctIndex) {
  //static float  counter = 0;
  //static  float  divisor = pow(2.0,1.0*(N-1)) ;
  //static int bestCardinality = 0;
  if(level == nodes.size()) {
    addSubset();
  }else {
    subset[level] = true;
    //std::cout << "OK 1.21 " << fctIndex << " " << level << '\n';
    if(  (this->*fct[fctIndex])(level) ) {
        findStableSet(level+1,fctIndex); //recursive call do take node level
    }
    if(level == 4 ) {
      std::cout << "CAME BACK " << level << std::endl;
    }
    subset[level] = false;
    findStableSet(level+1,fctIndex); //recursive call do not take node level
  }
}

bool Graph::verifyMaxStable(int level) {
  //std::cout << "OK 1.22'" << '\n';
  for (size_t i = 0; i < N; ++i) {
    if(subset[i] && distances[i][level] != INF) {return false;}
    //if(subset[neigbours[level][i]->getLabel()]) {return false;}
  }
  return true;
}

bool Graph::verifyStable(int level) {
  //std::cout << "OK 1.22 "   << level << '\n';
  for (size_t i = 0; i < neigbours[level].size(); ++i) {
    if(subset[neigbours[level][i]->getLabel()]) {return false;}
  }
  return true;
}

int Graph::verifyMaximun() {
  //std::cout << "AK" << '\n';
  unsigned int ci = classifySubset();
  //std::cout << "AK V1" << '\n';
  unsigned int i = 0;
  bool res = true; //true if maximal
  while(i < nodes.size() && res) {
    if(subset[i] ==  0) {
      bool resI = true; //true if can be added
      //std::cout << "AK V1.1" << '\n';
      resI = verifyStable(i); //TO DO Adapt VERIFY FCT depending on algo
      unsigned int j = 0;
      //test if classification is still the same
      while( j < nodes.size() && resI) {
        if(subset[j] && distances[i][j] < ci) {
          resI = false;
        }
        ++j;
      }
      res = !resI;
    }
    ++i;
  }
  return res;
}

int Graph::classifySubset() {
  int min =  INF;
  for (size_t i = 0; i < nodes.size(); i++) {
    for (size_t j = 0; j < nodes.size(); j++) {
      if(subset[i] && subset[j]) {
          if(getDistance(i,j) < min) {
            min = getDistance(i,j);
          }
      }
    }
  }
  stableSetClassification.push_back(min);
  return min;
}

Graph::~Graph() {
  for (size_t i = 0; i < N; ++i) {
    delete[] distances[i];
    delete nodes[i];
  }
  delete[] distances;
  delete[] subset;
}
