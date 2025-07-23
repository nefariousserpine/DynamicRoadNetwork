#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <unordered_map>
#include <vector>
#include <utility>


class Graph {
public:
    Graph() = default;
    ~Graph() = default;
    Graph(const Graph&) = default;
    Graph& operator=(const Graph&) = default;

    void add_edge(int u, int v, int weight);

    void remove_edge(int u, int v);

    void update_weight(int u, int v, int new_weight);

    bool edge_exists(int u, int v) const;

    void print_graph() const;

    const std::vector<std::pair<int,int>>& get_neighbors(int u) const;

private:
    std::unordered_map<int, std::vector<std::pair<int,int>>> adjList_;

    static const std::vector<std::pair<int,int>> EMPTY_NEIGHBORS_;
};

#endif 
