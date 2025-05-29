#ifndef DYNAMICDIJKSTRA_HPP
#define DYNAMICDIJKSTRA_HPP

#include "Graph.hpp"
#include <unordered_map>
#include <vector>

/**
 * @brief Maintains single‐source shortest paths in a graph under edge‐weight updates.
 * 
 * This class supports:
 *  1) An initial Dijkstra run from a given source (compute).
 *  2) Querying the shortest‐path distance and actual path to any target (get_distance / get_shortest_path).
 *  3) Updating the weight of an existing edge and repairing the current shortest‐path tree (update_edge).
 * 
 * Note: For simplicity, if an edge’s weight increases and that edge lies on the current SPT,
 *       we recompute Dijkstra from scratch. When an edge’s weight decreases, we perform a
 *       localized “push‐down” update from the affected endpoint.
 */
class DynamicDijkstra {
public:
    /**
     * @brief Construct a DynamicDijkstra over the given graph.
     * @param graph  Reference to a Graph object that supports add_edge, remove_edge, update_weight, get_neighbors.
     */
    explicit DynamicDijkstra(Graph& graph);

    /**
     * @brief Run (or re‐run) standard Dijkstra from the specified source node.
     * 
     * After calling this, get_distance(target) and get_shortest_path(target) will refer
     * to the distances/paths from this source to every reachable node.
     * 
     * @param source  The source node ID for SSSP.
     */
    void compute(int source);

    /**
     * @brief Return the shortest‐path distance from the last compute(source) to target.
     * @param target  The destination node ID.
     * @return If target is reachable, the shortest distance; otherwise INT_MAX.
     */
    int get_distance(int target) const;

    /**
     * @brief Return the actual shortest‐path (as a vector of node IDs) from the last source to target.
     * @param target  The destination node ID.
     * @return A vector of node IDs [source, …, target]. If target is unreachable, returns an empty vector.
     */
    std::vector<int> get_shortest_path(int target) const;

    /**
     * @brief Update the weight of an existing undirected edge (u,v) to new_weight, then repair SPT.
     * 
     * If new_weight < old_weight, we attempt a localized “decrease‐weight” update:
     *   • If going through u → v yields a shorter path to v, we push v (and its affected descendants) through
     *     a small Dijkstra “push” starting at v.
     * 
     * If new_weight > old_weight and (u,v) lies on the current shortest‐path tree, we simply rerun compute(source)
     * from scratch (simplest correct approach). Otherwise, no action is needed.
     * 
     * @param u           One endpoint of the existing edge.
     * @param v           The other endpoint.
     * @param new_weight  The updated weight to assign to edge (u,v).
     * @throws std::runtime_error if (u,v) does not exist in the graph.
     */
    void update_edge(int u, int v, int new_weight);

private:
    Graph& graph_;                             // Reference to the underlying graph.
    int source_;                               // The last‐used source ID (or –1 if none).
    std::unordered_map<int,int> dist_;         // Current distance from source_ to each node.
    std::unordered_map<int,int> parent_;       // parent_[v] = predecessor of v in the SPT.

    /**
     * @brief Find the current weight of edge (u,v) by scanning graph_.get_neighbors(u).
     * 
     * @param u  One endpoint.
     * @param v  Other endpoint.
     * @return The weight of (u,v).
     * @throws std::runtime_error if edge (u,v) does not exist.
     */
    int get_edge_weight(int u, int v) const;
};

#endif // DYNAMICDIJKSTRA_HPP
