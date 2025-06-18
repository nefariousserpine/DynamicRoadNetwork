#ifndef DYNAMICDIJKSTRA_HPP
#define DYNAMICDIJKSTRA_HPP

#include "Graph.hpp"
#include <unordered_map>
#include <vector>

/**
 * Maintains single-source shortest paths under edge-weight updates.
 * 
 * Supports:
 *  1) Initial Dijkstra run from a source node.
 *  2) Querying distances and paths to any target.
 *  3) Edge-weight updates with localized repair or full recomputation.
 * 
 * If an edge on the SPT increases in weight, we recompute Dijkstra.
 * If an edge's weight decreases, we perform a localized push update.
 */
class DynamicDijkstra {
public:
    // Constructs the object over the given graph
    explicit DynamicDijkstra(Graph& graph);

    // Runs Dijkstra from the given source node
    void compute(int source);

    // Returns distance from source to target, or INT_MAX if unreachable
    int get_distance(int target) const;

    // Returns shortest path [source, ..., target], or empty if unreachable
    std::vector<int> get_shortest_path(int target) const;

    // Updates edge (u,v) to new_weight and repairs SPT accordingly
    void update_edge(int u, int v, int new_weight);

private:
    Graph& graph_;                             // Underlying graph reference
    int source_;                               // Last source used in compute()
    std::unordered_map<int,int> dist_;         // Distance from source to each node
    std::unordered_map<int,int> parent_;       // Shortest-path parent of each node

    // Returns weight of edge (u,v); throws if edge doesn't exist
    int get_edge_weight(int u, int v) const;
};

#endif 
