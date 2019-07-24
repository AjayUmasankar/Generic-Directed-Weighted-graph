#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace gdwg {

template <typename N, typename E>
bool Graph<N, E>::InsertNode(const N& val) {
  if (node_map.find(val) != node_map.end())
    return false;

  N val_copy = val;
  std::shared_ptr<N> node = std::make_shared<N>(val_copy);
  node_map.emplace(val_copy, node);

  // creates empty edge set for edges from this node
  std::set<Edge, EdgeCmp> edge_set;
  edge_map[node] = edge_set;
  return true;
}

template <typename N, typename E>
bool Graph<N, E>::InsertEdge(const N& src, const N& dst, const E& w) {
  // insert nodes if they don't exist
  // no effect if nodes already exist, see above
  InsertNode(src);
  InsertNode(dst);

  std::shared_ptr<N> src_sp = node_map.find(src)->second;
  std::shared_ptr<N> dst_sp = node_map.find(dst)->second;
  Edge e{src_sp, dst_sp, std::make_shared<E>(w)};

  //std::cout << "Inserting Edge: " << *src_sp << " " << *dst_sp << " " << w << "\n";
  std::set<Edge, EdgeCmp>& edge_set = edge_map[src_sp];
  for(const Edge& edge : edge_set) {
    if(*edge.src.lock() == src && *edge.dst.lock() == dst && *edge.weight == w)
      return false;
  }
  edge_set.insert(e);
  return true;
}

template <typename N, typename E>
bool Graph<N, E>::DeleteNode(const N& node) {
  if (node_map.find(node) == node_map.end())
    return false;

  std::shared_ptr<N> node_sp = node_map.find(node)->second;
  node_sp.reset();
  node_map.erase(node);
  return true;
}

template <typename N, typename E>
bool Graph<N, E>::IsNode(const N& val) {
  return node_map.find(val) != node_map.end();
}

template <typename N, typename E>
bool Graph<N, E>::IsConnected(const N& src, const N& dst) {
  std::shared_ptr<N> src_sp = node_map[src];
  std::set<Edge, EdgeCmp> edge_set = edge_map.find(src_sp)->second;
  for(const auto &[from, to, weight] : edge_set) {
    if(dst == *to.lock())
      return true;
  }
  return false;
}

template <typename N, typename E>
std::vector<N> Graph<N, E>::GetNodes() {
  std::vector<N> node_vector;
  for(const auto& n : node_map)
    node_vector.push_back(n.first);

  return node_vector;
}

template <typename N, typename E>
std::vector<N> Graph<N, E>::GetConnected(const N& src) {
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

template <typename N, typename E>
std::vector<E> Graph<N, E>::GetWeights(const N& src, const N& dst) {
  std::shared_ptr<N> src_sp = node_map[src];
  std::vector<E> weights;
  // TODO: this set automatically orders?
  // std::multiset<E> weights_set;
  for(Edge e : edge_map[src_sp]) {
    if(*e.dst.lock() == dst)
      weights.push_back(*e.weight);
  }
  return weights;
}

}