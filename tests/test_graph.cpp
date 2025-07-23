#include "Graph.hpp"
#include <iostream>
#include <cassert>

int main() {
    Graph g;
    assert(!g.edge_exists(1, 2));
    assert(g.get_neighbors(1).empty());
    assert(g.get_neighbors(2).empty());

    g.add_edge(1, 2, 10);
    assert(g.edge_exists(1, 2));
    assert(g.edge_exists(2, 1));

    {
        const auto& nbrs1 = g.get_neighbors(1);
        assert(nbrs1.size() == 1);
        assert(nbrs1[0].first == 2 && nbrs1[0].second == 10);

        const auto& nbrs2 = g.get_neighbors(2);
        assert(nbrs2.size() == 1);
        assert(nbrs2[0].first == 1 && nbrs2[0].second == 10);
    }

    g.update_weight(1, 2, 20);
    {
        const auto& nbrs1 = g.get_neighbors(1);
        assert(nbrs1.size() == 1);
        assert(nbrs1[0].first == 2 && nbrs1[0].second == 20);

        const auto& nbrs2 = g.get_neighbors(2);
        assert(nbrs2.size() == 1);
        assert(nbrs2[0].first == 1 && nbrs2[0].second == 20);
    }

    g.add_edge(1, 2, 30);
    {
        const auto& nbrs1b = g.get_neighbors(1);
        assert(nbrs1b.size() == 1);
        assert(nbrs1b[0].first == 2 && nbrs1b[0].second == 30);

        const auto& nbrs2b = g.get_neighbors(2);
        assert(nbrs2b.size() == 1);
        assert(nbrs2b[0].first == 1 && nbrs2b[0].second == 30);
    }

    g.remove_edge(1, 2);
    assert(!g.edge_exists(1, 2));
    assert(!g.edge_exists(2, 1));
    assert(g.get_neighbors(1).empty());
    assert(g.get_neighbors(2).empty());

    g.remove_edge(1, 2);
    assert(!g.edge_exists(1, 2));

    try {
        g.update_weight(1, 2, 50);
        assert(false && "Expected runtime_error for updating non-existent edge");
    }
    catch (const std::runtime_error&) {
        
    }

    std::cout << "All Graph tests passed!" << std::endl;
    return 0;
}
