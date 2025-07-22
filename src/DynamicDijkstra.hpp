#ifndef DYNAMICDIJKSTRA_HPP
#define DYNAMICDIJKSTRA_HPP

#include "Graph.hpp"
#include <unordered_map>
#include <vector>

/**
 * Maintains single-source shortest paths under edge-weight updates.
 * On any edge update, the SPT is marked dirty; the next distance/path
 * query triggers a full Dijkstra recompute from the last source.
 */
class DynamicDijkstra {
public:
    explicit DynamicDijkstra(Graph& graph);

    // Run Dijkstra from the given source node
    void compute(int source);

    // Return distance from source to target, or INT_MAX if unreachable
    // Trigger recompute if graph was updated since last compute
    int get_distance(int target);

    // Return shortest path source -> target, or empty if unreachable
    // Trigger recompute if graph was updated since last compute
    std::vector<int> get_shortest_path(int target);

    // Update edge (u,v) to new_weight and defer SPT repair
    void update_edge(int u, int v, int new_weight);

private:
    Graph& graph_;
    int source_;
    bool dirty_;    // true if SPT requires recompute before next query
    std::unordered_map<int,int> dist_;
    std::unordered_map<int,int> parent_;

    // Fetch current weight of edge (u,v)
    int get_edge_weight(int u, int v) const;
};

#endif // DYNAMICDIJKSTRA_HPP
