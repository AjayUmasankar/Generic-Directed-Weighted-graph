#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <map>
#include <memory>

namespace gdwg {

template <typename N, typename E>
class Graph {
 public:
  class const_iterator {};
  bool InsertNode(const N& val) {
    std::shared_ptr<N> node{val};
  }
  bool InsertEdge(const N& src, const N& dst, const E& w) {
    // shared_ptr to tuple?
    std::shared_ptr<N> src_ptr{src};
    std::shared_ptr<N> dst_ptr{dst};

  }
  friend std::ostream& operator<<(std::ostream& os, const gdwg::Graph<N, E>& g) {

  }
 private:
  std::map<N, E>
};

}  // namespace gdwg

#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
