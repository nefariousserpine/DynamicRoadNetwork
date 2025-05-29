// test_dijkstra.cpp
// ============================
// Unit tests for the DynamicDijkstra class.
// Builds a small sample graph, runs Dijkstra, and then performs dynamic updates.
// ============================
#include "Graph.hpp"
#include "DynamicDijkstra.hpp"
#include <iostream>
#include <vector>
#include <limits>
#include <cassert>

/**
 * @brief Helper to compare two integer vectors for equality.
 */
static bool vec_equal(const std::vector<int>& a, const std::vector<int>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

int main() {
    // ---------- Construct a sample undirected graph ----------
    //
    //   (1)
    //   / \
    //  1   4
    // /     \
    //(2)---(3)
    //    2
    //
    // Edges:
    //   1–2 (weight 1)
    //   2–3 (weight 2)
    //   1–3 (weight 4)

    Graph g;
    g.add_edge(1, 2, 1);
    g.add_edge(2, 3, 2);
    g.add_edge(1, 3, 4);

    // ---------- Test 1: Basic Dijkstra from source 1 ----------
    DynamicDijkstra dd(g);
    dd.compute(1);

    // Distances should be:
    //   dist(1) = 0
    //   dist(2) = 1
    //   dist(3) = min(1 + 2, 4) = 3
    assert(dd.get_distance(1) == 0);
    assert(dd.get_distance(2) == 1);
    assert(dd.get_distance(3) == 3);

    // Shortest path from 1 to 3 should be [1, 2, 3]
    {
        std::vector<int> path = dd.get_shortest_path(3);
        std::vector<int> expected = {1, 2, 3};
        assert(vec_equal(path, expected));
    }

    // ---------- Test 2: Decrease weight of edge (1,3) to 2 ----------
    // Now direct edge 1–3 has weight 2, so shortest path from 1 to 3 should be [1,3].
    dd.update_edge(1, 3, 2);

    // New distances:
    //   dist(3) = 2 (via direct edge)
    assert(dd.get_distance(3) == 2);

    // Path from 1 to 3 should be [1,3]
    {
        std::vector<int> path = dd.get_shortest_path(3);
        std::vector<int> expected = {1, 3};
        assert(vec_equal(path, expected));
    }

    // ---------- Test 3: Increase weight of edge (1,2) to 5 ----------
    // After this change, edge (1,2) no longer part of shortest path tree.
    // We expect the shortest path from 1 to 2 to be [1, 3, 2] of cost 2 + 2 = 4.
    dd.update_edge(1, 2, 5);

    assert(dd.get_distance(2) == 4);
    {
        std::vector<int> path = dd.get_shortest_path(2);
        std::vector<int> expected = {1, 3, 2};
        assert(vec_equal(path, expected));
    }

    // ---------- Test 4: Query unreachable node (e.g., node 4) ----------
    // Node 4 does not exist in the graph; distance should be INT_MAX.
    int INF = std::numeric_limits<int>::max();
    assert(dd.get_distance(4) == INF);
    {
        std::vector<int> path = dd.get_shortest_path(4);
        assert(path.empty());
    }

    std::cout << "All DynamicDijkstra tests passed!" << std::endl;
    return 0;
}
