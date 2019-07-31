#include <iostream>
#include <string>

#include "assignments/dg/graph.h"

// Note: At the moment, there is no client.sampleout. Please do your own testing

int main() {
  gdwg::Graph<std::string, int> g;
  g.InsertNode("hello");

  g.InsertNode("how");
  g.InsertNode("are");
  g.InsertNode("you?");



  g.InsertEdge("hello", "how", 5);
  g.InsertEdge("hello", "are", 8);
  g.InsertEdge("hello", "are", 2);

  g.InsertEdge("how", "you?", 1);
  g.InsertEdge("how", "hello", 4);

  g.InsertEdge("are", "you?", 3);

  std::cout << g << '\n';
  std::cout << "endl" << std::endl;

  gdwg::Graph<std::string, int> g2{g};

  //std::cout << g2 << "\n";

  // This is a structured binding.
  // https://en.cppreference.com/w/cpp/language/structured_binding
  // It allows you to unpack your tuple.

  for (const auto& [from, to, weight] : g) {
    std::cout << from << " -> " << to << " (weight " << weight << ")\n" << std::endl;
  }


// *** MINE **** //
  gdwg::Graph<int, int> g3;
  g3.InsertNode(1);
  g3.InsertNode(2);
  g3.InsertNode(3);
  g3.InsertNode(4);
  g3.InsertNode(5);
  g3.InsertNode(6);
  g3.InsertEdge(1,5,-1);
  g3.InsertEdge(4,1,-4);
  g3.InsertEdge(4,5,3);
  g3.InsertEdge(2,4,2);
  g3.InsertEdge(2,1,1);
  g3.InsertEdge(5,2,7);
  g3.InsertEdge(3,2,2);
  g3.InsertEdge(6,3,10);
  g3.InsertEdge(3,6,-8);
  g3.InsertEdge(6,2,5);

  std::cout << "Printing g3" << std::endl << g3;

//  for (const auto& [from, to, weight] : g3) {
//    std::cout << from << " -> " << to << " (weight " << weight << ")\n" << std::endl;
//  }

//  // manual iterator test
//  for(auto it = g3.begin(); it != g3.end(); ++it) {
//    std::cout << std::get<0>(*it) << " -> " << std::get<1>(*it) << " (weight " << std::get<2>(*it) << ")\n" << std::endl;
//  }
//
//  for(auto it = --g3.end(); it != --g3.begin(); --it) {
//    std::cout << std::get<0>(*it) << " -> " << std::get<1>(*it) << " (weight " << std::get<2>(*it) << ")\n" << std::endl;
//  }
  std::cout << "COMPLETED SUCCESS" << std::endl;





  // find test
//  auto it = g3.find(2,4,2);
//      std::cout << std::get<0>(*it) << " -> " << std::get<1>(*it) << " (weight " << std::get<2>(*it) << ")\n" << std::endl;

// *** END **** //
}
