// DynamicDijkstra.cpp
#include "DynamicDijkstra.hpp"
#include <queue>
#include <limits>
#include <stdexcept>
#include <algorithm>

DynamicDijkstra::DynamicDijkstra(Graph& graph)
    : graph_(graph), source_(-1) {}

void DynamicDijkstra::compute(int source) {
    source_ = source;
    dist_.clear();
    parent_.clear();

    using Pair = std::pair<int,int>;
    auto cmp = [](const Pair& a, const Pair& b) { return a.first > b.first; };
    std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pq(cmp);

    dist_[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        auto [d,u] = pq.top(); pq.pop();
        if (d > dist_[u]) continue;
        for (auto& [v,w] : graph_.get_neighbors(u)) {
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

int DynamicDijkstra::get_distance(int target) const {
    auto it = dist_.find(target);
    return (it == dist_.end()) ? std::numeric_limits<int>::max() : it->second;
}

std::vector<int> DynamicDijkstra::get_shortest_path(int target) const {
    std::vector<int> path;
    if (dist_.find(target) == dist_.end()) return path;
    for (int cur = target; cur != source_;) {
        path.push_back(cur);
        auto pit = parent_.find(cur);
        if (pit == parent_.end()) return {};
        cur = pit->second;
    }
    path.push_back(source_);
    std::reverse(path.begin(), path.end());
    return path;
}

int DynamicDijkstra::get_edge_weight(int u, int v) const {
    for (auto& [nbr,w] : graph_.get_neighbors(u)) {
        if (nbr == v) return w;
    }
    throw std::runtime_error("Edge (" + std::to_string(u) + "," + std::to_string(v) + ") not found");
}

void DynamicDijkstra::update_edge(int u, int v, int new_w) {
    int old_w = get_edge_weight(u,v);
    graph_.update_weight(u,v,new_w);
    if (source_ < 0) return;

    // Decrease: localized repair
    if (new_w < old_w) {
        auto relax = [&](int src,int dst) {
            auto it = dist_.find(src);
            if (it==dist_.end()) return;
            int cand = it->second + new_w;
            if (cand < get_distance(dst)) {
                dist_[dst]=cand;
                parent_[dst]=src;
                using Pair=std::pair<int,int>;
                auto cmp=[](auto&a,auto&b){return a.first>b.first;};
                std::priority_queue<Pair,std::vector<Pair>,decltype(cmp)> pq(cmp);
                pq.push({cand,dst});
                while(!pq.empty()){
                    auto [d,x]=pq.top(); pq.pop();
                    if(d>dist_[x]) continue;
                    for(auto& [y,wxy]:graph_.get_neighbors(x)){
                        int nd=d+wxy;
                        if(nd<get_distance(y)){
                            dist_[y]=nd;
                            parent_[y]=x;
                            pq.push({nd,y});
                        }
                    }
                }
            }
        };
        relax(u,v);
        relax(v,u);
        return;
    }

    // Increase: if on SPT, signal failure
    bool v_child = parent_.count(v)&& parent_.at(v)==u;
    bool u_child = parent_.count(u)&& parent_.at(u)==v;
    if (new_w>old_w && (v_child||u_child)) {
        throw std::runtime_error("SPT repair failed: weight increase on tree edge");
    }
}