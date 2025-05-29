#include "DynamicDijkstra.hpp"
#include <queue>
#include <limits>
#include <stdexcept>
#include <algorithm>  // for std::reverse
#include <functional> // for std::greater
#include <utility>    // for std::pair

using namespace std;

DynamicDijkstra::DynamicDijkstra(Graph& graph)
    : graph_(graph), source_(-1)
{
    // Nothing else to do here.
}

void DynamicDijkstra::compute(int source) {
    source_ = source;
    dist_.clear();
    parent_.clear();

    // Min‐heap: pairs of (distance, nodeID)
    auto cmp = [](const pair<int,int>& a, const pair<int,int>& b) {
        return a.first > b.first;
    };
    priority_queue<pair<int,int>, vector<pair<int,int>>, decltype(cmp)> pq(cmp);

    // Initialize source distance and push
    dist_[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        // If this distance is stale, skip
        if (d > dist_[u]) continue;

        // Relax all neighbors of u
        const auto& neighbors = graph_.get_neighbors(u);
        for (const auto& edge : neighbors) {
            int v = edge.first;
            int w = edge.second;
            int newDist = d + w;

            auto it = dist_.find(v);
            if (it == dist_.end() || newDist < it->second) {
                dist_[v] = newDist;
                parent_[v] = u;
                pq.push({newDist, v});
            }
        }
    }
}

int DynamicDijkstra::get_distance(int target) const {
    auto it = dist_.find(target);
    if (it == dist_.end()) {
        return numeric_limits<int>::max();
    }
    return it->second;
}

vector<int> DynamicDijkstra::get_shortest_path(int target) const {
    vector<int> path;
    // If target not in dist_, it’s unreachable
    auto it = dist_.find(target);
    if (it == dist_.end()) {
        return path;
    }
    // Backtrack from target to source_
    int cur = target;
    while (cur != source_) {
        path.push_back(cur);
        auto pit = parent_.find(cur);
        if (pit == parent_.end()) {
            // No predecessor => something’s wrong or disconnected
            path.clear();
            return path;
        }
        cur = pit->second;
    }
    path.push_back(source_);
    reverse(path.begin(), path.end());
    return path;
}

int DynamicDijkstra::get_edge_weight(int u, int v) const {
    // Scan adjacency list of u for neighbor v
    const auto& nbrs = graph_.get_neighbors(u);
    for (const auto& p : nbrs) {
        if (p.first == v) {
            return p.second;
        }
    }
    throw runtime_error("DynamicDijkstra::get_edge_weight(): edge (" +
                        to_string(u) + ", " + to_string(v) + ") does not exist.");
}

void DynamicDijkstra::update_edge(int u, int v, int new_weight) {
    // 1) Retrieve old weight (throws if edge doesn’t exist)
    int old_weight = get_edge_weight(u, v);

    // 2) Propagate change to underlying graph
    graph_.update_weight(u, v, new_weight);

    // If compute(...) has never been called, there is no SPT to maintain
    if (source_ < 0) {
        return;
    }

    // 3) If the edge weight decreased, attempt localized “decrease‐push”
    if (new_weight < old_weight) {
        // Check if going source_ → ... → u → v yields a shorter distance to v
        auto it_u = dist_.find(u);
        if (it_u != dist_.end()) {
            int dist_u = it_u->second;
            int dist_v = get_distance(v);
            if (dist_u + new_weight < dist_v) {
                // We found a strictly better path to v via u
                dist_[v] = dist_u + new_weight;
                parent_[v] = u;

                // Now “push” that improvement downstream from v
                auto cmp = [](const pair<int,int>& a, const pair<int,int>& b) {
                    return a.first > b.first;
                };
                priority_queue<pair<int,int>, vector<pair<int,int>>, decltype(cmp)> pq(cmp);
                pq.push({dist_[v], v});

                while (!pq.empty()) {
                    auto [d_x, x] = pq.top();
                    pq.pop();
                    if (d_x > dist_[x]) continue;

                    // Relax neighbors of x
                    const auto& nbrs = graph_.get_neighbors(x);
                    for (const auto& ed : nbrs) {
                        int y = ed.first;
                        int w_xy = ed.second;
                        int newDistY = d_x + w_xy;

                        auto it_y = dist_.find(y);
                        int oldDistY = (it_y == dist_.end()) ? numeric_limits<int>::max() : it_y->second;
                        if (newDistY < oldDistY) {
                            dist_[y] = newDistY;
                            parent_[y] = x;
                            pq.push({newDistY, y});
                        }
                    }
                }
            }
        }
        // Also check vice versa: maybe going source_ → ... → v → u is better for u?
        auto it_v = dist_.find(v);
        if (it_v != dist_.end()) {
            int dist_v = it_v->second;
            int dist_u2 = get_distance(u);
            if (dist_v + new_weight < dist_u2) {
                // Update distance to u via v
                dist_[u] = dist_v + new_weight;
                parent_[u] = v;

                auto cmp = [](const pair<int,int>& a, const pair<int,int>& b) {
                    return a.first > b.first;
                };
                priority_queue<pair<int,int>, vector<pair<int,int>>, decltype(cmp)> pq(cmp);
                pq.push({dist_[u], u});

                while (!pq.empty()) {
                    auto [d_x, x] = pq.top();
                    pq.pop();
                    if (d_x > dist_[x]) continue;

                    const auto& nbrs = graph_.get_neighbors(x);
                    for (const auto& ed : nbrs) {
                        int y = ed.first;
                        int w_xy = ed.second;
                        int newDistY = d_x + w_xy;

                        auto it_y = dist_.find(y);
                        int oldDistY = (it_y == dist_.end()) ? numeric_limits<int>::max() : it_y->second;
                        if (newDistY < oldDistY) {
                            dist_[y] = newDistY;
                            parent_[y] = x;
                            pq.push({newDistY, y});
                        }
                    }
                }
            }
        }
    }
    // 4) If the edge weight increased, and (u,v) was used in the current SPT,
    //    we simply recompute from scratch. This is simpler (though less efficient)
    //    than a full “decremental SPT repair” algorithm.
    else if (new_weight > old_weight) {
        // If v’s parent is u, or u’s parent is v, the SPT is invalidated
        auto pit_v = parent_.find(v);
        auto pit_u = parent_.find(u);

        bool v_child_of_u = (pit_v != parent_.end() && pit_v->second == u);
        bool u_child_of_v = (pit_u != parent_.end() && pit_u->second == v);

        if (v_child_of_u || u_child_of_v) {
            // Recompute entire SPT from source_
            compute(source_);
        }
        // Otherwise, if (u,v) wasn’t on the tree, no distances change
    }
    // If new_weight == old_weight, nothing changes in distances
}
