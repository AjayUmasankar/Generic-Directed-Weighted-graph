#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace gdwg {

template <typename N, typename E>
bool Graph<N, E>::InsertNode(const N& val) {
  if(edge_map.find(Node{val}) != edge_map.end()) {
    return false;
  }
  Node new_node{val};
  std::set<Edge, EdgeCmp> edge_set;
  edge_map.emplace(new_node, edge_set);
  return true;
}


template <typename N, typename E>
bool Graph<N, E>::InsertEdge(const N& src, const N& dst, const E& w) {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::runtime_error("Cannot call Graph::InsertEdge when either src or dst node does not exist");
  }

  Node src_node = edge_map.find(Node{src})->first;
  Node dst_node = edge_map.find(Node{dst})->first;
  Edge e{src_node.get(),dst_node.get(),std::make_shared<E>(w)};

  //std::cout << "Inserting Edge: " << *src_sp << " " << *dst_sp << " " << w << "\n";
  std::set<Edge, EdgeCmp>& edge_set = edge_map.find(Node{src})->second;
  for(const Edge& edge : edge_set) {
    if(*edge.src.lock() == src && *edge.dst.lock() == dst && *edge.weight == w) {
      return false;
    }
  }
  edge_set.insert(e);
  return true;
}


template <typename N, typename E>
bool Graph<N, E>::DeleteNode(const N& node) {
  if (edge_map.find(Node{node}) == edge_map.end()) {
    return false;
  }
  Node to_remove = *edge_map.find(Node{node})->first;
  std::shared_ptr<N> node_sp = to_remove.get();
  for (auto it = edge_map.begin(); it != edge_map.end(); ++it) {
    std::set<Edge,EdgeCmp>& edge_set = it->second;
    for(auto edge_it = edge_set.begin(); edge_it != edge_set.end();) {
      if (*edge_it->dst.lock() == node) {
        edge_it = edge_set.erase(edge_it);
      } else {
        ++edge_it;
      }
    }
  }
  edge_map.erase(to_remove);
  return true;
}


template <typename N, typename E>
bool Graph<N, E>::IsNode(const N& val) {
  return (edge_map.find(Node{val}) != edge_map.end());
}


template <typename N, typename E>
bool Graph<N, E>::IsConnected(const N& src, const N& dst) {
  if(!IsNode(src) || !IsNode(dst)) {
    throw std::runtime_error("Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
  }
  std::set<Edge, EdgeCmp> edge_set = edge_map.find(Node{src})->second;
  for(const auto &[from, to, weight] : edge_set) {
    if(dst == *to.lock()) {
      return true;
    }
  }
  return false;
}


template <typename N, typename E>
std::vector<N> Graph<N, E>::GetNodes() {
  std::vector<N> node_vector;
  for(const auto [key, val] : edge_map) {
    node_vector.push_back(*key);
  }
  return node_vector;
}


template <typename N, typename E>
std::vector<N> Graph<N, E>::GetConnected(const N& src) {
  if (!IsNode(src)) {
    throw std::out_of_range("Cannot call Graph::GetConnected if src doesn't exist in the graph");
  }
  std::set<N> seen_nodes;
  //std::set<Edge,
  for(Edge e : edge_map.find(Node{src})->second) {
    seen_nodes.insert(*e.dst.lock());
  }
  std::vector<N> connected_nodes{seen_nodes.begin(), seen_nodes.end()};
  return connected_nodes;
}


template <typename N, typename E>
std::vector<E> Graph<N, E>::GetWeights(const N& src, const N& dst) {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::out_of_range("Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
  }
  std::vector<E> weights;
  for(Edge e : edge_map.find(Node{src})->second) {
    if(*e.dst.lock() == dst) {
      weights.push_back(*e.weight);
    }
  }
  return weights;
}


template <typename N, typename E>
bool Graph<N,E>::Replace(const N& oldData, const N& newData) {
  if (edge_map.find(Node{oldData}) == edge_map.end()) {
    throw std::runtime_error("Cannot call Graph::Replace on a node that doesn't exist");
  } else if (edge_map.find(Node{newData}) != edge_map.end()) {
    return false;
  }
  InsertNode(newData);
  std::set<Edge, EdgeCmp> old_edges = edge_map.find(Node{oldData})->second;
  for (const auto& Edge : old_edges) {
    InsertEdge(newData, *Edge.dst.lock(), *Edge.weight);
  }
  DeleteNode(oldData);
  return true;
}


template <typename N, typename E>
void Graph<N,E>::MergeReplace(const N& oldData, const N& newData) {
  if (!IsNode(oldData) || !IsNode(newData)) {
    throw std::runtime_error("Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
  }

  // get outgoing edges of src_node (src_node -> other_node)
  // replace src_node with dest node and insert new edge into graph
  std::set<Edge, EdgeCmp> src_edges = edge_map.find(Node{oldData})->second;
  for (const auto& e : src_edges) {
    InsertEdge(newData, *e.dst.lock(), *e.weight);
  }

  // get incoming edges of src_node (other_node -> src_node)
  // replace src_node with dest node and insert new edge into graph
  for (auto it = edge_map.begin(); it != edge_map.end(); ++it) {
    std::set<Edge,EdgeCmp>& edge_set = it->second;
    for (auto edge_it = edge_set.begin(); edge_it != edge_set.end();) {
      if (*edge_it->src.lock() != newData && *edge_it->dst.lock() == oldData) {
        InsertEdge(*edge_it->src.lock(), newData, *edge_it->weight);
        edge_it = edge_set.erase(edge_it);
      } else {
        ++edge_it;
      }
    }
  }
  // delete oldData and its edges
  DeleteNode(oldData);
}


}