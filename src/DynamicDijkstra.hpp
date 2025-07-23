#ifndef DYNAMICDIJKSTRA_HPP
#define DYNAMICDIJKSTRA_HPP

#include "Graph.hpp"
#include <unordered_map>
#include <vector>


class DynamicDijkstra {
public:
    explicit DynamicDijkstra(Graph& graph);

    void compute(int source);

    int get_distance(int target);

    std::vector<int> get_shortest_path(int target);

    void update_edge(int u, int v, int new_weight);

private:
    Graph& graph_;
    int source_;
    bool dirty_;    
    std::unordered_map<int,int> dist_;
    std::unordered_map<int,int> parent_;

    int get_edge_weight(int u, int v) const;
};

#endif 
