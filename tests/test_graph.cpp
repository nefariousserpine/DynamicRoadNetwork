// test_graph.cpp
// ============================
// Unit tests for the Graph class.
// Verifies add_edge, remove_edge, update_weight, edge_exists, and get_neighbors.
// ============================
#include "Graph.hpp"
#include <iostream>
#include <cassert>

int main() {
    // ---------- Test: Initial state ----------
    // A newly constructed Graph should have no edges.
    Graph g;
    assert(!g.edge_exists(1, 2));
    assert(g.get_neighbors(1).empty());
    assert(g.get_neighbors(2).empty());

    // ---------- Test: add_edge and edge_exists ----------
    // Add edge (1,2) with weight 10, then verify existence on both endpoints.
    g.add_edge(1, 2, 10);
    assert(g.edge_exists(1, 2));
    assert(g.edge_exists(2, 1));

    // Verify adjacency lists for both nodes
    {
        const auto& nbrs1 = g.get_neighbors(1);
        assert(nbrs1.size() == 1);
        assert(nbrs1[0].first == 2 && nbrs1[0].second == 10);

        const auto& nbrs2 = g.get_neighbors(2);
        assert(nbrs2.size() == 1);
        assert(nbrs2[0].first == 1 && nbrs2[0].second == 10);
    }

    // ---------- Test: update_weight ----------
    // Change weight of (1,2) from 10 to 20, then verify both sides updated.
    g.update_weight(1, 2, 20);
    {
        const auto& nbrs1 = g.get_neighbors(1);
        assert(nbrs1.size() == 1);
        assert(nbrs1[0].first == 2 && nbrs1[0].second == 20);

        const auto& nbrs2 = g.get_neighbors(2);
        assert(nbrs2.size() == 1);
        assert(nbrs2[0].first == 1 && nbrs2[0].second == 20);
    }

    // ---------- Test: add_edge existing edge updates, not duplicates ----------
    // Adding (1,2) again with weight 30 should update, not append a duplicate.
    g.add_edge(1, 2, 30);
    {
        const auto& nbrs1b = g.get_neighbors(1);
        assert(nbrs1b.size() == 1);
        assert(nbrs1b[0].first == 2 && nbrs1b[0].second == 30);

        const auto& nbrs2b = g.get_neighbors(2);
        assert(nbrs2b.size() == 1);
        assert(nbrs2b[0].first == 1 && nbrs2b[0].second == 30);
    }

    // ---------- Test: remove_edge ----------
    // Remove edge (1,2), then verify it no longer exists and adjacency lists are empty.
    g.remove_edge(1, 2);
    assert(!g.edge_exists(1, 2));
    assert(!g.edge_exists(2, 1));
    assert(g.get_neighbors(1).empty());
    assert(g.get_neighbors(2).empty());

    // ---------- Test: remove non-existent edge (should do nothing) ----------
    // Removing again should not crash or throw.
    g.remove_edge(1, 2);
    assert(!g.edge_exists(1, 2));

    // ---------- Test: update_weight on non-existent edge ----------
    // Attempting to update should throw std::runtime_error.
    try {
        g.update_weight(1, 2, 50);
        // If we reach here, test failed.
        assert(false && "Expected runtime_error for updating non-existent edge");
    }
    catch (const std::runtime_error&) {
        // Expected exception; test passes.
    }

    std::cout << "All Graph tests passed!" << std::endl;
    return 0;
}