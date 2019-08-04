#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>


namespace gdwg {



template <typename N, typename E>
class Graph {
 private:
  struct Edge {
    std::weak_ptr<N> src_;
    std::weak_ptr<N> dst_;
    std::shared_ptr<E> weight_;
    friend bool operator==(const Edge& lhs, const Edge& rhs) {
      return *lhs.src_.lock() == *rhs.src_.lock() && *lhs.dst_.lock() == *rhs.dst_.lock() && *lhs.weight_ == *rhs.weight_;
    }
  };
  class Node {
   public:
    Node(N value) : sptr_{std::make_shared<N>(value)} {};
    N& operator* () const { return *sptr_; }
    std::shared_ptr<N> get() const { return sptr_; }

//    bool operator<(const Node& other) const {
//      return *this < *other;
//    }
    friend bool operator==(const Node& lhs, const Node& rhs) {
      return *lhs.sptr_ == *rhs.sptr_;
    }

   private:
    std::shared_ptr<N> sptr_;
  };


  struct NodeCmp {
    bool operator() (const Node& lhs, const Node& rhs) const {
      return *lhs < *rhs;
    }
  };

  // this is the comparison as well as the ordering function for the set, cant seperate the two
  struct EdgeCmp {
    bool operator() (const Edge lhs, const Edge rhs) const {
      N src1 = *lhs.src_.lock();
      N src2 = *rhs.src_.lock();
      if (src1 == src2) {
        N dst1 = *lhs.dst_.lock();
        N dst2 = *rhs.dst_.lock();
        if(dst1 == dst2) {
          E w1 = *lhs.weight_;
          E w2 = *rhs.weight_;
          return w1 < w2;
        }
        return dst1 < dst2;
      }
      return src1 < src2;
    }
  };

 public:
  Graph() : edge_map_() {}
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
    edge_map_ = other.edge_map_;
  }
  Graph(Graph&& other) : Graph() {
    edge_map_ = std::move(other.edge_map_);
    other.Clear();
  }

  ~Graph() {
    // not sure if default is enough or if clear is required
    Clear();
  }

  Graph<N, E>& operator=(const Graph<N, E>& other) const {
    /*
      if (this != &other)
        then do the bottom?
     */
    edge_map_ = other.edge_map_;
    return *this;
  }

  Graph<N, E>& operator=(Graph<N, E>&& other) const {
    /*
      if (this != &other)
        then do the bottom?
     */
    edge_map_ = std::move(other.edge_map_);
    other.Clear();
    return *this;
  }


  class const_iterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::tuple<N, N, E>;
    using reference = std::tuple<const N&, const N&, const E&>;
    using pointer = std::tuple<N, N, E>*;
    using difference_type = int;

    reference operator*() const {
      Edge cur = *inner_;
//      if(cur.src.expired() || cur.dst.expired()) {
//        std::cout << "Expired ... " << std::endl;
//      }
      return {*cur.src_.lock(), *cur.dst_.lock(), *cur.weight_};
    }

    pointer operator->() const { return &(operator*()); }

    const_iterator operator++() {
      // Undefined behaviour if ++ on end()
      ++inner_;
      if(inner_ == outer_->second.cend()) {
        // skip nodes with no edges
        while(++outer_ == outer_end_ || outer_->second.empty()) {
          // or else keeps looping forever
          if (outer_ == outer_end_) {
            inner_ = (--outer_)->second.cend();
            ++outer_;
            return *this;
          }
        }
        inner_ = outer_->second.cbegin();
      }
      return *this;
    }

    const_iterator operator++(int) {
      auto copy{*this};
      ++(*this);
      return copy;
    }

    const_iterator operator--() {
      // assuming more than one edge in graph, else unexpected behaviour
      if(outer_ == outer_end_) {
        --outer_;
        inner_ = outer_->second.cend();
      }
      if(inner_ == outer_->second.cbegin()) {
        // skip all empty nodes
        while(outer_ == outer_begin_ || (--outer_)->second.empty()) {
          if (outer_ == outer_begin_) { return *this; }
        }
        inner_ = outer_->second.cend();
      }
      --inner_;
      return *this;
    }
    const_iterator operator--(int) {
      auto copy{*this};
      --(*this);
      return copy;
    }

    friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) {
      return lhs.outer_ == rhs.outer_ && lhs.inner_ == rhs.inner_;
    }

    friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) {
      return !(lhs == rhs);
    }

   private:
    typename std::map<Node, std::set<Edge, EdgeCmp>, NodeCmp>::const_iterator outer_;
    const typename std::map<Node, std::set<Edge, EdgeCmp>, NodeCmp>::const_iterator outer_begin_;
    const typename std::map<Node, std::set<Edge, EdgeCmp>, NodeCmp>::const_iterator outer_end_;
    typename std::set<Edge, EdgeCmp>::const_iterator inner_;

    friend class Graph;

    explicit const_iterator(const decltype(outer_)& outer,  const decltype(outer_begin_)& outer_begin, const decltype(outer_end_)& outer_end) : outer_ {outer}, outer_begin_{outer_begin}, outer_end_{outer_end} {
      // instead of passing in inner index, we can do this instead to deduce the inner index
      if(outer_begin_ == outer_end_) {
        // if empty graph, dont do anything (everything is undefined behaviour)
      } else if (outer_ == outer_end_) {
        // cend is sent in, so set end indexes
        inner_ = (--outer_)->second.cend();
        ++outer_;
      } else if (outer_ == outer_begin_) {
        // cbegin is sent in, set first element index
        inner_ = outer_->second.cbegin();
      }
    }
  };

// methods
  bool InsertNode(const N& val);
  bool InsertEdge(const N& src, const N& dst, const E& w);
  bool DeleteNode(const N& node);
  bool IsNode(const N& val) const;
  bool IsConnected(const N& src, const N& dst) const;
  std::vector<N> GetNodes() const;
  std::vector<N> GetConnected(const N& src) const;
  std::vector<E> GetWeights(const N& src, const N& dst) const;
  bool Replace(const N& oldData, const N& newData);
  void MergeReplace(const N& oldData, const N& newData);
  const_iterator cbegin() const;
  const_iterator cend() const;
// iterator methods

  void Clear() {
    edge_map_.clear();
  }


  const_iterator find(const N& src, const N& dst, const E& weight) const {
    for(auto it = cbegin(); it != cend(); ++it) {
      if (*it == std::tie(src, dst, weight)) {
        return it;
      }
    }
    return cend();
  }


  bool erase(const N& src, const N& dst, const E& w) {
    // if edge exists, delete it and return true. Else, false
    auto it = edge_map_.find(Node{src});
    if(it == edge_map_.end()) {
      return false;
    }

    std::set<Edge, EdgeCmp>& edge_set = it->second;
    auto edge_it = std::find_if(edge_set.cbegin(), edge_set.cend(),
                                [=] (const Edge& e) {
                                  return *e.dst_.lock() == dst && *e.weight_ == w;
                                });

    if(edge_it == edge_set.end()) {
      return false;
    } else {
      edge_set.erase(edge_it);
      return true;
    }
//      if(*edge.dst.lock() == dst && *edge.weight == w) {
//        edge_set.erase(edge);
//        erased = true;
//        break;
//      }
//    }
//    return erased;
  }

  const_iterator erase(const_iterator it) {
    if(it == cend()) {
      return it;
    }
    N src;
    N dst;
    E weight;
    std::tie(src, dst, weight) = *it++;
    if(it == cend()) {
      erase(src, dst, weight);
      return cend();
    }
    N src_next;
    N dst_next;
    E weight_next;
    std::tie(src_next, dst_next, weight_next) = *it;
    erase(src, dst, weight);
    return find(src_next, dst_next, weight_next);
  }

  const_iterator begin() const { return cbegin(); }
  const_iterator end() const { return cend(); }


  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  const_reverse_iterator rbegin() const { return crbegin(); }
  const_reverse_iterator rend() const { return crend(); }
  const_reverse_iterator crbegin() const { return const_reverse_iterator{cend()}; }
  const_reverse_iterator crend() const { return const_reverse_iterator{cbegin()}; }

// friends

  friend bool operator==(const gdwg::Graph<N, E>& g1, const gdwg::Graph<N, E>& g2) {
    return g1.edge_map_ == g2.edge_map_;
  }
  friend bool operator!=(const gdwg::Graph<N, E>& g1, const gdwg::Graph<N, E>& g2) {
    return !(g1==g2);
  }
  friend std::ostream& operator<<(std::ostream& os, const gdwg::Graph<N, E>& g) {
    for(const auto &[key, val]  : g.edge_map_) {
      std::cout << *key << " (\n";
      std::set<Edge, EdgeCmp> node_to_edge = val;
      for(const auto &[src, dst, weight] : node_to_edge) {
        std::cout << "  " << *dst.lock() << " | " << *weight << "\n";
      }
      std::cout << ")\n";
    }
    return os;
  }

// common methods (not in spec)
  bool IsEmpty() const {
    return edge_map_.empty();
  }

  size_t NumNodes() const {
    return edge_map_.size();
  }

  size_t NumEdges() const {
    size_t sum = 0;
    for (const auto& n : edge_map_)
      sum += n.second.size();
    return sum;
  }



 private:
  std::map<Node, std::set<Edge, EdgeCmp>, NodeCmp> edge_map_;
};

}  // namespace gdwg



#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
