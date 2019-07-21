#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <iostream>
#include <map>
#include <memory>
#include <set>

namespace gdwg {

template <typename N, typename E>
class Graph {
 public:
  struct Edge {
    std::weak_ptr<N> src;
    std::weak_ptr<N> dst;
    std::shared_ptr<E> weight;
  };

  struct NodeCmp {
    bool operator() (const std::shared_ptr<N> lhs, const std::shared_ptr<N> rhs) const {
      return *lhs < *rhs;
    }
  };
  // this is the comparison as well as the ordering function for the set, cant seperate the two
  // Elements a and b are considered equal iff !(a < b) && !(b < a)
  struct EdgeCmp {
    bool operator() (const Edge lhs, const Edge rhs) const {
      N src1 = *lhs.src.lock();
      N src2 = *rhs.src.lock();
      if (src1 == src2) {
        N dst1 = *lhs.dst.lock();
        N dst2 = *rhs.dst.lock();
        if(dst1 == dst2) {
          E w1 = *lhs.weight;
          E w2 = *rhs.weight;
          return w1 < w2;
        }
        return dst1 < dst2;
      }
      return src1 < src2;
    }
  };



  class const_iterator {
  //   public:
  //    using iterator_category = std::bidirectional_iterator_tag;
  //    using value_type = std::tuple<N, N, E>;
  //    using reference = std::tuple<const N&, const N&, const E&>;
  //    using pointer = T*;
  //    using difference_type = int;

      // we store an edge inside an iterator
      // difference between start() and current iterator pos used to index into ordered edge set?
  };

//####################################  METHODS ########################################
  bool InsertNode(const N& val) {
    if(node_map.find(val) != node_map.end()) {
      return false;
    }
    N val_copy = val;
    std::shared_ptr<N> node = std::make_shared<N>(val_copy);
    //node_set.emplace(val);
    node_map.emplace(val_copy, node);

    // creates empty edge set for edges from this node
    std::set<Edge, EdgeCmp> edge_set;
    edge_map[node] = edge_set;
    return true;
  }
  bool InsertEdge(const N& src, const N& dst, const E& w) {
    // shared_ptr to tuple?
    std::shared_ptr<N> src_sp = node_map.find(src)->second;
    std::shared_ptr<N> dst_sp = node_map.find(dst)->second;
    Edge e{src_sp,dst_sp,std::make_shared<E>(w)};

    //std::cout << "Inserting Edge: " << *src_sp << " " << *dst_sp << " " << w << "\n";
    std::set<Edge, EdgeCmp>& edge_set = edge_map[src_sp];
    edge_set.insert(e);
    return true;
  }

  bool DeleteNode(const N& node) {
    if (node_map.find(node) == node_map.end()) {
      return false;
    }
    std::shared_ptr<N> node_sp = node_map.find(node)->second;
    // TODO: is this needed?
    node_sp.reset();
    node_map.erase(node);
    return true;
  }

  bool IsNode(const N& val) {
    return (node_map.find(val) != node_map.end());
  }

  bool IsConnected(const N& src, const N& dst) {
    std::set<Edge, EdgeCmp> edge_set = edge_map.find(src)->second;
    for(const auto &[from, to, weight] : edge_set) {
      if(*dst == *to) {
        return true;
      }
    }
    return false;
  }



//################################ FRIENDS ######################################
  friend std::ostream& operator<<(std::ostream& os, const gdwg::Graph<N, E>& g) {
    std::set<std::shared_ptr<N>, NodeCmp> node_set;
    for(const auto &[fir,sec] : g.node_map) {
      node_set.insert(sec);
    }
    for(std::shared_ptr<N> node : node_set) {
      std::cout << *node << " (\n";
      std::set<Edge, EdgeCmp> edge_set = g.edge_map.find(node)->second;
      for(const auto &[src, dst, weight] : edge_set) {
        std::cout << "  " << *dst.lock() << " | " << *weight << "\n";
      }
      std::cout << ")\n";
    }
    return os;
  }
 private:
  // maps input nodes into our heap-allocated equivalent
  std::map<N, std::shared_ptr<N>> node_map;
  std::map<std::shared_ptr<N>, std::set<Edge, EdgeCmp>> edge_map;
};

}  // namespace gdwg

#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
