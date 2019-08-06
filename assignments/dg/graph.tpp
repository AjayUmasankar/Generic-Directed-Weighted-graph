#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>

namespace gdwg {

template <typename N, typename E>
bool Graph<N, E>::InsertNode(const N& val) {
  if (edge_map_.find(Node{val}) != edge_map_.end()) {
    return false;
  }
  Node new_node{val};
  std::set<Edge, EdgeCmp> edge_set;
  edge_map_.emplace(new_node, edge_set);
  return true;
}

template <typename N, typename E>
bool Graph<N, E>::InsertEdge(const N& src, const N& dst, const E& w) {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::runtime_error(
        "Cannot call Graph::InsertEdge when either src or dst node does not exist");
  }

  Node src_node = edge_map_.find(Node{src})->first;
  Node dst_node = edge_map_.find(Node{dst})->first;
  Edge e{src_node.get(), dst_node.get(), std::make_shared<E>(w)};

  // std::cout << "Inserting Edge: " << *src_sp << " " << *dst_sp << " " << w << "\n";
  std::set<Edge, EdgeCmp>& edge_set = edge_map_.find(Node{src})->second;
  for (const Edge& edge : edge_set) {
    if (*edge.src_.lock() == src && *edge.dst_.lock() == dst && *edge.weight_ == w) {
      return false;
    }
  }
  edge_set.insert(e);
  return true;
}

template <typename N, typename E>
bool Graph<N, E>::DeleteNode(const N& node) noexcept {
  if (edge_map_.find(Node{node}) == edge_map_.end()) {
    return false;
  }
  edge_map_.erase(Node{node});
  for (auto it = edge_map_.begin(); it != edge_map_.end(); ++it) {
    std::set<Edge, EdgeCmp>& edge_set = it->second;
    for (auto edge_it = edge_set.begin(); edge_it != edge_set.end();) {
      if (edge_it->dst_.expired()) {
        edge_it = edge_set.erase(edge_it);
      } else {
        ++edge_it;
      }
    }
  }
  return true;
}

template <typename N, typename E>
bool Graph<N, E>::IsNode(const N& val) const noexcept {
  return (edge_map_.find(Node{val}) != edge_map_.end());
}

template <typename N, typename E>
bool Graph<N, E>::IsConnected(const N& src, const N& dst) const {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::runtime_error(
        "Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
  }
  std::set<Edge, EdgeCmp> edge_set = edge_map_.find(Node{src})->second;
  for (const auto& [from, to, weight] : edge_set) {
    if (dst == *to.lock()) {
      return true;
    }
  }
  return false;
}

template <typename N, typename E>
std::vector<N> Graph<N, E>::GetNodes() const noexcept {
  std::vector<N> node_vector;
  for (const auto& key : edge_map_) {
    node_vector.push_back(*key.first);
  }
  return node_vector;
}

template <typename N, typename E>
std::vector<N> Graph<N, E>::GetConnected(const N& src) const {
  if (!IsNode(src)) {
    throw std::out_of_range("Cannot call Graph::GetConnected if src doesn't exist in the graph");
  }
  std::set<N> seen_nodes;
  // std::set<Edge,
  for (Edge e : edge_map_.find(Node{src})->second) {
    seen_nodes.insert(*e.dst_.lock());
  }
  std::vector<N> connected_nodes{seen_nodes.begin(), seen_nodes.end()};
  return connected_nodes;
}

template <typename N, typename E>
std::vector<E> Graph<N, E>::GetWeights(const N& src, const N& dst) const {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::out_of_range(
        "Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
  }
  std::vector<E> weights;
  for (Edge e : edge_map_.find(Node{src})->second) {
    if (*e.dst_.lock() == dst) {
      weights.push_back(*e.weight_);
    }
  }
  return weights;
}

template <typename N, typename E>
bool Graph<N, E>::Replace(const N& oldData, const N& newData) {
  if (edge_map_.find(Node{oldData}) == edge_map_.end()) {
    throw std::runtime_error("Cannot call Graph::Replace on a node that doesn't exist");
  } else if (edge_map_.find(Node{newData}) != edge_map_.end()) {
    return false;
  }
  InsertNode(newData);
  std::set<Edge, EdgeCmp> old_edges = edge_map_.find(Node{oldData})->second;
  for (const auto& Edge : old_edges) {
    InsertEdge(newData, *Edge.dst_.lock(), *Edge.weight_);
  }
  return DeleteNode(oldData);
  // return true;
}

template <typename N, typename E>
void Graph<N, E>::MergeReplace(const N& oldData, const N& newData) {
  if (!IsNode(oldData) || !IsNode(newData)) {
    throw std::runtime_error(
        "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
  }

  // get outgoing edges of src_node (src_node -> other_node)
  // replace src_node with dest node and insert new edge into graph
  std::set<Edge, EdgeCmp> src_edges = edge_map_.find(Node{oldData})->second;
  for (const auto& e : src_edges) {
    InsertEdge(newData, *e.dst_.lock(), *e.weight_);
  }

  // get incoming edges of src_node (other_node -> src_node)
  // replace src_node with dest node and insert new edge into graph
  for (auto it = edge_map_.begin(); it != edge_map_.end(); ++it) {
    std::set<Edge, EdgeCmp>& edge_set = it->second;
    for (auto edge_it = edge_set.begin(); edge_it != edge_set.end();) {
      if (*edge_it->src_.lock() != newData && *edge_it->dst_.lock() == oldData) {
        InsertEdge(*edge_it->src_.lock(), newData, *edge_it->weight_);
        edge_it = edge_set.erase(edge_it);
      } else {
        ++edge_it;
      }
    }
  }
  // delete oldData and its edges
  DeleteNode(oldData);
}

// iterator methods
// template <typename N, typename E>
// bool Graph<N, E>::IsNode(const N& val) {
//  return (edge_map_.find(Node{val}) != edge_map_.end());
//}

template <typename N, typename E>
typename Graph<N, E>::const_iterator Graph<N, E>::cbegin() const noexcept {
  auto first_edge_it = std::find_if(
      edge_map_.cbegin(), edge_map_.cend(),
      [](const std::pair<Node, std::set<Edge, EdgeCmp>>& item) { return !item.second.empty(); });
  return Graph<N, E>::const_iterator{first_edge_it, first_edge_it, edge_map_.cend()};
}

template <typename N, typename E>
typename Graph<N, E>::const_iterator Graph<N, E>::cend() const noexcept {
  auto first_edge_it = std::find_if(
      edge_map_.cbegin(), edge_map_.cend(),
      [](const std::pair<Node, std::set<Edge, EdgeCmp>>& item) { return !item.second.empty(); });
  return Graph<N, E>::const_iterator{edge_map_.cend(), first_edge_it, edge_map_.cend()};
}

template <typename N, typename E>
typename Graph<N, E>::const_iterator
Graph<N, E>::erase(typename Graph<N, E>::const_iterator it) noexcept {
  if (it == Graph<N, E>::cend()) {
    return it;
  }
  N src;
  N dst;
  E weight;
  std::tie(src, dst, weight) = *it++;
  if (it == Graph<N, E>::cend()) {
    erase(src, dst, weight);
    return Graph<N, E>::cend();
  }
  N src_next;
  N dst_next;
  E weight_next;
  std::tie(src_next, dst_next, weight_next) = *it;
  erase(src, dst, weight);
  return find(src_next, dst_next, weight_next);
}

template <typename N, typename E>
bool Graph<N, E>::erase(const N& src, const N& dst, const E& w) noexcept {
  // if edge exists, delete it and return true. Else, false
  auto it = edge_map_.find(Node{src});
  if (it == edge_map_.end()) {
    return false;
  }

  std::set<Edge, EdgeCmp>& edge_set = it->second;
  auto edge_it = std::find_if(edge_set.cbegin(), edge_set.cend(), [=](const Edge& e) {
    return *e.dst_.lock() == dst && *e.weight_ == w;
  });

  if (edge_it == edge_set.end()) {
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

}  // namespace gdwg
