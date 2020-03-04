# Generic-Directed-Weighted-graph

A Generic Directed Weighted graph (GDWG) with value-like semantics in C++. Both the data stored at a node and the weight stored at an edge are of generic types. Both generic types may be different. For example, here is a graph with nodes storing std::string and edges weighted by int: gdwg::Graph<std::string, int> g;

The graph has various methods such as inserting/deleting nodes/edges, replacing, mergereplacing, IsConnected (checks if an edge from src to dst exists), erase, etc. The full scope of the task is specified in Spec.pdf.

This is implemented using smart pointers such that each node and edge have only one underlying resource (in the heap).   
Operator usages are modified to used to implement equality and cout (<<).  
A bidirectional iterator also enables us to traverse this graph in increasing edge weights.
