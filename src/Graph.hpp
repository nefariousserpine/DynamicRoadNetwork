#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <unordered_map>
#include <vector>
#include <utility>

/**
 * Represents a simple undirected, weighted graph using adjacency lists.
 * 
 * Nodes are identified by integers. Edges have integer weights (e.g., travel
 * time, distance, etc.). This class supports dynamic operations like adding,
 * removing, and updating edges.
 */
class Graph {
public:
    Graph() = default;
    ~Graph() = default;
    Graph(const Graph&) = default;
    Graph& operator=(const Graph&) = default;

    // Adds an undirected edge between nodes u and v with the specified weight.
    // If the edge already exists, its weight is updated.
    void add_edge(int u, int v, int weight);

    // Removes the undirected edge between nodes u and v, if it exists.
    // If no such edge exists, this operation has no effect.
    void remove_edge(int u, int v);

    // Updates the weight of the existing undirected edge between u and v.
    // Throws std::runtime_error if the edge does not exist.
    void update_weight(int u, int v, int new_weight);

    // Returns true if there is an undirected edge between u and v.
    bool edge_exists(int u, int v) const;

    // Prints the adjacency list representation of the graph.
    // For each node u, it prints: u: (v1, w1) (v2, w2) ...
    void print_graph() const;

    // Returns a reference to the list of neighbors of node u.
    // Each neighbor is represented as a (neighbor_id, weight) pair.
    // If u has no edges, returns an empty vector.
    const std::vector<std::pair<int,int>>& get_neighbors(int u) const;

private:
    // Adjacency list where each key is a node ID and the value is a list
    // of (neighbor ID, edge weight) pairs.
    std::unordered_map<int, std::vector<std::pair<int,int>>> adjList_;

    // Shared empty vector used for nodes without neighbors.
    static const std::vector<std::pair<int,int>> EMPTY_NEIGHBORS_;
};

#endif // GRAPH_HPP
