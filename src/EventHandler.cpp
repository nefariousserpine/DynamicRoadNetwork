#include "EventHandler.hpp"
#include <sstream>
#include <iostream>
#include <vector>
#include <limits>

using namespace std;

EventHandler::EventHandler(Graph& g, DynamicDijkstra& d)
    : graph_(g), dijkstra_(d), last_source_(-1), spt_valid_(false)
{
}

void EventHandler::run_event_loop() {
    string line;
    cout << "Enter commands (HELP for list). Type EXIT or <EOF> to quit." << endl;
    while (true) {
        if (!std::getline(cin, line)) {
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
    cout << "Exiting event loop." << endl;
}

void EventHandler::test_process_command(const string& cmd) {
    process_command(cmd);
}

void EventHandler::process_command(const string& line) {
    // Tokenize by whitespace
    istringstream iss(line);
    vector<string> tokens;
    string tok;
    while (iss >> tok) {
        tokens.push_back(tok);
    }
    if (tokens.empty()) {
        return;
    }

    const string& cmd = tokens[0];

    try {
        if (cmd == "ADD") {
            // Usage: ADD u v w
            if (tokens.size() != 4) {
                cout << "ERROR: ADD requires 3 arguments: u v w" << endl;
                return;
            }
            int u = stoi(tokens[1]);
            int v = stoi(tokens[2]);
            int w = stoi(tokens[3]);
            graph_.add_edge(u, v, w);
            // After insertion, any existing SPT is invalid
            spt_valid_ = false;
            cout << "Added edge (" << u << ", " << v << ") with weight " << w << endl;
        }
        else if (cmd == "REMOVE") {
            // Usage: REMOVE u v
            if (tokens.size() != 3) {
                cout << "ERROR: REMOVE requires 2 arguments: u v" << endl;
                return;
            }
            int u = stoi(tokens[1]);
            int v = stoi(tokens[2]);
            if (!graph_.edge_exists(u, v)) {
                cout << "ERROR: Edge (" << u << ", " << v << ") does not exist." << endl;
                return;
            }
            graph_.remove_edge(u, v);
            // After deletion, any existing SPT is invalid
            spt_valid_ = false;
            cout << "Removed edge (" << u << ", " << v << ")" << endl;
        }
        else if (cmd == "UPDATE") {
            // Usage: UPDATE u v w
            if (tokens.size() != 4) {
                cout << "ERROR: UPDATE requires 3 arguments: u v w" << endl;
                return;
            }
            int u = stoi(tokens[1]);
            int v = stoi(tokens[2]);
            int new_w = stoi(tokens[3]);
            if (!graph_.edge_exists(u, v)) {
                cout << "ERROR: Cannot update; edge (" << u << ", " << v << ") does not exist." << endl;
                return;
            }
            // Retrieve old weight for message (optional)
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
            // Update weight in the Graph
            graph_.update_weight(u, v, new_w);

            // If we have a valid SPT, attempt localized update:
            if (spt_valid_) {
                try {
                    dijkstra_.update_edge(u, v, new_w);
                    cout << "Updated weight of edge (" << u << ", " << v << ") from "
                         << old_w << " to " << new_w << " (SPT repaired)." << endl;
                }
                catch (const exception& e) {
                    // If dynamic update failed, invalidate SPT for next query
                    spt_valid_ = false;
                    cout << "WARNING: Dynamic update failed; will recompute on next QUERY. ("
                         << e.what() << ")" << endl;
                }
            }
            else {
                cout << "Updated weight of edge (" << u << ", " << v << ") from "
                     << old_w << " to " << new_w << endl;
            }
        }
        else if (cmd == "QUERY") {
            // Usage: QUERY u v
            if (tokens.size() != 3) {
                cout << "ERROR: QUERY requires 2 arguments: u v" << endl;
                return;
            }
            int u = stoi(tokens[1]);
            int v = stoi(tokens[2]);
            // Ensure SPT rooted at u is valid
            ensure_spt(u);

            int dist = dijkstra_.get_distance(v);
            if (dist == numeric_limits<int>::max()) {
                cout << "NO PATH from " << u << " to " << v << endl;
                return;
            }
            vector<int> path = dijkstra_.get_shortest_path(v);
            // Print distance and path
            cout << "Distance from " << u << " to " << v << " = " << dist << endl;
            cout << "Path: ";
            for (size_t i = 0; i < path.size(); ++i) {
                cout << path[i];
                if (i + 1 < path.size()) {
                    cout << " -> ";
                }
            }
            cout << endl;
        }
        else if (cmd == "EXISTS") {
            // Usage: EXISTS u v
            if (tokens.size() != 3) {
                cout << "ERROR: EXISTS requires 2 arguments: u v" << endl;
                return;
            }
            int u = stoi(tokens[1]);
            int v = stoi(tokens[2]);
            if (graph_.edge_exists(u, v)) {
                cout << "Edge (" << u << ", " << v << ") exists." << endl;
            }
            else {
                cout << "Edge (" << u << ", " << v << ") does not exist." << endl;
            }
        }
        else if (cmd == "PRINT") {
            // Usage: PRINT
            graph_.print_graph();
        }
        else if (cmd == "HELP") {
            print_help();
        }
        else {
            cout << "ERROR: Unknown command \"" << cmd << "\". Type HELP for list." << endl;
        }
    }
    catch (const invalid_argument&) {
        cout << "ERROR: Invalid integer argument in command." << endl;
    }
    catch (const out_of_range&) {
        cout << "ERROR: Integer argument out of range." << endl;
    }
    catch (const exception& e) {
        cout << "ERROR: Exception caught: " << e.what() << endl;
    }
}

void EventHandler::print_help() const {
    cout << "Supported commands:" << endl;
    cout << "  ADD u v w       - Add undirected edge (u,v) with weight w." << endl;
    cout << "  REMOVE u v      - Remove undirected edge (u,v)." << endl;
    cout << "  UPDATE u v w    - Update edge (u,v) weight to w." << endl;
    cout << "  QUERY u v       - Print shortest-path distance and nodes from u to v." << endl;
    cout << "  EXISTS u v      - Check if undirected edge (u,v) exists." << endl;
    cout << "  PRINT           - Print current graph adjacency list." << endl;
    cout << "  HELP            - Show this help message." << endl;
    cout << "  EXIT            - Exit event loop." << endl;
}

void EventHandler::ensure_spt(int src) {
    // If last computed SPT source differs or invalid, recompute SPT from scratch
    if (!spt_valid_ || last_source_ != src) {
        dijkstra_.compute(src);
        last_source_ = src;
        spt_valid_ = true;
    }
}
