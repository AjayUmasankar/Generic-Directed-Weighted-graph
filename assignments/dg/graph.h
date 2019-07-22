#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <vector>

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
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::tuple<N, N, E>;
    using reference = std::tuple<const N&, const N&, const E&>;
    using pointer = std::tuple<N, N, E>*;
    using difference_type = int;

    reference operator*() const {
      Edge cur = *index_;
      if(cur.src.expired() || cur.dst.expired()) {
        std::cout << "Expired ... " << std::endl;
      }
      return {*cur.src.lock(), *cur.dst.lock(), *cur.weight};
    }

    pointer operator->() const { return &(operator*()); }

    const_iterator operator++() {
      ++index_;
      return *this;
    }

    const_iterator operator++(int) {
      auto copy{*this};
      ++(*this);
      return copy;
    }

    const_iterator operator--() {
      --index_;
      return *this;
    }
    const_iterator operator--(int) {
      auto copy{*this};
      --(*this);
      return copy;
    }

    friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) {
      // TODO: Need to check both iterators have same set of edges as well?
      // ^ in the case of comparing iterators from two different graphs
      (void) lhs;
      (void) rhs;
      return lhs.index_ == rhs.index_;
//      if(lhs.index_ != lhs.edges_.end() && rhs.index_ != rhs.edges_.end()) {
//        return (*lhs == *rhs);
//      } else if (lhs.index_ == lhs.edges_.end() && rhs.index_ == rhs.edges_.end()) {
//        return true;
//      } else {
//        return false;
//      }



//      N src1;
//      N dst1;
//      E weight1;
//      std::tie(src1, dst1, weight1) = *lhs;
//      N src2;
//      N dst2;
//      E weight2;
//      std::tie(src2, dst2, weight2) = *rhs;
//      std::cout<<src1 << " " << dst1 << " " << weight1 <<std::endl;
//      std::cout<<src2 << " " << dst2 << " " << weight2 <<std::endl;


      //return (*lhs == *rhs);


//      Edge lhs_edge = *lhs;
//      Edge rhs_edge = *rhs;
//      return (*lhs_edge.src.lock() == *rhs_edge.src.lock() && *lhs_edge.dst.lock() == *rhs_edge.dst.lock() && *lhs_edge.weight == *rhs_edge.weight);
    }

    friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) {
      return !(lhs == rhs);
    }

   private:
    explicit const_iterator(const std::set<Edge, EdgeCmp> edges, typename std::set<Edge, EdgeCmp>::iterator index):  index_{index}, edges_{edges} {}
    typename std::set<Edge, EdgeCmp>::iterator index_;
    const std::set<Edge, EdgeCmp> edges_;


    friend class Graph;
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
    for(const Edge& edge : edge_set) {
      if(*edge.src.lock() == src && *edge.dst.lock() == dst && *edge.weight == w) {
        return false;
      }
    }
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
    std::shared_ptr<N> src_sp = node_map[src];
    std::set<Edge, EdgeCmp> edge_set = edge_map.find(src_sp)->second;
    for(const auto &[from, to, weight] : edge_set) {
      if(dst == *to.lock()) {
        return true;
      }
    }
    return false;
  }

  std::vector<N> GetNodes() {
    std::vector<N> node_vector;
    for(const auto &[fir,sec] : node_map) {
      node_vector.push_back(*sec);
    }
    return node_vector;
  }

  std::vector<N> GetConnected(const N& src) {
    std::shared_ptr<N> src_sp = node_map[src];
    // TODO: this set automatically orders?
    std::set<N> seen_nodes;
    //std::set<Edge,
    for(Edge e : edge_map[src_sp]) {
      seen_nodes.insert(*e.dst.lock());
    }
    std::vector<N> connected_nodes{seen_nodes.begin(), seen_nodes.end()};
    return connected_nodes;
  }

  std::vector<E> GetWeights(const N& src, const N& dst) {
    std::shared_ptr<N> src_sp = node_map[src];
    std::vector<E> weights;
//    // TODO: this set automatically orders?
//    std::multiset<E> weights_set;
    for(Edge e : edge_map[src_sp]) {
      if(*e.dst.lock() == dst) {
        weights.push_back(*e.weight);
      }
    }
    return weights;
  }

  void Clear() {
    // TODO: Do we need to .reset() any pointers?
    node_map.erase(node_map.begin(), node_map.end());
    edge_map.erase(edge_map.begin(), edge_map.end());
//    for(auto it = node_map.begin(); it != node_map.end(); ++it) {
//      // it->second.reset();
//      it = node_map.erase(it);
//    }
//    for(auto it = edge_map.begin(); it != edge_map.end(); ++it) {
//      it = edge_map.erase(it);
//    }
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

//################################ ITERATORS ######################################

  // TODO: can all_edges be references
  const_iterator begin() const { return cbegin(); }
  const_iterator cbegin() const {
    std::set<Edge, EdgeCmp> all_edges = GetEdges();
    return const_iterator{all_edges, all_edges.begin()};
  }
  const_iterator end() const { return cend(); }
  const_iterator cend() const {
    std::set<Edge, EdgeCmp> all_edges = GetEdges();
    return const_iterator{all_edges, all_edges.end()};
  }
//################################ HELPERS ######################################
  std::set<Edge, EdgeCmp> GetEdges() const {
    std::set<Edge, EdgeCmp> edges;
    for (const auto &[node, node_ptr] : node_map) {
      // cant use edge_map[node_ptr] or else const qualifier isnt satisfied
      const std::set<Edge, EdgeCmp>& cur_set = edge_map.find(node_ptr)->second;
      edges.insert(cur_set.begin(), cur_set.end());
    }
//  prints all edges correctly
//    for(Edge e : edges) {
//      std::cout<<*e.src.lock() << " " << *e.dst.lock() << " " << *e.weight << std::endl;
//    }
//    std::cout<<"\n";
    return edges;
  }
 private:
  // maps input nodes into our heap-allocated equivalent
  std::map<N, std::shared_ptr<N>> node_map;
  std::map<std::shared_ptr<N>, std::set<Edge, EdgeCmp>> edge_map;
};

}  // namespace gdwg

#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
