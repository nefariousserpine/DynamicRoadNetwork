#include "Graph.hpp"
#include <iostream>
#include <algorithm>    
#include <stdexcept>    

// Initialize the static empty neighbors vector
const std::vector<std::pair<int,int>> Graph::EMPTY_NEIGHBORS_ = {};

// Adds or updates an undirected edge between nodes u and v with the given weight.
// Throws runtime error, if the edge already exists
void Graph::add_edge(int u, int v, int weight) {
    auto& neighbors_u = adjList_[u];
    for (const auto& p : neighbors_u) {
        if (p.first == v) {
            throw std::runtime_error("Graph::add_edge(): edge (" 
                                     + std::to_string(u) + ", " + std::to_string(v) 
                                     + ") already exists with weight " + std::to_string(p.second));
        }
    }

    auto& neighbors_v = adjList_[v];
    // Add the undirected edge
    neighbors_u.emplace_back(v, weight);
    neighbors_v.emplace_back(u, weight);
}

// Removes the undirected edge between nodes u and v.
// If the edge does not exist, this has no effect.
void Graph::remove_edge(int u, int v) {
    auto it_u = adjList_.find(u);
    if (it_u != adjList_.end()) {
        auto& neighbors_u = it_u->second;
        neighbors_u.erase(
            std::remove_if(neighbors_u.begin(), neighbors_u.end(),
                           [v](const std::pair<int,int>& p) { return p.first == v; }),
            neighbors_u.end()
        );
        if (neighbors_u.empty()) adjList_.erase(it_u);
    }

    auto it_v = adjList_.find(v);
    if (it_v != adjList_.end()) {
        auto& neighbors_v = it_v->second;
        neighbors_v.erase(
            std::remove_if(neighbors_v.begin(), neighbors_v.end(),
                           [u](const std::pair<int,int>& p) { return p.first == u; }),
            neighbors_v.end()
        );
        if (neighbors_v.empty()) adjList_.erase(it_v);
    }
}

// Updates the weight of an existing undirected edge between u and v.
// Throws a runtime error if the edge does not exist.
void Graph::update_weight(int u, int v, int new_weight) {
    bool found = false;

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

// Returns true if an undirected edge between u and v exists; false otherwise.
bool Graph::edge_exists(int u, int v) const {
    auto it_u = adjList_.find(u);
    if (it_u == adjList_.end()) {
        return false;
    }

    for (const auto& p : it_u->second) {
        if (p.first == v) {
            return true;
        }
    }
    return false;
}

// Prints the current adjacency list of the graph.
// Each line represents a node and its connected neighbors with edge weights.
void Graph::print_graph() const {
    std::cout << "Node: (neighbour_node, edge_weight)" << std::endl;
    for (const auto& entry : adjList_) {
        int u = entry.first;
        const auto& neighbors = entry.second;
        std::cout << u << ":";
        for (const auto& p : neighbors) {
            std::cout << " (" << p.first << ", " << p.second << ")";
        }
        std::cout << std::endl;
    }
}

// Returns the list of neighbors for a given node u.
// If u has no neighbors, returns a reference to a static empty vector.
const std::vector<std::pair<int,int>>& Graph::get_neighbors(int u) const {
    auto it = adjList_.find(u);
    if (it != adjList_.end()) {
        return it->second;
    }
    return EMPTY_NEIGHBORS_;
}
