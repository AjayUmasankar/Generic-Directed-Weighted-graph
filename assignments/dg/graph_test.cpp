/*
  == Explanation and rational of testing ==
  Explain and justify how you approached testing, the degree
   to which you're certain you have covered all possibilities,
   and why you think your tests are that thorough.
https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md
*/

#include "assignments/dg/graph.h"
#include "catch.h"

SCENARIO("Default Graph constructor and basic methods work") {
  GIVEN("Default Graph constructor Graph<string, int>") {
    gdwg::Graph<std::string, int> g;

    WHEN("gdwg::Graph::IsEmpty is used to check state of graph") {
      THEN("Result is true (graph is empty)") {
        REQUIRE(g.IsEmpty() == true);
      }
    }

    WHEN("gdwg::Graph::NumNodes is used to check number of nodes") {
      THEN("Result is 0") {
        REQUIRE(g.NumNodes() == 0);
      }
    }

    WHEN("gdwg::Graph::NumEdges is used to check number of edges") {
      THEN("Result is 0") {
        REQUIRE(g.NumEdges() == 0);
      }
    }

    WHEN("gdwg::Graph::InsertNode is used to insert nodes") {
      std::string s{"n"};
      REQUIRE(g.InsertNode(s) == true);
      THEN("Node is inserted as expected") {
        REQUIRE(g.IsNode(s) == true);
        REQUIRE(g.IsEmpty() == false);
        REQUIRE(g.NumNodes() == 1);
        REQUIRE(g.NumEdges() == 0);
      }
      WHEN("Second node is inserted") {
        std::string t{"t"};
        REQUIRE(g.InsertNode(t));
        THEN("Node is inserted as expected") {
          REQUIRE(g.IsNode(s) == true);
          REQUIRE(g.IsEmpty() == false);
          REQUIRE(g.NumNodes() == 2);
          REQUIRE(g.NumEdges() == 0);
        }
      }
      WHEN("Existing node is inserted") {
        THEN("Result of insertion is false and graph is unchanged") {
          REQUIRE(g.InsertNode(s) == false);
          REQUIRE(g.IsNode(s) == true);
          REQUIRE(g.IsEmpty() == false);
          REQUIRE(g.NumNodes() == 1);
          REQUIRE(g.NumEdges() == 0);
        }
      }
    }

    WHEN("gdwg::Graph::InsertEdge is used to insert an edge") {
      std::string node1{"c"};
      std::string node2{"s"};
      int weight = 1;
      REQUIRE(g.InsertNode(node1) == true);
      REQUIRE(g.InsertNode(node2) == true);
      REQUIRE(g.InsertEdge(node1, node2, weight) == true);
      THEN("InsertEdge on already inserted edge fails") {
        REQUIRE(g.InsertEdge(node1, node2, weight) == false);
      }
      THEN("Edge is inserted as expected") {
        // this is a directed graph, so one edge!!
        REQUIRE(g.NumEdges() == 1);
        REQUIRE(g.IsConnected(node1, node2) == true);
        REQUIRE(g.GetWeights(node1, node2)[0] == weight);
        REQUIRE(g.GetConnected(node1)[0] == node2);
      }
    }

    WHEN("gdwg::Graph::InsertEdge is used to insert multiple edges") {
      std::string node1{"c"};
      std::string node2{"s"};
      std::string node3{"v"};
      int weight = 1;
      REQUIRE(g.InsertNode(node1) == true);
      REQUIRE(g.InsertNode(node2) == true);
      REQUIRE(g.InsertNode(node3) == true);
      REQUIRE(g.InsertEdge(node1, node2, weight) == true);
      THEN("InsertEdge on already inserted edge fails") {
        REQUIRE(g.InsertEdge(node1, node2, weight) == false);
      }
      REQUIRE(g.InsertEdge(node2, node3, weight) == true);
      REQUIRE(g.InsertEdge(node2, node3, -weight) == true);
      REQUIRE(g.InsertEdge(node2, node2, weight) == true);
      THEN("InsertEdge on already inserted same-node edge fails") {
        REQUIRE(g.InsertEdge(node2, node2, weight) == false);
      }
      REQUIRE(g.InsertEdge(node2, node1, weight + 1) == true);
      THEN("Edges are inserted as expected") {
        // this is a directed graph, so one edge!!
        REQUIRE(g.NumEdges() == 5);
        REQUIRE(g.IsConnected(node1, node2) == true);
        REQUIRE(g.IsConnected(node1, node3) == false);
        REQUIRE(g.GetWeights(node1, node2)[0] == weight);
        REQUIRE(g.GetConnected(node1)[0] == node2);
        REQUIRE(g.GetWeights(node2, node3)[0] == -weight);
        REQUIRE(g.GetWeights(node2, node3)[1] == weight);
        REQUIRE(g.GetConnected(node2)[0] == node1);
        REQUIRE(g.GetConnected(node2)[1] == node2);
        REQUIRE(g.GetConnected(node2)[2] == node3);
      }
    }

    WHEN("gdwg::Graph::InsertEdge is used for non-existing nodes") {
      std::string node1{"c"};
      std::string node2{"s"};
      int weight = 1;
      REQUIRE(g.IsNode(node1) == false);
      REQUIRE(g.IsNode(node2) == false);
      THEN("Exception is thrown and graph is unchanged") {
        CHECK_THROWS_WITH(g.InsertEdge(node1, node2, weight), "Cannot call Graph::InsertEdge when either src or dst node does not exist");
        REQUIRE(g.IsNode(node1) == false);
        REQUIRE(g.IsNode(node2) == false);
      }
    }
  }
}

SCENARIO("Graph Constructor with vectors work") {
  GIVEN("Vector of ints") {
    std::vector<int> v{1,2,3,4,5};
    WHEN("Graph constructor is called with vector of ints") {
      gdwg::Graph<int, int> g{v.begin(), v.end()};
      THEN("Contents of graph has vector as nodes") {
        REQUIRE(g.IsEmpty() == false);
        REQUIRE(g.NumNodes() == v.size());
        for (const auto& e : v)
          REQUIRE(g.IsNode(e));
        REQUIRE_THAT(g.GetNodes(), Catch::Matchers::Equals(std::vector<int>{1,2,3,4,5}));
        REQUIRE(g.NumEdges() == 0);
      }
    }
  }

  GIVEN("Vector of strings") {
    std::vector<std::string> v{"Hello", "how", "are", "you"};
    WHEN("Graph constructor is called with vector of strings") {
      gdwg::Graph<std::string, double> g{v.begin(),v.end()};
      THEN("Contents of graph has vector as nodes as expected") {
        REQUIRE(g.IsEmpty() == false);
        REQUIRE(g.NumNodes() == v.size());
        for (const auto& e : v)
          REQUIRE(g.IsNode(e));
        REQUIRE_THAT(g.GetNodes(), Catch::Matchers::Equals(std::vector<std::string>{"Hello","are","how","you"}));

      }
    }
  }
}

SCENARIO("Graph constructor with vector of tuples work") {
  GIVEN("Vector of tuples<string, string, int>") {
    std::string s1{"Hello"};
    std::string s2{"how"};
    std::string s3{"are"};
    double w1 = 5.4;
    double w2 = 7.6;
    auto e1 = std::make_tuple(s1, s2, w1);
    auto e2 = std::make_tuple(s2, s3, w2);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2};
    WHEN("Graph constructor is called with vector of tuples") {
      gdwg::Graph<std::string, double> g{e.begin(), e.end()};
      THEN("Contents of graph has nodes and connected edges as expected") {
        REQUIRE(g.IsNode(s1));
        REQUIRE(g.IsNode(s2));
        REQUIRE(g.IsNode(s3));
        REQUIRE(g.IsConnected(s1, s2) == true);
        REQUIRE(g.GetWeights(s1, s2)[0] == w1);
        REQUIRE(g.GetConnected(s1)[0] == s2);
        REQUIRE(g.IsConnected(s2, s3) == true);
        REQUIRE(g.GetWeights(s2, s3)[0] == w2);
        REQUIRE(g.GetConnected(s2)[0] == s3);
      }
    }
  }

  GIVEN("Vector of tuples<int, int, int>") {
    int s1 = 1;
    int s2 = 2;
    int s3 = 3;
    int w1 = 5;
    int w2 = 7;
    auto e1 = std::make_tuple(s1, s2, w1);
    auto e2 = std::make_tuple(s2, s3, w2);
    auto e = std::vector<std::tuple<int, int, int>>{e1, e2};
    WHEN("Graph constructor is called with vector of tuples") {
      gdwg::Graph<int, int> g{e.begin(), e.end()};
      THEN("Contents of graph has nodes and connected edges as expected") {
        REQUIRE(g.IsNode(s1));
        REQUIRE(g.IsNode(s2));
        REQUIRE(g.IsNode(s3));
        REQUIRE(g.IsConnected(s1, s2) == true);
        REQUIRE(g.GetWeights(s1, s2)[0] == w1);
        REQUIRE(g.GetConnected(s1)[0] == s2);
        REQUIRE(g.IsConnected(s2, s3) == true);
        REQUIRE(g.GetWeights(s2, s3)[0] == w2);
        REQUIRE(g.GetConnected(s2)[0] == s3);
      }
    }
  }

  GIVEN("Vector of tuples<int, int, double>") {
    int s1 = 1;
    int s2 = 2;
    int s3 = 3;
    double w1 = 5;
    double w2 = 7;
    auto e1 = std::make_tuple(s1, s2, w1);
    auto e2 = std::make_tuple(s2, s3, w2);
    auto e = std::vector<std::tuple<int, int, double>>{e1, e2};
    WHEN("Graph constructor is called with vector of tuples") {
      gdwg::Graph<int, double> g{e.begin(), e.end()};
      THEN("Contents of graph has nodes and connected edges as expected") {
        REQUIRE(g.IsNode(s1));
        REQUIRE(g.IsNode(s2));
        REQUIRE(g.IsNode(s3));
        REQUIRE(g.IsConnected(s1, s2) == true);
        REQUIRE(g.GetWeights(s1, s2)[0] == w1);
        REQUIRE(g.GetConnected(s1)[0] == s2);
        REQUIRE(g.IsConnected(s2, s3) == true);
        REQUIRE(g.GetWeights(s2, s3)[0] == w2);
        REQUIRE(g.GetConnected(s2)[0] == s3);
      }
    }
  }
}

SCENARIO("Graph constructor with initializer lists work") {
  GIVEN("Graph is constructed with initializer list of chars") {
    gdwg::Graph<char, std::string> g{'c', 's', 'e'};
    THEN("Graph is constructed with the nodes as expected") {
      REQUIRE(g.IsEmpty() == false);
      REQUIRE(g.NumNodes() == 3);
      REQUIRE(g.NumEdges() == 0);

      REQUIRE(g.IsNode('c') == true);
      REQUIRE(g.IsNode('s') == true);
      REQUIRE(g.IsNode('e') == true);
    }
  }

  GIVEN("Graph is constructed with initializer list of ints") {
    gdwg::Graph<int, int> g{99, 115, 101};
    THEN("Graph is constructed with the nodes as expected") {
      REQUIRE(g.IsEmpty() == false);
      REQUIRE(g.NumNodes() == 3);
      REQUIRE(g.NumEdges() == 0);

      REQUIRE(g.IsNode(99) == true);
      REQUIRE(g.IsNode(115) == true);
      REQUIRE(g.IsNode(101) == true);
    }
  }
}

SCENARIO("Graph copy constructor/assignment work") {
  GIVEN("Graph constructed with initializer list of chars") {
    gdwg::Graph<char, std::string> g{'c', 's', 'e'};
    WHEN("Copy constructor is used") {
      g.InsertEdge('c','s',"howdy");
      gdwg::Graph<char, std::string> cp{g};
      THEN("Graph is copied with the nodes as expected") {
        REQUIRE(cp.IsEmpty() == false);
        REQUIRE(cp.NumNodes() == 3);
        REQUIRE(cp.NumEdges() == 1);

        REQUIRE(cp.IsNode('c') == true);
        REQUIRE(cp.IsNode('s') == true);
        REQUIRE(cp.IsNode('e') == true);
      }
    }

    WHEN("Copy assignment is used") {
      gdwg::Graph<char, std::string> cp = g;
      THEN("Graph is copied with the nodes as expected") {
        REQUIRE(cp.IsEmpty() == false);
        REQUIRE(cp.NumNodes() == 3);
        REQUIRE(cp.NumEdges() == 0);

        REQUIRE(cp.IsNode('c') == true);
        REQUIRE(cp.IsNode('s') == true);
        REQUIRE(cp.IsNode('e') == true);
      }
    }
  }

  GIVEN("Graph is constructed with vector of ints") {
    std::vector<int> v{1, 2, 3, 4, 5};
    gdwg::Graph<int, int> g{v.begin(), v.end()};
    WHEN("Copy constructor is used") {
      gdwg::Graph<int, int> cp{g};
      THEN("Copy of graph has vector as nodes") {
        REQUIRE(cp.IsEmpty() == false);
        REQUIRE(cp.NumNodes() == v.size());
        for (const auto& e : v)
          REQUIRE(cp.IsNode(e));
      }
    }

    WHEN("Copy assignment is used") {
      gdwg::Graph<int, int> cp = g;
      THEN("Copy of graph has vector as nodes") {
        REQUIRE(cp.IsEmpty() == false);
        REQUIRE(cp.NumNodes() == v.size());
        for (const auto& e : v)
          REQUIRE(cp.IsNode(e));
      }
    }
  }

  GIVEN("Vector of tuples<string, string, int>") {
    std::string s1{"Hello"};
    std::string s2{"how"};
    std::string s3{"are"};
    double w1 = 5.4;
    double w2 = 7.6;
    auto e1 = std::make_tuple(s1, s2, w1);
    auto e2 = std::make_tuple(s2, s3, w2);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2};
    gdwg::Graph<std::string, double> g{e.begin(), e.end()};
    WHEN("Copy constructor is used") {
      gdwg::Graph<std::string, double> cp{g};
      THEN("Copy of graph has nodes and connected edges as expected") {
        REQUIRE(cp.IsNode(s1));
        REQUIRE(cp.IsNode(s2));
        REQUIRE(cp.IsNode(s3));
        REQUIRE(cp.IsConnected(s1, s2) == true);
        REQUIRE(cp.GetWeights(s1, s2)[0] == w1);
        REQUIRE(cp.GetConnected(s1)[0] == s2);
        REQUIRE(cp.IsConnected(s2, s3) == true);
        REQUIRE(cp.GetWeights(s2, s3)[0] == w2);
        REQUIRE(cp.GetConnected(s2)[0] == s3);
      }
    }

    WHEN("Copy assignment is used") {
      gdwg::Graph<std::string, double> cp = g;
      THEN("Copy of graph has nodes and connected edges as expected") {
        REQUIRE(cp.IsNode(s1));
        REQUIRE(cp.IsNode(s2));
        REQUIRE(cp.IsNode(s3));
        REQUIRE(cp.IsConnected(s1, s2) == true);
        REQUIRE(cp.GetWeights(s1, s2)[0] == w1);
        REQUIRE(cp.GetConnected(s1)[0] == s2);
        REQUIRE(cp.IsConnected(s2, s3) == true);
        REQUIRE(cp.GetWeights(s2, s3)[0] == w2);
        REQUIRE(cp.GetConnected(s2)[0] == s3);
      }
    }
  }
}

SCENARIO("Graph move constructor/assignment work") {
  GIVEN("Vector of tuples<string, string, int>") {
    std::string s1{"Hello"};
    std::string s2{"how"};
    std::string s3{"are"};
    double w1 = 5.4;
    double w2 = 7.6;
    auto e1 = std::make_tuple(s1, s2, w1);
    auto e2 = std::make_tuple(s2, s3, w2);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2};
    gdwg::Graph<std::string, double> g{e.begin(), e.end()};
    WHEN("Move constructor is used") {
      gdwg::Graph<std::string, double> cp{std::move(g)};
      THEN("Graph has been moved to another graph as expected") {
        REQUIRE(cp.IsNode(s1));
        REQUIRE(cp.IsNode(s2));
        REQUIRE(cp.IsNode(s3));
        REQUIRE(cp.IsConnected(s1, s2) == true);
        REQUIRE(cp.GetWeights(s1, s2)[0] == w1);
        REQUIRE(cp.GetConnected(s1)[0] == s2);
        REQUIRE(cp.IsConnected(s2, s3) == true);
        REQUIRE(cp.GetWeights(s2, s3)[0] == w2);
        REQUIRE(cp.GetConnected(s2)[0] == s3);

        REQUIRE(g.IsEmpty());
      }
    }

    WHEN("Move assignment is used") {
      gdwg::Graph<std::string, double> cp = std::move(g);
      THEN("Graph has been moved to another graph as expected") {
        REQUIRE(cp.IsNode(s1));
        REQUIRE(cp.IsNode(s2));
        REQUIRE(cp.IsNode(s3));
        REQUIRE(cp.IsConnected(s1, s2) == true);
        REQUIRE(cp.GetWeights(s1, s2)[0] == w1);
        REQUIRE(cp.GetConnected(s1)[0] == s2);
        REQUIRE(cp.IsConnected(s2, s3) == true);
        REQUIRE(cp.GetWeights(s2, s3)[0] == w2);
        REQUIRE(cp.GetConnected(s2)[0] == s3);

        REQUIRE(g.IsEmpty());
      }
    }
  }
}

SCENARIO("Graph Delete Node works") {
  GIVEN("Graph with some nodes and edges in it") {
    gdwg::Graph<int, int> g;
    REQUIRE(g.InsertNode(0) == true);
    REQUIRE(g.InsertNode(1) == true);
    REQUIRE(g.InsertNode(2) == true);
    REQUIRE(g.InsertNode(3) == true);
    REQUIRE(g.InsertEdge(0, 1, 0) == true);
    REQUIRE(g.InsertEdge(2, 0, 0) == true);
    REQUIRE(g.InsertEdge(0, 2, 0) == true);
    REQUIRE(g.InsertEdge(2, 3, 0) == true);
    REQUIRE(g.NumNodes() == 4);
    REQUIRE(g.NumEdges() == 4);
    REQUIRE(g.IsConnected(0, 1) == true);
    REQUIRE(g.IsConnected(2, 0) == true);
    REQUIRE(g.IsConnected(0, 2) == true);
    REQUIRE(g.IsConnected(2, 3) == true);
    WHEN("gdwg::Graph::DeleteNode is used to remove a single existing node") {
      REQUIRE(g.DeleteNode(0) == true);
      REQUIRE(g.IsNode(0) == false);
      REQUIRE(g.NumNodes() == 3);
      REQUIRE(g.NumEdges() == 1);
      REQUIRE(g.IsConnected(2, 3) == true);
      REQUIRE_THROWS_WITH(
          g.IsConnected(0, 1),
          "Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
      REQUIRE_THROWS_WITH(
          g.IsConnected(2, 0),
          "Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
      REQUIRE_THROWS_WITH(
          g.IsConnected(0, 2),
          "Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
      for (int i = 1; i < 4; i++)
        REQUIRE(g.IsNode(i));
    }
    WHEN("gdwg::Graph::DeleteNode is used to remove non-existing nodes") {
      REQUIRE(g.IsNode(69) == false);
      REQUIRE(g.DeleteNode(69) == false);
      REQUIRE(g.NumNodes() == 4);
      for (int i = 0; i < 4; ++i)
        REQUIRE(g.IsNode(i));
    }
    WHEN("Existing node is removed and re-added") {
      REQUIRE(g.DeleteNode(0) == true);
      REQUIRE(g.InsertNode(0) == true);
      REQUIRE(g.NumNodes() == 4);
      REQUIRE(g.NumEdges() == 1);
      REQUIRE(g.IsConnected(2, 3) == true);
      REQUIRE(g.IsConnected(0, 1) == false);
      REQUIRE(g.IsConnected(2, 0) == false);
      REQUIRE(g.IsConnected(0, 2) == false);
      for (int i = 0; i < 4; i++)
        REQUIRE(g.IsNode(i));
    }
  }
}

SCENARIO("Graph Replace Node works") {
  GIVEN("Graph with some nodes") {
    gdwg::Graph<int, int> g;
    int num = 5;
    for (int i = 0; i < num; ++i)
      REQUIRE(g.InsertNode(i) == true);
    REQUIRE(g.NumNodes() == num);
    WHEN("gdwg::Graph::Replace is used with existing node and non-existing new node") {
      REQUIRE(g.Replace(1, num) == true);
      THEN("Existing node is replaced with new data") {
        REQUIRE(g.NumNodes() == num);
        REQUIRE(g.IsNode(num) == true);
        REQUIRE(g.IsNode(1) == false);
      }
    }
    WHEN("gdwg::Graph::Replace is used with non-existing old node") {
      THEN("Exception is thrown and graph is unchanged") {
        CHECK_THROWS_WITH(g.Replace(num, num),
                          "Cannot call Graph::Replace on a node that doesn't exist");
        REQUIRE(g.NumNodes() == num);
        for (int i = 0; i < num; ++i)
          REQUIRE(g.IsNode(i) == true);
      }
    }
    WHEN("gdwg::Graph::Replace is used with both different existing nodes") {
      THEN("Graph is unchanged") {
        REQUIRE(g.Replace(0, 1) == false);
        REQUIRE(g.NumNodes() == num);
        for (int i = 0; i < num; ++i)
          REQUIRE(g.IsNode(i) == true);
      }
    }
    WHEN("gdwg::Graph::Replace is used with same existing nodes") {
      THEN("Graph is unchanged") {
        REQUIRE(g.Replace(0, 0) == false);
        REQUIRE(g.NumNodes() == num);
        for (int i = 0; i < num; ++i)
          REQUIRE(g.IsNode(i) == true);
      }
    }
  }
}

SCENARIO("Graph MergeReplace works") {
  GIVEN("Basic MergeReplace Example from Specifications") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("A");
    g.InsertNode("B");
    g.InsertNode("C");
    g.InsertNode("D");
    g.InsertEdge("A", "B", 1);
    g.InsertEdge("A", "C", 2);
    g.InsertEdge("A", "D", 3);
    REQUIRE(g.NumEdges() == 3);
    WHEN("gdwg::Graph::MergeReplace is used with A, B") {
      g.MergeReplace("A", "B");
      THEN("Node is replaced and edges are merged as expected") {
        REQUIRE(g.IsNode("A") == false);
        REQUIRE(g.IsNode("B") == true);
        REQUIRE(g.IsConnected("B", "B") == true);
        REQUIRE(g.IsConnected("B", "C") == true);
        REQUIRE(g.IsConnected("B", "D") == true);
        REQUIRE(g.NumEdges() == 3);
      }
    }
    WHEN("gdwg::Graph::MergeReplace is used with non-existant lhs node") {
      THEN("Exception is thrown") {
        REQUIRE_THROWS_WITH(g.MergeReplace("E", "C"), "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
      }
    }
    WHEN("gdwg::Graph::MergeReplace is used with non-existant rhs node") {
      THEN("Exception is thrown") {
        REQUIRE_THROWS_WITH(g.MergeReplace("C", "E"), "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
      }
    }
    WHEN("gdwg::Graph::MergeReplace is used with both non-existant nodes") {
      THEN("Exception is thrown") {
        REQUIRE_THROWS_WITH(g.MergeReplace("X", "Y"), "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
      }
    }
  }

  GIVEN("Duplicate Edge MergeReplace Example from Specification") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("A");
    g.InsertNode("B");
    g.InsertNode("C");
    g.InsertNode("D");
    g.InsertEdge("A", "B", 1);
    g.InsertEdge("A", "C", 2);
    g.InsertEdge("A", "D", 3);
    g.InsertEdge("B", "B", 1);
    REQUIRE(g.NumEdges() == 4);
      WHEN("gdwg::Graph::MergeReplace is used with A, B") {
      g.MergeReplace("A", "B");
      THEN("Node is replaced and edges are merged as expected") {
        REQUIRE(g.IsNode("A") == false);
        REQUIRE(g.IsNode("B") == true);
        REQUIRE(g.IsConnected("B", "B") == true);
        REQUIRE(g.IsConnected("B", "C") == true);
        REQUIRE(g.IsConnected("B", "D") == true);
        REQUIRE(g.NumEdges() == 3);
      }
    }
  }

  GIVEN("Extra example for Incoming/Outgoing Edges") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("A");
    g.InsertNode("B");
    g.InsertNode("C");
    g.InsertNode("D");
    g.InsertEdge("A", "B", 1);
    g.InsertEdge("B", "C", 2);
    g.InsertEdge("A", "D", 3);
    REQUIRE(g.NumEdges() == 3);
      WHEN("gdwg::Graph::MergeReplace is used with A, B") {
      g.MergeReplace("B", "C");
      THEN("Node is replaced and edges are merged as expected") {
        REQUIRE(g.IsNode("A") == true);
        REQUIRE(g.IsNode("B") == false);
        REQUIRE(g.IsNode("C") == true);
        REQUIRE(g.IsNode("D") == true);
        REQUIRE(g.IsConnected("A", "D") == true);
        REQUIRE(g.IsConnected("C", "C") == true);
        REQUIRE(g.IsConnected("A", "C") == true);
        REQUIRE(g.NumEdges() == 3);
      }
    }
  }
}

SCENARIO("Basic Graph iterator methods work as expected") {
  GIVEN("Graph that is empty") {
    gdwg::Graph<std::string, double> g;
    THEN("All variations of begin and end are equal") {
      // cbegin == cend == begin == end
      // rbegin == rend == crbegin == crend
      REQUIRE(g.cbegin() == g.cend());
      REQUIRE(g.cend() == g.begin());
      REQUIRE(g.begin() == g.end());
      REQUIRE(g.rbegin() == g.rend());
      REQUIRE(g.rend() == g.crbegin());
      REQUIRE(g.crbegin() == g.crend());
    }
  }
  GIVEN("Graph with some nodes and edges") {
    gdwg::Graph<std::string, double> g;
    std::string node1{"c"};
    std::string node2{"s"};
    std::string node3{"v"};
    int weight = 1;
    REQUIRE(g.InsertNode(node1) == true);
    REQUIRE(g.InsertNode(node2) == true);
    REQUIRE(g.InsertNode(node3) == true);
    REQUIRE(g.InsertEdge(node1, node2, weight) == true);
    REQUIRE(g.InsertEdge(node2, node3, weight) == true);
    REQUIRE(g.InsertEdge(node2, node3, -weight) == true);
    REQUIRE(g.InsertEdge(node2, node2, weight) == true);
    THEN("Begin and End variations act as described in spec") {
      REQUIRE(*g.begin() == std::make_tuple("c", "s", weight));
      REQUIRE(*g.cbegin() == std::make_tuple("c", "s", weight));
      REQUIRE(*g.rbegin() == std::make_tuple("s", "v", weight));
      REQUIRE(*g.crbegin() == std::make_tuple("s", "v", weight));
      REQUIRE(*(--g.end()) == std::make_tuple("s", "v", weight));
      REQUIRE(*(--g.cend()) == std::make_tuple("s", "v", weight));
      REQUIRE(*(--g.rend()) == std::make_tuple("c", "s", weight));
      REQUIRE(*(--g.crend()) == std::make_tuple("c", "s", weight));
    }
  }
  GIVEN("Graph with empty nodes at the start and end") {
    gdwg::Graph<std::string, double> g;
    std::string node00{"a"};
    std::string node0{"b"};
    std::string node1{"c"};
    std::string node2{"s"};
    std::string node3{"v"};
    std::string node4{"w"};
    std::string node5{"x"};
    std::string node6{"z"};
    int weight = 1;
    REQUIRE(g.InsertNode(node1) == true);
    REQUIRE(g.InsertNode(node2) == true);
    REQUIRE(g.InsertNode(node3) == true);
    REQUIRE(g.InsertNode(node4) == true);
    REQUIRE(g.InsertNode(node5) == true);
    REQUIRE(g.InsertNode(node6) == true);
    REQUIRE(g.InsertNode(node00) == true);
    REQUIRE(g.InsertNode(node0) == true);
    REQUIRE(g.InsertEdge(node1, node2, weight) == true);
    REQUIRE(g.InsertEdge(node2, node3, weight) == true);
    REQUIRE(g.InsertEdge(node2, node3, -weight) == true);
    REQUIRE(g.InsertEdge(node2, node2, weight) == true);
    THEN("Begin and End variations act as described in spec") {
      REQUIRE(*g.begin() == std::make_tuple("c", "s", weight));
      REQUIRE(*g.cbegin() == std::make_tuple("c", "s", weight));
      REQUIRE(*g.rbegin() == std::make_tuple("s", "v", weight));
      REQUIRE(*g.crbegin() == std::make_tuple("s", "v", weight));
      REQUIRE(*(--g.end()) == std::make_tuple("s", "v", weight));
      REQUIRE(*(--g.cend()) == std::make_tuple("s", "v", weight));
      REQUIRE(*(--g.rend()) == std::make_tuple("c", "s", weight));
      REQUIRE(*(--g.crend()) == std::make_tuple("c", "s", weight));
    }
  }

}

SCENARIO("Graph iterator navigates edges in correct order") {
  GIVEN("Graph with some nodes and edges, and expected edge order") {
    gdwg::Graph<int, int> g;
    // filler nodes, shouldnt impact anything
    REQUIRE(g.InsertNode(0) == true);
    REQUIRE(g.InsertNode(7) == true);
    REQUIRE(g.InsertNode(18) == true);
    REQUIRE(g.InsertNode(21) == true);
    REQUIRE(g.InsertNode(-5) == true);
    REQUIRE(g.InsertNode(-22) == true);

    REQUIRE(g.InsertNode(3) == true);
    REQUIRE(g.InsertNode(5) == true);
    REQUIRE(g.InsertNode(4) == true);
    REQUIRE(g.InsertNode(1) == true);
    REQUIRE(g.InsertNode(2) == true);
    REQUIRE(g.InsertNode(6) == true);
    REQUIRE(g.InsertEdge(1, 5, -1) == true);
    REQUIRE(g.InsertEdge(4, 1, -4) == true);
    REQUIRE(g.InsertEdge(4, 5, 3) == true);
    REQUIRE(g.InsertEdge(2, 4, 2) == true);
    REQUIRE(g.InsertEdge(2, 1, 1) == true);
    REQUIRE(g.InsertEdge(5, 2, 7) == true);
    REQUIRE(g.InsertEdge(3, 2, 2) == true);
    REQUIRE(g.InsertEdge(6, 3, 10) == true);
    REQUIRE(g.InsertEdge(3, 6, -8) == true);
    REQUIRE(g.InsertEdge(6, 2, 5) == true);
    // 10 edges
    std::vector<std::tuple<int, int, int>> expected_edges = {{1,5,-1}, {2,1,1}, {2,4,2}, {3,2,2}, {3,6,-8}, {4,1,-4}, {4,5,3}, {5,2,7}, {6,2,5}, {6,3,10}};
    WHEN("Normal iterator is used to iterate through edges") {
      THEN("Edges are as expected when using preincrement") {
        int i = 0;
        for(auto it = g.begin(); it != g.cend(); ++it) {
          REQUIRE(*it == expected_edges[i++]);
        }
      }
      THEN("Edges are as expected when using postincrement") {
        int i = 0;
        for(auto it = g.begin(); it != g.end(); it++) {
          REQUIRE(*it == expected_edges[i++]);
        }
        REQUIRE(i == 10);
      }
      THEN("Edges are as expected when using predecrement"){
        int i = 0;
        for(auto it = --g.end(); it != g.begin(); --it) {
          REQUIRE(*it == expected_edges[9-i++]);
        }
      }
      THEN("Edges are as expected when using postdecrement"){
        int i = 0;
        for(auto it = --g.end(); it != g.begin(); it--) {
          REQUIRE(*it == expected_edges[9-i++]);
        }
        REQUIRE(i == 9);
      }
    }
    WHEN("Reverse iterator is used to iterate through edges") {
      THEN("Edges are as expected when using preincrement") {
        int i = 0;
        for (auto it = g.rbegin(); it != g.rend(); ++it) {
          REQUIRE(*it == expected_edges[9 - i++]);
        }
      }

      THEN("Edges are as expected when using postincrement") {
        int i = 0;
        for(auto it = g.rbegin(); it != g.rend(); it++) {
          REQUIRE(*it == expected_edges[9 - i++]);
        }
      }
      THEN("Edges are as expected when using predecrement"){
        int i = 0;
        for(auto it = --g.rend(); it != g.rbegin(); --it) {
          REQUIRE(*it == expected_edges[i++]);
        }
      }
      THEN("Edges are as expected when using postdecrement"){
        int i = 0;
        for(auto it = --g.rend(); it != g.rbegin(); it--) {
          REQUIRE(*it == expected_edges[i++]);
        }
      }
    }
  }
}

SCENARIO("Graph find works as expected") {
  gdwg::Graph<int, int> g;
  GIVEN("Graph with no edges") {
    REQUIRE(g.InsertNode(1) == true);
    REQUIRE(g.InsertNode(88) == true);
    REQUIRE(g.NumEdges() == 0);
    THEN("Find returns g.end() when called") {
      auto it = g.find(1,88,0);
      REQUIRE(it == g.end());
      REQUIRE(g.NumEdges() == 0);
    }
  }
  GIVEN("Graph with one edge") {
    REQUIRE(g.InsertNode(1) == true);
    REQUIRE(g.InsertNode(88) == true);
    REQUIRE(g.InsertEdge(1,88,1) == true);
    REQUIRE(g.NumEdges() == 1);
    REQUIRE(g.IsConnected(1,88) == true);
    THEN("Find can find the edge successfully with no side effects") {
      auto it = g.find(1,88,1);
      REQUIRE(it != g.cend());
      REQUIRE(g.NumEdges() == 1);
      REQUIRE(g.IsConnected(1,88) == true);
    }
    THEN("Find fails when searching for nonexistant edge") {
      auto it = g.find(1,88,2);
      REQUIRE(it == g.cend());
    }
  }
  GIVEN("Graph with many edges") {
    REQUIRE(g.InsertNode(0) == true);
    REQUIRE(g.InsertNode(3) == true);
    REQUIRE(g.InsertNode(5) == true);
    REQUIRE(g.InsertNode(4) == true);
    REQUIRE(g.InsertNode(1) == true);
    REQUIRE(g.InsertNode(2) == true);
    REQUIRE(g.InsertNode(6) == true);
    REQUIRE(g.InsertNode(87) == true);
    REQUIRE(g.InsertNode(7) == true);
    REQUIRE(g.InsertNode(-1) == true);
    REQUIRE(g.InsertEdge(1, 5, -1) == true);
    REQUIRE(g.InsertEdge(4, 1, -4) == true);
    REQUIRE(g.InsertEdge(4, 5, 3) == true);
    REQUIRE(g.InsertEdge(2, 4, 2) == true);
    REQUIRE(g.InsertEdge(2, 1, 1) == true);
    REQUIRE(g.InsertEdge(4, 2, 7) == true);
    REQUIRE(g.InsertEdge(3, 2, 2) == true);
    REQUIRE(g.InsertEdge(6, 3, 10) == true);
    REQUIRE(g.InsertEdge(3, 6, -8) == true);
    REQUIRE(g.InsertEdge(6, 2, 5) == true);
    REQUIRE(g.NumEdges() == 10);
    THEN("Find can find first edge successfully") {
      auto it = g.find(1,5,-1);
      REQUIRE(*it == std::make_tuple(1, 5, -1));
    }
    THEN("Find can find a (random) edge successfully") {
      auto it = g.find(4,5,3);
      REQUIRE(*it == std::make_tuple(4, 5, 3));
    }
    THEN("Find can find last edge successfully") {
      auto it = g.find(6,3,10);
      REQUIRE(*it == std::make_tuple(6, 3, 10));
    }
  }
}

SCENARIO("Graph erase(iterator) works as expected") {
  gdwg::Graph<int, int> g;
  GIVEN("Graph with no edges") {
    REQUIRE(g.InsertNode(1) == true);
    REQUIRE(g.InsertNode(88) == true);
    REQUIRE(g.NumEdges() == 0);
    THEN("Erase returns g.end() when called") {
      auto it = g.erase(g.cbegin());
      REQUIRE(it == g.end());
      REQUIRE(g.NumEdges() == 0);
    }
  }
  GIVEN("Graph with one edge") {
    REQUIRE(g.InsertNode(1) == true);
    REQUIRE(g.InsertNode(88) == true);
    REQUIRE(g.InsertEdge(1,88,1) == true);
    REQUIRE(g.NumEdges() == 1);
    REQUIRE(g.IsConnected(1,88) == true);
    THEN("Erase can remove first edge successfully") {
      auto it = g.erase(g.cbegin());
      REQUIRE(it == g.cend());
      REQUIRE(g.NumEdges() == 0);
      REQUIRE(g.IsConnected(1,88) == false);
    }
  }
  GIVEN("Graph with many edges") {
    REQUIRE(g.InsertNode(0) == true);
    REQUIRE(g.InsertNode(3) == true);
    REQUIRE(g.InsertNode(5) == true);
    REQUIRE(g.InsertNode(4) == true);
    REQUIRE(g.InsertNode(1) == true);
    REQUIRE(g.InsertNode(2) == true);
    REQUIRE(g.InsertNode(6) == true);
    REQUIRE(g.InsertNode(87) == true);
    REQUIRE(g.InsertNode(7) == true);
    REQUIRE(g.InsertNode(-1) == true);
    REQUIRE(g.InsertEdge(1, 5, -1) == true);
    REQUIRE(g.InsertEdge(4, 1, -4) == true);
    REQUIRE(g.InsertEdge(4, 5, 3) == true);
    REQUIRE(g.InsertEdge(2, 4, 2) == true);
    REQUIRE(g.InsertEdge(2, 1, 1) == true);
    REQUIRE(g.InsertEdge(4, 2, 7) == true);
    REQUIRE(g.InsertEdge(3, 2, 2) == true);
    REQUIRE(g.InsertEdge(6, 3, 10) == true);
    REQUIRE(g.InsertEdge(3, 6, -8) == true);
    REQUIRE(g.InsertEdge(6, 2, 5) == true);
    REQUIRE(g.NumEdges() == 10);
    // successfully here implies no side effects (
    THEN("Erase can remove first edge successfully") {
      REQUIRE(g.IsConnected(1,5) == true);
      REQUIRE(g.find(1,5,-1) != g.cend());
      auto it = g.erase(g.cbegin());
      REQUIRE(*it == std::make_tuple(2, 1, 1));
      REQUIRE(g.NumEdges() == 9);
      REQUIRE(g.IsConnected(1,5) == false);
      REQUIRE(g.find(1,5,-1) == g.cend());
    }
    THEN("Erase can remove an edge in the middle successfully") {
      REQUIRE(g.NumEdges() == 10);
      REQUIRE(g.IsConnected(4,5) == true);
      REQUIRE(g.find(4,5,3) != g.cend());
      auto it = g.erase(g.find(4,5,3));
      REQUIRE(*it == std::make_tuple(6,2,5));
      REQUIRE(g.NumEdges() == 9);
      REQUIRE(g.IsConnected(4,5) == false);
      REQUIRE(g.find(4,5,3) == g.cend());
    }
    THEN("Erase can remove an edge at the end successfully") {
      REQUIRE(g.NumEdges() == 10);
      REQUIRE(g.IsConnected(6,3) == true);
      REQUIRE(g.find(6,3,10) != g.cend());
      auto it = g.erase(--g.cend());
      REQUIRE(it == g.cend());
      REQUIRE(g.NumEdges() == 9);
      REQUIRE(g.IsConnected(6,3) == false);
      REQUIRE(g.find(6,3,10) == g.cend());
    }
  }
}



SCENARIO("Const graphs can call appropriate const member functions") {
  GIVEN("Const graph with some nodes and edges") {
    gdwg::Graph<int, int> g_creator;
    REQUIRE(g_creator.InsertNode(2) == true);
    REQUIRE(g_creator.InsertNode(3) == true);
    REQUIRE(g_creator.InsertNode(5) == true);
    REQUIRE(g_creator.InsertNode(4) == true);
    REQUIRE(g_creator.InsertNode(6) == true);
    REQUIRE(g_creator.InsertNode(1) == true);
    REQUIRE(g_creator.InsertEdge(1, 5, -1) == true);
    REQUIRE(g_creator.InsertEdge(4, 1, -4) == true);
    REQUIRE(g_creator.InsertEdge(4, 5, 3) == true);
    REQUIRE(g_creator.InsertEdge(2, 4, 2) == true);
    REQUIRE(g_creator.InsertEdge(2, 1, 1) == true);
    REQUIRE(g_creator.InsertEdge(5, 2, 7) == true);
    REQUIRE(g_creator.InsertEdge(3, 2, 2) == true);
    REQUIRE(g_creator.InsertEdge(6, 3, 10) == true);
    REQUIRE(g_creator.InsertEdge(3, 6, -8) == true);
    REQUIRE(g_creator.InsertEdge(6, 2, 5) == true);
    REQUIRE(g_creator.InsertEdge(6, 2, -10) == true);
    const gdwg::Graph<int, int> g = g_creator;
    THEN("IsNode works") {
      REQUIRE(g.IsNode(2) == true);
    }
    THEN("IsConnected works") {
      REQUIRE(g.IsConnected(1,5) == true);
    }
    THEN("GetNodes works") {
      REQUIRE_THAT(g.GetNodes(), Catch::Matchers::Equals(std::vector<int>{1,2,3,4,5,6}));
    }
    THEN("GetConnected works") {
      REQUIRE_THAT(g.GetConnected(4), Catch::Matchers::Equals(std::vector<int>{1,5}));
    }
    THEN("GetWeights works") {
      REQUIRE_THAT(g.GetWeights(6,2), Catch::Matchers::Equals(std::vector<int>{-10,5}));
    }
    THEN("Find works") {
      auto it = g.find(2,4,2);
      REQUIRE(*it == std::make_tuple(2,4,2));
    }
    THEN("Iterators contain correct order of edges") {
      std::vector<std::tuple<int, int, int>> expected_edges = {{1,5,-1}, {2,1,1}, {2,4,2}, {3,2,2}, {3,6,-8}, {4,1,-4}, {4,5,3}, {5,2,7}, {6,2,-10}, {6,2,5}, {6,3,10}};
      THEN("Edges are as expected when using preincrement") {
        int i = 0;
        for (auto it = g.rbegin(); it != g.rend(); ++it) {
          REQUIRE(*it == expected_edges[10 - i++]);
        }
      }
      THEN("Edges are as expected when using preincrement") {
        int i = 0;
        for(auto it = g.begin(); it != g.cend(); ++it) {
          REQUIRE(*it == expected_edges[i++]);
        }
      }
    }
    THEN("Operator == works") {
      REQUIRE(g == g_creator);
      g_creator.InsertNode(181);
      REQUIRE_FALSE(g == g_creator);
      g_creator.DeleteNode(181);
      REQUIRE(g == g_creator);
    }
    THEN("Operator != works") {
      REQUIRE_FALSE(g != g_creator);
    }
  }
}