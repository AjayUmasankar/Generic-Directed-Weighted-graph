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
  Graph() : node_set(), edge_map() {}
  Graph(typename std::vector<N>::const_iterator start, typename std::vector<N>::const_iterator end)
      : Graph() {
    for (auto it = start; it != end; it++)
      InsertNode(*start++);
  }
  Graph(typename std::vector<std::tuple<N, N, E>>::const_iterator start,
        typename std::vector<std::tuple<N, N, E>>::const_iterator end)
      : Graph() {
    for (auto it = start; it != end; ++it) {
      N src;
      N dst;
      E weight;
      std::tie(src, dst, weight) = *it;
      // insert nodes, no effect if they already exist
      InsertNode(src);
      InsertNode(dst);
      InsertEdge(src, dst, weight);
    }
  }
  Graph(typename std::initializer_list<N> il) : Graph() {
    for (auto it = std::begin(il); it != std::end(il); ++it)
      InsertNode(*it);
  }
  Graph(const Graph& other) : Graph() {
    node_set = other.node_set;
    edge_map = other.edge_map;
  }
  Graph(Graph&& other) : Graph() {
    node_set = std::move(other.node_set);
    edge_map = std::move(other.edge_map);
    other.Clear();
  }

  ~Graph() {
    // not sure if default is enough or if clear is required
    Clear();
  }

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

// methods
  bool InsertNode(const N& val);
  bool InsertEdge(const N& src, const N& dst, const E& w);
  bool DeleteNode(const N& node);
  bool IsNode(const N& val);
  bool IsConnected(const N& src, const N& dst);
  std::vector<N> GetNodes();
  std::vector<N> GetConnected(const N& src);
  std::vector<E> GetWeights(const N& src, const N& dst);
  bool Replace(const N& oldData, const N& newData);


  void Clear() {
    // TODO: Do we need to .reset() any pointers?
    node_set.clear();
    edge_map.clear();
  }

// iterator methods

  const_iterator find(const N& src, const N& dst, const E& weight) {
    //std::tuple<N&, N&, E&> = std::tie(src, dst, weight);
    for(auto it = cbegin(); it != cend(); ++it) {
      if (*it == std::tie(src, dst, weight)) {
        return it;
      }
    }
    return cend();
  }

  const_iterator erase(const_iterator it) {
    // TODO edge case of removing iterator to last element maybe
    N src = std::get<0>(*it);
    N dst = std::get<1>(*it);
    E weight = std::get<2>(*it);
    ++it;
    N src_next = std::get<0>(*it);
    N dst_next = std::get<1>(*it);
    E weight_next = std::get<2>(*it);
    erase(src, dst, weight);
    return find(src_next, dst_next, weight_next);
  }

  // TODO: can all_edges be references
  const_iterator begin() const { return cbegin(); }
  const_iterator cbegin() const {
    return const_iterator{edge_map.cbegin()};
  }
  const_iterator end() const { return cend(); }
  const_iterator cend() const {
    return const_iterator{edge_map.cend()};
  }

// friends
  Graph<N, E>& operator=(const Graph<N, E>& other) {
    /*
      if (this != &other)
        then do the bottom?
     */
    node_set = other.node_map;
    edge_map = other.edge_map;
    return *this;
  }

  Graph<N, E>& operator=(Graph<N, E>&& other) {
    /*
      if (this != &other)
        then do the bottom?
     */
    node_set = std::move(other.node_map);
    edge_map = std::move(other.edge_map);
    other.Clear();
    return *this;
  }

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

// common methods (not in spec)
  bool isEmpty() {
    return node_set.empty() && edge_map.empty();
  }

  size_t numNodes() {
    return node_set.size();
  }

  size_t numEdges() {
    size_t sum = 0;
    for (const auto& n : edge_map)
      sum += n.second.size();
    return sum;
  }



 private:
  std::set<Node, NodeCmp> node_set;
//  // maps input nodes into our heap-allocated equivalent
//  cant do this because we are storing a copy of N
//  std::map<N, std::shared_ptr<N>> node_map;
  //std::set<Edge, EdgeCmp> edge_set;
  std::map<std::shared_ptr<N>, std::set<Edge, EdgeCmp>, PtrCmp> edge_map;
};

}  // namespace gdwg



#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
