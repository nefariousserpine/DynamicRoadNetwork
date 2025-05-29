#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <unordered_map>
#include <vector>
#include <utility>

/**
 * @brief A simple undirected, weighted graph representation using adjacency lists.
 * 
 * Nodes are identified by integers. Edges have integer weights (e.g., travel
 * time, distance, etc.). This class supports dynamic operations: adding edges,
 * removing edges, and updating edge weights.
 */
class Graph {
public:
    Graph() = default;
    ~Graph() = default;
    Graph(const Graph&) = default;
    Graph& operator=(const Graph&) = default;

    /**
     * @brief Add an undirected edge between u and v with the given weight.
     * 
     * If the edge already exists, its weight is updated to 'weight'. Otherwise,
     * a new entry is created in both u’s and v’s adjacency lists.
     * 
     * @param u       Source node ID
     * @param v       Destination node ID
     * @param weight  Edge weight (e.g., travel time)
     */
    void add_edge(int u, int v, int weight);

    /**
     * @brief Remove the undirected edge between u and v, if it exists.
     * 
     * If there is no edge between u and v, this operation is a no-op.
     * 
     * @param u  Source node ID
     * @param v  Destination node ID
     */
    void remove_edge(int u, int v);

    /**
     * @brief Update the weight of the existing undirected edge between u and v.
     * 
     * If the edge does not exist, this method throws a std::runtime_error.
     * 
     * @param u           Source node ID
     * @param v           Destination node ID
     * @param new_weight  New weight to assign to the edge
     * @throws std::runtime_error if the edge (u, v) does not exist.
     */
    void update_weight(int u, int v, int new_weight);

    /**
     * @brief Check whether an undirected edge between u and v exists.
     * 
     * @param u  One endpoint node ID
     * @param v  The other endpoint node ID
     * @return true if an edge (u, v) exists; false otherwise.
     */
    bool edge_exists(int u, int v) const;

    /**
     * @brief Print the entire graph’s adjacency lists to stdout.
     * 
     * For each node u present in the adjacency map, prints a line:
     *   u: (v1, w1) (v2, w2) ...
     * indicating edges u→v1 with weight w1, etc.
     */
    void print_graph() const;

    /**
     * @brief Return a const reference to the neighbors of node u.
     * 
     * Neighbors are returned as a vector of (neighbor_id, weight) pairs. If u
     * does not exist in the graph (no incident edges), an empty vector is returned.
     * 
     * @param  u  Node ID whose adjacency list is requested
     * @return const std::vector<std::pair<int,int>>&  
     *         List of (neighbor_id, edge_weight) pairs for node u,
     *         or an empty vector if u has no edges.
     */
    const std::vector<std::pair<int,int>>& get_neighbors(int u) const;

private:
    // Underlying adjacency list:
    //   key   = node ID
    //   value = vector of (neighbor ID, weight) pairs
    std::unordered_map<int, std::vector<std::pair<int,int>>> adjList_;

    // A static empty vector returned when get_neighbors(u) is called on a node
    // that has no adjacency list entry.
    static const std::vector<std::pair<int,int>> EMPTY_NEIGHBORS_;
};

#endif // GRAPH_HPP
