#include "EventHandler.hpp"
#include <sstream>
#include <iostream>
#include <vector>
#include <limits>

EventHandler::EventHandler(Graph& g, DynamicDijkstra& d)
    : graph_(g), dijkstra_(d), last_source_(-1), spt_valid_(false)
{
}

void EventHandler::run_event_loop() {
    std::string line;
    std::cout << "Enter commands (HELP for list). Type EXIT or <EOF> to quit." << std::endl;
    while (true) {
        if (!std::getline(std::cin, line)) {
            // EOF reached
            break;
        }
        if (line.empty()) {
            continue;
        }
        if (line == "EXIT") {
            break;
        }
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
    std::string tok;
    while (iss >> tok) {
        tokens.push_back(tok);
    }
    if (tokens.empty()) {
        return;
    }

    const std::string& cmd = tokens[0];

    try {
        if (cmd == "ADD") {
            if (tokens.size() != 4) {
                std::cout << "ERROR: ADD requires 3 arguments: u v w" << std::endl;
                return;
            }
            int u = std::stoi(tokens[1]);
            int v = std::stoi(tokens[2]);
            int w = std::stoi(tokens[3]);
            graph_.add_edge(u, v, w);
            spt_valid_ = false;
            std::cout << "Added edge (" << u << ", " << v << ") with weight " << w << std::endl;
        }
        else if (cmd == "REMOVE") {
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
            if (tokens.size() != 4) {
                std::cout << "ERROR: UPDATE requires 3 arguments: u v w" << std::endl;
                return;
            }
            int u = std::stoi(tokens[1]);
            int v = std::stoi(tokens[2]);
            int new_w = std::stoi(tokens[3]);
            if (!graph_.edge_exists(u, v)) {
                std::cout << "ERROR: Cannot update; edge (" << u << ", " << v << ") does not exist." << std::endl;
                return;
            }
            int old_w = 0;
            {
                const auto& nbrs = graph_.get_neighbors(u);
                for (auto& p : nbrs) {
                    if (p.first == v) {
                        old_w = p.second;
                        break;
                    }
                }
            }
            graph_.update_weight(u, v, new_w);

            if (spt_valid_) {
                try {
                    dijkstra_.update_edge(u, v, new_w);
                    std::cout << "Updated weight of edge (" << u << ", " << v << ") from "
                              << old_w << " to " << new_w << " (SPT repaired)." << std::endl;
                }
                catch (const std::exception& e) {
                    spt_valid_ = false;
                    std::cout << "WARNING: Dynamic update failed; will recompute on next QUERY. ("
                              << e.what() << ")" << std::endl;
                }
            }
            else {
                std::cout << "Updated weight of edge (" << u << ", " << v << ") from "
                          << old_w << " to " << new_w << std::endl;
            }
        }
        else if (cmd == "QUERY") {
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
                return;
            }
            std::vector<int> path = dijkstra_.get_shortest_path(v);
            std::cout << "Distance from " << u << " to " << v << " = " << dist << std::endl;
            std::cout << "Path: ";
            for (size_t i = 0; i < path.size(); ++i) {
                std::cout << path[i];
                if (i + 1 < path.size()) {
                    std::cout << " -> ";
                }
            }
            std::cout << std::endl;
        }
        else if (cmd == "EXISTS") {
            if (tokens.size() != 3) {
                std::cout << "ERROR: EXISTS requires 2 arguments: u v" << std::endl;
                return;
            }
            int u = std::stoi(tokens[1]);
            int v = std::stoi(tokens[2]);
            if (graph_.edge_exists(u, v)) {
                std::cout << "Edge (" << u << ", " << v << ") exists." << std::endl;
            }
            else {
                std::cout << "Edge (" << u << ", " << v << ") does not exist." << std::endl;
            }
        }
        else if (cmd == "PRINT") {
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
        std::cout << "ERROR: Integer argument out of range." << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "ERROR: Exception caught: " << e.what() << std::endl;
    }
}

void EventHandler::print_help() const {
    std::cout << "Supported commands:" << std::endl;
    std::cout << "  ADD u v w       - Add undirected edge (u,v) with weight w." << std::endl;
    std::cout << "  REMOVE u v      - Remove undirected edge (u,v)." << std::endl;
    std::cout << "  UPDATE u v w    - Update edge (u,v) weight to w." << std::endl;
    std::cout << "  QUERY u v       - Print shortest-path distance and nodes from u to v." << std::endl;
    std::cout << "  EXISTS u v      - Check if undirected edge (u,v) exists." << std::endl;
    std::cout << "  PRINT           - Print current graph adjacency list." << std::endl;
    std::cout << "  HELP            - Show this help message." << std::endl;
    std::cout << "  EXIT            - Exit event loop." << std::endl;
}

void EventHandler::ensure_spt(int src) {
    if (!spt_valid_ || last_source_ != src) {
        dijkstra_.compute(src);
        last_source_ = src;
        spt_valid_ = true;
    }
}
