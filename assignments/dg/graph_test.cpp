/*

  == Explanation and rational of testing ==

  Explain and justify how you approached testing, the degree
   to which you're certain you have covered all possibilities,
   and why you think your tests are that thorough.

*/

#include "assignments/dg/graph.h"
#include "catch.h"

// TODO: GetNodes increasing order of node??
// TODO: GetConnected ambiguity (include only outnodes or innodes?)

// TODO(students): Fill this in.
SCENARIO("Basic graph construction methods work") {
  gdwg::Graph<std::string, int> g;
  GIVEN("Non-const Graphs") {
    gdwg::Graph<std::string, int> g1;
    WHEN("method InsertNode is used to insert \"s\" as a node") {
      std::string node{"s"};
      g1.InsertNode(node);
      THEN("Node is inserted as expected") {
        REQUIRE(g1.IsNode("s") == true);
      }
    }
    WHEN("Method InsertEdge is executed") {
      std::string node1{"s"};
      std::string node2{"r"};
      g1.InsertNode(node1);
      g1.InsertNode(node2);
      REQUIRE(g1.InsertEdge(node1, node2, 1) == true);
      THEN("InsertEdge on already inserted edge fails") {
        REQUIRE(g1.InsertEdge(node1, node2, 1) == false);
      }
      THEN("Edge is inserted as expected") {
        REQUIRE(g1.IsConnected(node1, node2) == true);
        REQUIRE(g1.GetWeights(node1, node2)[0] == 1);
        REQUIRE(g1.GetConnected(node1)[0] == "node2");
      }
    }
  }
}

SCENARIO("Methods on more complex sample graphs work as expected") {
  GIVEN("sample graph") {
    gdwg::Graph<int, int> g;
    g.InsertNode(1);
    g.InsertNode(2);
    g.InsertNode(3);
    g.InsertNode(4);
    g.InsertNode(5);
    g.InsertNode(6);
    g.InsertEdge(1,5,-1);
    g.InsertEdge(4,1,-4);
    g.InsertEdge(4,5,3);
    g.InsertEdge(2,4,2);
    g.InsertEdge(2,1,1);
    g.InsertEdge(5,2,7);
    g.InsertEdge(3,2,2);
    g.InsertEdge(6,3,10);
    g.InsertEdge(3,6,-8);
    g.InsertEdge(6,2,5);
//  std::ostringstream graph_string;
//  graph_string << g;
//  graph_string == "1 (\n"
//                  "  5 | -1\n"
//                  ")\n"
//                  "2 (\n"
//                  "  1 | 1\n"
//                  "  4 | 2\n"
//                  ")\n"
//                  "3 (\n"
//                  "  2 | -8\n"
//                  "  2 | 2\n"
//                  ")\n"
//                  "4 (\n"
//                  "  1 | -4\n"
//                  "  5 | 3\n"
//                  "5 (\n"
//                  "  2 | 7\n"
//                  ")\n"
//                  "6 (\n"
//                  "  2 | 5\n"
//                  "  3 | 10\n"
//                  ")"

    REQUIRE(g.IsNode(6) == true);
    REQUIRE(g.IsNode(1));
    REQUIRE(g.IsConnected(1,5));
    REQUIRE(g.IsConnected(3,2));
    REQUIRE_THAT(g.GetNodes(), Catch::Matchers::Equals(
        std::vector<int>{1,2,3,4,5,6}));
    REQUIRE_THAT(g.GetConnected(4), Catch::Matchers::Equals(
        std::vector<int>{2}));
    REQUIRE_THAT(g.GetWeights(3,6), Catch::Matchers::Equals(
        std::vector<int>{-8,10}));
    WHEN("MergeReplace is called successfully") {
      // TODO: Edge case where duplicate edges are removed
      REQUIRE_FALSE(g.IsConnected(4,6));
      REQUIRE_FALSE(g.IsConnected(2,6));
      g.MergeReplace(1, 6);
      // 4->6 and 2->6 created (since 4->1 and 2->1 exist)
      REQUIRE(g.IsConnected(4,6));
      REQUIRE(g.IsConnected(2,6));
      //REQUIRE_THAT(g.GetConnected(6), Catch::Matchers::Equals(std::vector<int>{1,5}));
      // do print graph check aswell
    }
    WHEN("Clear is called on the graph") {
      g.Clear();
      REQUIRE(g.GetNodes.size() == 0);
    }
    // TODO: const_iterator find

    WHEN("Erase is called on an existing edge") {
      REQUIRE(g.IsConnected(5,2));
      REQUIRE(g.erase(5,2,7));
      REQUIRE_FALSE(g.IsConnected(5,2));
    }

    // TODO: const_iterator erase
    // TODO: lots of iterator methods
  }

}