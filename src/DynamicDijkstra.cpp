#include "DynamicDijkstra.hpp"
#include <queue>
#include <limits>
#include <stdexcept>
#include <algorithm>

DynamicDijkstra::DynamicDijkstra(Graph& graph)
    : graph_(graph), source_(-1), dirty_(false) {}

void DynamicDijkstra::compute(int source) {
    source_ = source;
    dist_.clear();
    parent_.clear();
    dirty_ = false;

    using Pair = std::pair<int,int>;
    auto cmp = [](const Pair& a, const Pair& b) { return a.first > b.first; };
    std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pq(cmp);

    dist_[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist_[u]) continue;
        for (auto& [v, w] : graph_.get_neighbors(u)) {
            int nd = d + w;
            auto it = dist_.find(v);
            if (it == dist_.end() || nd < it->second) {
                dist_[v] = nd;
                parent_[v] = u;
                pq.push({nd, v});
            }
        }
    }
}

int DynamicDijkstra::get_distance(int target) {
    if (dirty_) {
        compute(source_);
    }
    auto it = dist_.find(target);
    return (it == dist_.end()) ? std::numeric_limits<int>::max() : it->second;
}

std::vector<int> DynamicDijkstra::get_shortest_path(int target) {
    if (dirty_) {
        compute(source_);
    }
    std::vector<int> path;
    if (dist_.find(target) == dist_.end()) return path;
    for (int cur = target; cur != source_;) {
        path.push_back(cur);
        cur = parent_[cur];
    }
    path.push_back(source_);
    std::reverse(path.begin(), path.end());
    return path;
}

void DynamicDijkstra::update_edge(int u, int v, int new_weight) {
    graph_.update_weight(u, v, new_weight);
    if (source_ >= 0) {
        dirty_ = true;
    }
}

int DynamicDijkstra::get_edge_weight(int u, int v) const {
    for (auto& [nbr, w] : graph_.get_neighbors(u)) {
        if (nbr == v) return w;
    }
    throw std::runtime_error("Edge (" + std::to_string(u)
                             + "," + std::to_string(v)
                             + ") not found");
}
