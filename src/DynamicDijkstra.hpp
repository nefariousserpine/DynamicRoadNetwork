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
 *  2) Querying distances and paths to any target (lazy repair).
 *  3) Edge-weight updates with deferred recomputation.
 *
 * On any edge update, the SPT is marked dirty; the next distance/path
 * query triggers a full Dijkstra recompute from the last source.
 */
class DynamicDijkstra {
public:
    explicit DynamicDijkstra(Graph& graph);

    // Runs (or reruns) Dijkstra from the given source node
    void compute(int source);

    // Returns distance from source to target, or INT_MAX if unreachable
    // Triggers recompute if graph has been updated since last compute
    int get_distance(int target);

    // Returns shortest path [source,...,target], or empty if unreachable
    // Triggers recompute if graph has been updated since last compute
    std::vector<int> get_shortest_path(int target);

    // Updates edge (u,v) to new_weight and defers SPT repair
    void update_edge(int u, int v, int new_weight);

private:
    Graph& graph_;
    int source_;
    bool dirty_;    // true if SPT needs recompute before next query
    std::unordered_map<int,int> dist_;
    std::unordered_map<int,int> parent_;

    // Fetches current weight of edge (u,v); throws if missing
    int get_edge_weight(int u, int v) const;
};

#endif // DYNAMICDIJKSTRA_HPP