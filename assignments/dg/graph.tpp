#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace gdwg {

template <typename N, typename E>
bool Graph<N, E>::InsertNode(const N& val) {
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





template <typename N, typename E>
bool Graph<N, E>::InsertEdge(const N& src, const N& dst, const E& w) {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::runtime_error("Cannot call Graph::InsertEdge when either src or dst node does not exist");
  }

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

template <typename N, typename E>
bool Graph<N, E>::DeleteNode(const N& node) {
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

template <typename N, typename E>
bool Graph<N, E>::IsNode(const N& val) {
  return (node_set.find(val) != node_set.end());
}


template <typename N, typename E>
bool Graph<N, E>::IsConnected(const N& src, const N& dst) {
  std::shared_ptr<N> src_sp = node_set.find(Node{src})->get();
  std::set<Edge, EdgeCmp> edge_set = edge_map.find(src_sp)->second;
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
  for(const Node& node : node_set) {
    node_vector.push_back(*node);
  }
  return node_vector;
}



template <typename N, typename E>
std::vector<N> Graph<N, E>::GetConnected(const N& src) {
  if (!IsNode(src)) {
    throw std::out_of_range("Cannot call Graph::GetConnected if src doesn't exist in the graph");
  }
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


template <typename N, typename E>
std::vector<E> Graph<N, E>::GetWeights(const N& src, const N& dst) {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::out_of_range("Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
  }
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

template <typename N, typename E>
bool Graph<N,E>::Replace(const N& oldData, const N& newData) {
  InsertNode(newData);
  std::shared_ptr<N> src_sp = node_set.find(Node{oldData})->get();
  std::set<Edge, EdgeCmp> old_edges = edge_map.find(src_sp)->second;
  for (const auto& Edge : old_edges) {
    InsertEdge(newData, *Edge.dst.lock(), *Edge.weight);
  }
  DeleteNode(oldData);
  return true;
}

}