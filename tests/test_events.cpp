// test_events.cpp
// ============================
// Unit tests for the EventHandler class.
// Directly invokes process_command(...) on simulated inputs,
// and verifies Graph state changes and expected console output.
// ============================
#include "Graph.hpp"
#include "DynamicDijkstra.hpp"
#include "EventHandler.hpp"
#include <iostream>
#include <sstream>
#include <cassert>

/**
 * @brief Captures output from std::cout during a callback.
 */
static std::string capture_output(std::function<void()> fn) {
    std::ostringstream oss;
    std::streambuf* old_buf = std::cout.rdbuf(oss.rdbuf());
    fn();
    std::cout.rdbuf(old_buf);
    return oss.str();
}

int main() {
    // ---------- Setup: Graph, Dijkstra, and EventHandler ----------
    Graph g;
    DynamicDijkstra dd(g);
    EventHandler handler(g, dd);

    // ---------- Test 1: ADD command ----------
    // After "ADD 1 2 10", edge (1,2) must exist, and console output must match.
    {
        std::string out = capture_output([&]() {
            handler.process_command("ADD 1 2 10");
        });
        assert(g.edge_exists(1, 2));
        assert(out.find("Added edge (1, 2) with weight 10") != std::string::npos);
    }

    // ---------- Test 2: EXISTS command (existing edge) ----------
    {
        std::string out = capture_output([&]() {
            handler.process_command("EXISTS 1 2");
        });
        assert(out.find("Edge (1, 2) exists.") != std::string::npos);
    }

    // ---------- Test 3: EXISTS command (non-existent edge) ----------
    {
        std::string out = capture_output([&]() {
            handler.process_command("EXISTS 2 3");
        });
        assert(out.find("Edge (2, 3) does not exist.") != std::string::npos);
    }

    // ---------- Test 4: REMOVE command ----------
    // Remove the previously added edge and ensure graph no longer has it.
    {
        std::string out = capture_output([&]() {
            handler.process_command("REMOVE 1 2");
        });
        assert(!g.edge_exists(1, 2));
        assert(out.find("Removed edge (1, 2)") != std::string::npos);
    }

    // ---------- Test 5: REMOVE non-existent edge (error) ----------
    {
        std::string out = capture_output([&]() {
            handler.process_command("REMOVE 1 3");
        });
        assert(out.find("ERROR: Edge (1, 3) does not exist.") != std::string::npos);
    }

    // ---------- Test 6: QUERY command with no valid SPT (unreachable nodes) ----------
    // Since no edges exist, querying 1→2 should report "NO PATH".
    {
        std::string out = capture_output([&]() {
            handler.process_command("QUERY 1 2");
        });
        assert(out.find("NO PATH from 1 to 2") != std::string::npos);
    }

    // ---------- Test 7: UPDATE command on non-existent edge ----------
    {
        std::string out = capture_output([&]() {
            handler.process_command("UPDATE 1 2 5");
        });
        assert(out.find("ERROR: Cannot update; edge (1, 2) does not exist.") != std::string::npos);
    }

    // ---------- Test 8: HELP command ----------
    {
        std::string out = capture_output([&]() {
            handler.process_command("HELP");
        });
        assert(out.find("Supported commands:") != std::string::npos);
        assert(out.find("ADD u v w") != std::string::npos);
        assert(out.find("REMOVE u v") != std::string::npos);
        assert(out.find("UPDATE u v w") != std::string::npos);
        assert(out.find("QUERY u v") != std::string::npos);
        assert(out.find("EXISTS u v") != std::string::npos);
        assert(out.find("PRINT") != std::string::npos);
        assert(out.find("HELP") != std::string::npos);
        assert(out.find("EXIT") != std::string::npos);
    }

    std::cout << "All EventHandler tests passed!" << std::endl;
    return 0;
}
