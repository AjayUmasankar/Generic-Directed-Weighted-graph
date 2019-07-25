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
  Graph() : node_map(), edge_map() {}
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
      InsertEdge(src, dst, weight);
    }
  }
  Graph(typename std::initializer_list<N> il) : Graph() {
    for (auto it = std::begin(il); it != std::end(il); ++it)
      InsertNode(*it);
  }
  Graph(const Graph& other) : Graph() {
    node_map = other.node_map;
    edge_map = other.edge_map;
  }
  Graph(Graph&& other) : Graph() {
    node_map = std::move(other.node_map);
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

  struct NodeCmp {
    bool operator() (const std::shared_ptr<N> lhs, const std::shared_ptr<N> rhs) const {
      return *lhs < *rhs;
    }
  };

  struct EdgeCmp {
    // this is the comparison as well as the ordering function for the set, cant seperate the two
    // Elements a and b are considered equal iff !(a < b) && !(b < a)
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
    // if(lhs.index_ != lhs.edges_.end() && rhs.index_ != rhs.edges_.end()) {
    //   return (*lhs == *rhs);
    // } else if (lhs.index_ == lhs.edges_.end() && rhs.index_ == rhs.edges_.end()) {
    //   return true;
    // } else {
    //   return false;
    // }



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

// methods
  bool InsertNode(const N& val);
  bool InsertEdge(const N& src, const N& dst, const E& w);
  bool DeleteNode(const N& node);
  bool IsNode(const N& val);
  bool IsConnected(const N& src, const N& dst);
  std::vector<N> GetNodes();
  std::vector<N> GetConnected(const N& src);
  std::vector<E> GetWeights(const N& src, const N& dst);

  void Clear() {
    node_map.erase(node_map.begin(), node_map.end());
    edge_map.erase(edge_map.begin(), edge_map.end());
  }

// common methods (not in spec)
  bool isEmpty() {
    return node_map.empty() && edge_map.empty();
  }

  size_t numNodes() {
    return node_map.size();
  }

  size_t numEdges() {
    size_t sum = 0;
    for (const auto& n : edge_map)
      sum += n.second.size();
    return sum;
  }

// friends
  Graph<N, E>& operator=(const Graph<N, E>& other) {
    /*
      if (this != &other)
        then do the bottom?
     */
    node_map = other.node_map;
    edge_map = other.edge_map;
    return *this;
  }

  Graph<N, E>& operator=(Graph<N, E>&& other) {
    /*
      if (this != &other)
        then do the bottom?
     */
    node_map = std::move(other.node_map);
    edge_map = std::move(other.edge_map);
    other.Clear();
    return *this;
  }

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

// iterators
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

// helpers
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
