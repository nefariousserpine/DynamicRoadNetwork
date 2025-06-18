#include "DynamicDijkstra.hpp"
#include <queue>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <utility>

DynamicDijkstra::DynamicDijkstra(Graph& graph)
    : graph_(graph), source_(-1) {}

void DynamicDijkstra::compute(int source) {
    source_ = source;
    dist_.clear();
    parent_.clear();

    // Min‐heap: pairs of (distance, nodeID)
    auto cmp = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        return a.first > b.first;
    };
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(cmp)> pq(cmp);

    dist_[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist_[u]) continue;

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
        return std::numeric_limits<int>::max();
    }
    return it->second;
}

std::vector<int> DynamicDijkstra::get_shortest_path(int target) const {
    std::vector<int> path;
    auto it = dist_.find(target);
    if (it == dist_.end()) {
        return path;
    }

    int cur = target;
    while (cur != source_) {
        path.push_back(cur);
        auto pit = parent_.find(cur);
        if (pit == parent_.end()) {
            path.clear();
            return path;
        }
        cur = pit->second;
    }
    path.push_back(source_);
    std::reverse(path.begin(), path.end());
    return path;
}

int DynamicDijkstra::get_edge_weight(int u, int v) const {
    const auto& nbrs = graph_.get_neighbors(u);
    for (const auto& p : nbrs) {
        if (p.first == v) {
            return p.second;
        }
    }
    throw std::runtime_error("DynamicDijkstra::get_edge_weight(): edge (" +
                             std::to_string(u) + ", " + std::to_string(v) + ") does not exist.");
}

void DynamicDijkstra::update_edge(int u, int v, int new_weight) {
    int old_weight = get_edge_weight(u, v);
    graph_.update_weight(u, v, new_weight);

    if (source_ < 0) {
        return;
    }

    if (new_weight < old_weight) {
        auto it_u = dist_.find(u);
        if (it_u != dist_.end()) {
            int dist_u = it_u->second;
            int dist_v = get_distance(v);
            if (dist_u + new_weight < dist_v) {
                dist_[v] = dist_u + new_weight;
                parent_[v] = u;

                auto cmp = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                    return a.first > b.first;
                };
                std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(cmp)> pq(cmp);
                pq.push({dist_[v], v});

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
                        int oldDistY = (it_y == dist_.end()) ? std::numeric_limits<int>::max() : it_y->second;
                        if (newDistY < oldDistY) {
                            dist_[y] = newDistY;
                            parent_[y] = x;
                            pq.push({newDistY, y});
                        }
                    }
                }
            }
        }

        auto it_v = dist_.find(v);
        if (it_v != dist_.end()) {
            int dist_v = it_v->second;
            int dist_u2 = get_distance(u);
            if (dist_v + new_weight < dist_u2) {
                dist_[u] = dist_v + new_weight;
                parent_[u] = v;

                auto cmp = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                    return a.first > b.first;
                };
                std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(cmp)> pq(cmp);
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
                        int oldDistY = (it_y == dist_.end()) ? std::numeric_limits<int>::max() : it_y->second;
                        if (newDistY < oldDistY) {
                            dist_[y] = newDistY;
                            parent_[y] = x;
                            pq.push({newDistY, y});
                        }
                    }
                }
            }
        }
    } else if (new_weight > old_weight) {
        auto pit_v = parent_.find(v);
        auto pit_u = parent_.find(u);

        bool v_child_of_u = (pit_v != parent_.end() && pit_v->second == u);
        bool u_child_of_v = (pit_u != parent_.end() && pit_u->second == v);

        if (v_child_of_u || u_child_of_v) {
            compute(source_);
        }
    }
}
