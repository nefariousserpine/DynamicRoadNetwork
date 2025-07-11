// EventHandler.cpp

#include "EventHandler.hpp"
#include <sstream>
#include <iostream>
#include <vector>
#include <limits>
#include <stdexcept>

EventHandler::EventHandler(Graph& g, DynamicDijkstra& d)
    : graph_(g), dijkstra_(d), last_source_(-1), spt_valid_(false)
{ }

void EventHandler::run_event_loop() {
    std::string line;
    std::cout << "Enter commands (HELP for list). Type EXIT or <EOF> to quit." << std::endl;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        if (line == "EXIT") break;
        process_command(line);
    }
    std::cout << "Exiting event loop." << std::endl;
}

void EventHandler::test_process_command(const std::string& cmd) {
    process_command(cmd);
}

void EventHandler::process_command(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    for (std::string t; iss >> t; )
        tokens.push_back(t);
    if (tokens.empty()) return;

    const auto& cmd = tokens[0];
    try {
        if (cmd == "ADD") {
            // Usage: ADD u v w
            if (tokens.size() != 4) {
                std::cout << "ERROR: ADD requires 3 arguments: u v w" << std::endl;
                return;
            }
            int u = std::stoi(tokens[1]);
            int v = std::stoi(tokens[2]);
            long long w_ll = std::stoll(tokens[3]);
            if (u == v) {
                std::cout << "ERROR: Cannot add self-loop (" << u << ", " << v << ")." << std::endl;
                return;
            }
            if (w_ll < 0 || w_ll > std::numeric_limits<int>::max()) {
                std::cout << "ERROR: Weight must be non-negative and within int range." << std::endl;
                return;
            }
            int w = static_cast<int>(w_ll);

            // Prevent duplicate edges
            if (graph_.edge_exists(u, v)) {
                std::cout << "ERROR: Edge (" << u << ", " << v << ") already exists." << std::endl;
                return;
            }

            graph_.add_edge(u, v, w);
            spt_valid_ = false;
            std::cout << "Added edge (" << u << ", " << v << ") with weight " << w << std::endl;
        }
        else if (cmd == "REMOVE") {
            // Usage: REMOVE u v
            if (tokens.size() != 3) {
                std::cout << "ERROR: REMOVE requires 2 arguments: u v" << std::endl;
                return;
            }
            int u = std::stoi(tokens[1]);
            int v = std::stoi(tokens[2]);

            if (!graph_.edge_exists(u, v)) {
                std::cout << "ERROR: Edge (" << u << ", " << v << ") does not exist." << std::endl;
                return;
            }

            graph_.remove_edge(u, v);
            spt_valid_ = false;
            std::cout << "Removed edge (" << u << ", " << v << ")" << std::endl;
        }
        else if (cmd == "UPDATE") {
            // Usage: UPDATE u v w
            if (tokens.size() != 4) {
                std::cout << "ERROR: UPDATE requires 3 arguments: u v w" << std::endl;
                return;
            }
            int u = std::stoi(tokens[1]);
            int v = std::stoi(tokens[2]);
            long long w_ll = std::stoll(tokens[3]);
            if (u == v) {
                std::cout << "ERROR: Cannot update self-loop (" << u << ", " << v << ")." << std::endl;
                return;
            }
            if (w_ll < 0 || w_ll > std::numeric_limits<int>::max()) {
                std::cout << "ERROR: Weight must be non-negative and within int range." << std::endl;
                return;
            }
            int newW = static_cast<int>(w_ll);

            // Check existence and fetch old weight
            if (!graph_.edge_exists(u, v)) {
                std::cout << "ERROR: Cannot update; edge (" << u << ", " << v << ") does not exist." << std::endl;
                return;
            }
            int oldW = 0;
            for (const auto& pr : graph_.get_neighbors(u)) {
                if (pr.first == v) { oldW = pr.second; break; }
            }

            // Base print message
            std::cout << "Updated weight of edge (" << u << ", " << v << ") from " << oldW << " to " << newW;

            dijkstra_.update_edge(u, v, newW);
            spt_valid_ = false;
            std::cout << std::endl;
        }
        else if (cmd == "QUERY") {
            // Usage: QUERY u v
            if (tokens.size() != 3) {
                std::cout << "ERROR: QUERY requires 2 arguments: u v" << std::endl;
                return;
            }
            int u = std::stoi(tokens[1]);
            int v = std::stoi(tokens[2]);

            ensure_spt(u);

            int dist = dijkstra_.get_distance(v);
            if (dist == std::numeric_limits<int>::max()) {
                std::cout << "NO PATH from " << u << " to " << v << std::endl;
            } else {
                std::vector<int> path = dijkstra_.get_shortest_path(v);
                std::cout << "Distance from " << u << " to " << v << " = " << dist << std::endl;
                std::cout << "Path: ";
                for (size_t i = 0; i < path.size(); ++i) {
                    std::cout << path[i];
                    if (i + 1 < path.size()) std::cout << " -> ";
                }
                std::cout << std::endl;
            }
        }
        else if (cmd == "EXISTS") {
            // Usage: EXISTS u v
            if (tokens.size() != 3) {
                std::cout << "ERROR: EXISTS requires 2 arguments: u v" << std::endl;
                return;
            }
            int u = std::stoi(tokens[1]);
            int v = std::stoi(tokens[2]);
            std::cout << (graph_.edge_exists(u, v)
                          ? "Edge (" + std::to_string(u) + ", " + std::to_string(v) + ") exists."
                          : "Edge (" + std::to_string(u) + ", " + std::to_string(v) + ") does not exist.")
                      << std::endl;
        }
        else if (cmd == "PRINT") {
            // Usage: PRINT
            graph_.print_graph();
        }
        else if (cmd == "HELP") {
            print_help();
        }
        else {
            std::cout << "ERROR: Unknown command \"" << cmd << "\". Type HELP for list." << std::endl;
        }
    }
    catch (const std::invalid_argument&) {
        std::cout << "ERROR: Invalid integer argument in command." << std::endl;
    }
    catch (const std::out_of_range&) {
        std::cout << "ERROR: Argument out of range." << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
    }
}

void EventHandler::print_help() const {
    std::cout << "Supported commands:\n"
              << "  ADD u v w       - Add undirected edge (u,v) with non-negative weight w.\n"
              << "  REMOVE u v      - Remove undirected edge (u,v).\n"
              << "  UPDATE u v w    - Update edge (u,v) weight to non-negative w.\n"
              << "  QUERY u v       - Print shortest-path distance and nodes from u to v.\n"
              << "  EXISTS u v      - Check if undirected edge (u,v) exists.\n"
              << "  PRINT           - Print current graph adjacency list.\n"
              << "  HELP            - Show this help message.\n"
              << "  EXIT            - Exit event loop.\n";
}

void EventHandler::ensure_spt(int src) {
    if (!spt_valid_ || last_source_ != src) {
        dijkstra_.compute(src);
        last_source_ = src;
        spt_valid_ = true;
    }
}
