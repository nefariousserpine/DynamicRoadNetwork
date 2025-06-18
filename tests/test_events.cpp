// test_events.cpp
// Unit tests for EventHandler: invokes process_command on simulated inputs,
// verifies Graph state changes and console output, including edge‑case handling.

#include "Graph.hpp"
#include "DynamicDijkstra.hpp"
#include "EventHandler.hpp"
#include <iostream>
#include <sstream>
#include <functional>
#include <limits>
#include <cassert>

// Capture stdout during a callback
static std::string capture_output(std::function<void()> fn) {
    std::ostringstream oss;
    auto* old_buf = std::cout.rdbuf(oss.rdbuf());
    fn();
    std::cout.rdbuf(old_buf);
    return oss.str();
}

int main() {
    Graph g;
    DynamicDijkstra dd(g);
    EventHandler handler(g, dd);

    // Test 1: ADD valid edge
    {
        auto out = capture_output([&]() {
            handler.test_process_command("ADD 1 2 10");
        });
        assert(g.edge_exists(1, 2));
        assert(out.find("Added edge (1, 2) with weight 10") != std::string::npos);
    }

    // Test 2: ADD duplicate edge
    {
        auto out = capture_output([&]() {
            handler.test_process_command("ADD 1 2 5");
        });
        assert(out.find("ERROR: Edge (1, 2) already exists.") != std::string::npos);
    }

    // Test 3: ADD self-loop
    {
        auto out = capture_output([&]() {
            handler.test_process_command("ADD 3 3 1");
        });
        assert(out.find("ERROR: Cannot add self-loop (3, 3).") != std::string::npos);
    }

    // Test 4: ADD negative weight
    {
        auto out = capture_output([&]() {
            handler.test_process_command("ADD 4 5 -2");
        });
        assert(out.find("ERROR: Weight must be non-negative") != std::string::npos);
    }

    // Test 5: ADD weight overflow
    {
        long long big = (long long)std::numeric_limits<int>::max() + 1;
        auto cmd = std::string("ADD 6 7 ") + std::to_string(big);
        auto out = capture_output([&]() {
            handler.test_process_command(cmd);
        });
        assert(out.find("ERROR: Weight must be non-negative and within int range") != std::string::npos);
    }

    // Test 6: EXISTS on existing and non-existing
    {
        auto out1 = capture_output([&]() {
            handler.test_process_command("EXISTS 1 2");
        });
        assert(out1.find("Edge (1, 2) exists.") != std::string::npos);

        auto out2 = capture_output([&]() {
            handler.test_process_command("EXISTS 2 3");
        });
        assert(out2.find("Edge (2, 3) does not exist.") != std::string::npos);
    }

    // Test 7: REMOVE valid and invalid
    {
        auto out = capture_output([&]() {
            handler.test_process_command("REMOVE 1 2");
        });
        assert(!g.edge_exists(1, 2));
        assert(out.find("Removed edge (1, 2)") != std::string::npos);

        auto out_err = capture_output([&]() {
            handler.test_process_command("REMOVE 1 2");
        });
        assert(out_err.find("ERROR: Edge (1, 2) does not exist.") != std::string::npos);
    }

    // Test 8: UPDATE valid decrease and increase
    g.add_edge(8, 9, 5);
    g.update_weight(8, 9, 3);  // decrease
    {
        auto out = capture_output([&]() {
            handler.test_process_command("QUERY 8 9");
        });
        assert(out.find("Distance from 8 to 9 = 3") != std::string::npos);
    }

    // Test 9: UPDATE self-loop
    {
        auto out = capture_output([&]() {
            handler.test_process_command("UPDATE 9 9 4");
        });
        assert(out.find("ERROR: Cannot update self-loop (9, 9).") != std::string::npos);
    }

    // Test 10: UPDATE negative and overflow
    {
        auto out_neg = capture_output([&]() {
            handler.test_process_command("UPDATE 8 9 -1");
        });
        assert(out_neg.find("ERROR: Weight must be non-negative") != std::string::npos);

        long long big = (long long)std::numeric_limits<int>::max() + 100;
        auto cmd = std::string("UPDATE 8 9 ") + std::to_string(big);
        auto out_of = capture_output([&]() {
            handler.test_process_command(cmd);
        });
        assert(out_of.find("ERROR: Weight must be non-negative and within int range") != std::string::npos);
    }

    // Test 11: QUERY unreachable and before any SPT
    {
        auto out = capture_output([&]() {
            handler.test_process_command("QUERY 1 3");
        });
        assert(out.find("NO PATH from 1 to 3") != std::string::npos);
    }

    // Test 12: HELP command lists all commands
    {
        auto out = capture_output([&]() {
            handler.test_process_command("HELP");
        });
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
