#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <unordered_map>
#include <vector>
#include <utility>

/**
 * Represents a simple undirected, weighted graph using adjacency lists.
 */
class Graph {
public:
    Graph() = default;
    ~Graph() = default;
    Graph(const Graph&) = default;
    Graph& operator=(const Graph&) = default;

    // Add an undirected edge between nodes u and v with the specified weight.
    void add_edge(int u, int v, int weight);

    // Remove the undirected edge between nodes u and v, if it exists.
    void remove_edge(int u, int v);

    // Update the weight of the existing undirected edge between u and v.
    void update_weight(int u, int v, int new_weight);

    // Return true if there is an undirected edge between u and v.
    bool edge_exists(int u, int v) const;

    // Print the adjacency list representation of the graph.
    // Node u -> u: (v1, w1) (v2, w2) ...
    void print_graph() const;

    // Return a reference to the list of neighbors of node u.
    // Each neighbor is represented as a (neighbor_id, weight) pair.
    const std::vector<std::pair<int,int>>& get_neighbors(int u) const;

private:
    // Adjacency list where each key is a node ID and the value is a list
    // of (neighbor ID, edge weight) pairs.
    std::unordered_map<int, std::vector<std::pair<int,int>>> adjList_;

    // Shared empty vector used for nodes without neighbors.
    static const std::vector<std::pair<int,int>> EMPTY_NEIGHBORS_;
};

#endif // GRAPH_HPP
