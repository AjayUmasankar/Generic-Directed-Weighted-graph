#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
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
  class Node {
   public:
    Node(N value) : node_sp{std::make_shared<N>(value)} {};
    N& operator* () const { return *node_sp; }
    std::shared_ptr<N> get() const { return node_sp; }

    bool operator<(const Node& other) const {
      return *this < *other;
    }
    friend bool operator==(const Node& lhs, const Node& rhs) {
      return *lhs.node_sp == *rhs.node_sp;
    }

   private:
    std::shared_ptr<N> node_sp;
  };


  struct NodeCmp {
    bool operator() (const Node& lhs, const Node& rhs) const {
      return *lhs < *rhs;
    }
  };


  struct PtrCmp {
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
      index_++;
      if(index_ == index_outer_->second.cend()) {
        index_outer_++;
        if(index_outer_->second.empty()) {
          index_outer_++;
        }
        index_ = index_outer_->second.cbegin();
      }
      return *this;
    }

    const_iterator operator++(int) {
      auto copy{*this};
      ++(*this);
      return copy;
    }

    const_iterator operator--() {
      if(index_ == index_outer_->second.cbegin()) {
        index_outer_--;
        index_ = index_outer_->second.cbegin();
      } else {
        index_--;
      }
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
//      if (lhs.index_outer_ == rhs.index_outer_) {
//        if(lhs.index_ == lhs.index_outer_->second.cend() && rhs.index_ == rhs.index_outer_->second.cend()) {
//          return true;
//        } else if (lhs.index_ == rhs.index_) {
//          return true;
//        }
//      }
//      return false;


      //std::cout << "entrance" << std::endl;
      bool value = lhs.index_outer_ == rhs.index_outer_;
      //std::cout << "exit " << value << std::endl;

      return value;


    }

    friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) {
      return !(lhs == rhs);
    }

   private:
    explicit const_iterator(typename std::map<std::shared_ptr<N>, std::set<Edge, EdgeCmp>, PtrCmp>::const_iterator index_outer) : index_outer_ {index_outer}  {
      index_ = index_outer_->second.cbegin();
    }
    typename std::map<std::shared_ptr<N>, std::set<Edge, EdgeCmp>, PtrCmp>::const_iterator index_outer_;
    typename std::set<Edge, EdgeCmp>::const_iterator index_;

    friend class Graph;
  };

//####################################  METHODS ########################################
  bool InsertNode(const N& val) {
    if(node_set.find(Node{val}) != node_set.end()) {
      return false;
    }
    Node new_node{val};
    node_set.insert(new_node);

    // creates empty edge set for edges from this node
    std::set<Edge, EdgeCmp> edge_set;
    edge_map[new_node.get()] = edge_set;
    return true;
  }
  bool InsertEdge(const N& src, const N& dst, const E& w) {
    // shared_ptr to tuple?
    std::shared_ptr<N> src_sp = node_set.find(Node{src})->get();
    std::shared_ptr<N> dst_sp = node_set.find(Node{dst})->get();
    Edge e{src_sp,dst_sp,std::make_shared<E>(w)};

    //std::cout << "Inserting Edge: " << *src_sp << " " << *dst_sp << " " << w << "\n";
    std::set<Edge, EdgeCmp>& node_to_edge = edge_map[src_sp];
    for(const Edge& edge : node_to_edge) {
      if(*edge.src.lock() == src && *edge.dst.lock() == dst && *edge.weight == w) {
        return false;
      }
    }
    node_to_edge.insert(e);
    //edge_set.insert(e);
    return true;
  }

  bool DeleteNode(const N& node) {
    if (node_set.find(Node{node}) == node_set.end()) {
      return false;
    }
    Node to_remove = *node_set.find(Node{node});
    std::shared_ptr<N> node_sp = to_remove.get();
    // TODO: is this needed?
    //node_sp.reset();
    node_set.erase(to_remove);
    return true;
  }

  bool IsNode(const N& val) {
    return (node_set.find(val) != node_set.end());
  }

  bool IsConnected(const N& src, const N& dst) {
    std::shared_ptr<N> src_sp = node_set.find(Node{src})->get();
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
    for(const Node& node : node_set) {
      node_vector.push_back(*node);
    }
    return node_vector;
  }

  std::vector<N> GetConnected(const N& src) {
    std::shared_ptr<N> src_sp = node_set.find(Node{src})->get();
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
    std::shared_ptr<N> src_sp = node_set.find(Node{src})->get();
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
    node_set.clear();
    //edge_set.clear();
    edge_map.clear();
//    node_map.erase(node_map.begin(), node_map.end());
//    edge_map.erase(edge_map.begin(), edge_map.end());
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
    for(const Node& node : g.node_set) {
      std::cout << *node << " (\n";
      std::set<Edge, EdgeCmp> node_to_edge = g.edge_map.find(node.get())->second;
      for(const auto &[src, dst, weight] : node_to_edge) {
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
    return const_iterator{edge_map.cbegin()};
  }
  const_iterator end() const { return cend(); }
  const_iterator cend() const {
    return const_iterator{edge_map.cend()};
  }
//################################ HELPERS ######################################

//  std::shared_ptr<N> GetNode() const {
//    return std::find_if(node_set.begin(), node_set.end(), [](std::shared_ptr<N> node_sp) -> bool {
//      return *node_sp.GetNode
//    })
//  }
 private:
  std::set<Node, NodeCmp> node_set;
//  // maps input nodes into our heap-allocated equivalent
//  cant do this because we are storing a copy of N
//  std::map<N, std::shared_ptr<N>> node_map;
  //std::set<Edge, EdgeCmp> edge_set;
  std::map<std::shared_ptr<N>, std::set<Edge, EdgeCmp>, PtrCmp> edge_map;
};

}  // namespace gdwg



#include "assignments/dg/graphtpp.cpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
