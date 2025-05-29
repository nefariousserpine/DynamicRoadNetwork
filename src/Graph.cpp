#include "Graph.hpp"
#include <iostream>
#include <algorithm>    
#include <stdexcept>    

// Definition of the static empty neighbors vector
const std::vector<std::pair<int,int>> Graph::EMPTY_NEIGHBORS_ = {};

/**
 * @brief Add or update an undirected edge between u and v with the specified weight.
 *
 * If an edge (u, v) already exists in either adjacency list, its weight is updated.
 * Otherwise, a new entry is appended to both u’s and v’s adjacency vectors.
 */
void Graph::add_edge(int u, int v, int weight) {
    auto& neighbors_u = adjList_[u];  // This creates an empty list for u if it does not exist
    bool updated_u = false;
    for (auto& p : neighbors_u) {
        if (p.first == v) {
            p.second = weight;
            updated_u = true;
            break;
        }
    }
    if (!updated_u) {
        // Edge did not exist, so add it
        neighbors_u.emplace_back(v, weight);
    }

    auto& neighbors_v = adjList_[v];  // This creates an empty list for v if it does not exist
    bool updated_v = false;
    for (auto& p : neighbors_v) {
        if (p.first == u) {
            p.second = weight;
            updated_v = true;
            break;
        }
    }
    if (!updated_v) {
        // Edge did not exist, so add it
        neighbors_v.emplace_back(u, weight);
    }
}

/**
 * @brief Remove the undirected edge between u and v.
 *
 * If node u or node v does not exist, or if the edge is not present, does nothing.
 */
void Graph::remove_edge(int u, int v) {
    // ----- Remove v from u's adjacency list -----
    auto it_u = adjList_.find(u);
    if (it_u != adjList_.end()) {
        auto& neighbors_u = it_u->second;
        neighbors_u.erase(
            std::remove_if(
                neighbors_u.begin(),
                neighbors_u.end(),
                [v](const std::pair<int,int>& p) { return p.first == v; }
            ),
            neighbors_u.end()
        );
        // erase u if no neighbors remain:
        if (neighbors_u.empty()) adjList_.erase(it_u);
    }

    // ----- Remove u from v's adjacency list -----
    auto it_v = adjList_.find(v);
    if (it_v != adjList_.end()) {
        auto& neighbors_v = it_v->second;
        neighbors_v.erase(
            std::remove_if(
                neighbors_v.begin(),
                neighbors_v.end(),
                [u](const std::pair<int,int>& p) { return p.first == u; }
            ),
            neighbors_v.end()
        );
        // erase v if no neighbors remain:
        if (neighbors_v.empty()) adjList_.erase(it_v);
    }
}

/**
 * @brief Update the weight of the existing undirected edge (u, v).
 *
 * If the edge does not exist in either adjacency list, throws a runtime_error.
 */
void Graph::update_weight(int u, int v, int new_weight) {
    bool found = false;

    // ----- Update weight in u's adjacency list -----
    auto it_u = adjList_.find(u);
    if (it_u != adjList_.end()) {
        for (auto& p : it_u->second) {
            if (p.first == v) {
                p.second = new_weight;
                found = true;
                break;
            }
        }
    }

    // ----- Update weight in v's adjacency list -----
    auto it_v = adjList_.find(v);
    if (it_v != adjList_.end()) {
        for (auto& p : it_v->second) {
            if (p.first == u) {
                p.second = new_weight;
                found = true;
                break;
            }
        }
    }

    if (!found) {
        throw std::runtime_error("Graph::update_weight(): edge (" 
                                 + std::to_string(u) + ", " + std::to_string(v) 
                                 + ") does not exist.");
    }
}

/**
 * @brief Check whether an undirected edge exists between u and v.
 *
 * @param u  One endpoint
 * @param v  The other endpoint
 * @return true if (u, v) exists; false otherwise.
 */
bool Graph::edge_exists(int u, int v) const {
    auto it_u = adjList_.find(u);
    if (it_u == adjList_.end()) {
        return false;
    }
    // Scan u’s neighbors for v
    for (const auto& p : it_u->second) {
        if (p.first == v) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Print the entire adjacency list representation to stdout.
 *
 * Format:
 *   u: (v1, w1) (v2, w2) ...
 * for each node u that has an adjacency entry.
 */
void Graph::print_graph() const {
    for (const auto& entry : adjList_) {
        int u = entry.first;
        const auto& neighbors = entry.second;
        std::cout << u << ":";
        for (const auto& p : neighbors) {
            int v = p.first;
            int w = p.second;
            std::cout << " (" << v << ", " << w << ")";
        }
        std::cout << std::endl;
    }
}

/**
 * @brief Retrieve the adjacency list of node u.
 *
 * Returns a const reference to a vector of (neighbor, weight) pairs.
 * If u has no entry in the adjacency map, returns a reference to a static empty vector.
 */
const std::vector<std::pair<int,int>>& Graph::get_neighbors(int u) const {
    auto it = adjList_.find(u);
    if (it != adjList_.end()) {
        return it->second;
    }
    return EMPTY_NEIGHBORS_;
}