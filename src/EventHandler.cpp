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
        // If user typed "EXIT", break
        if (line == "EXIT") {
            break;
        }
        process_command(line);
    }
    cout << "Exiting event loop." << endl;
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
        cout << "ERROR: Exception: " << e.what() << endl;
    }
}

void EventHandler::print_help() const {
    cout << "\nSupported commands:\n"
         << "ADD u v w    — Add an undirected edge (u, v) with weight w.\n"
         << "REMOVE u v   — Remove the undirected edge (u, v).\n"
         << "UPDATE u v w — Update the weight of edge (u, v) to w.\n"
         << "QUERY u v    — Print shortest‐path distance & nodes from u to v.\n"
         << "PRINT        — Print the current adjacency list of the graph.\n"
         << "HELP         — Show this help message.\n"
         << "EXIT         — Exit the event loop (or use EOF).\n\n";
}

void EventHandler::ensure_spt(int src) {
    // If we haven’t computed, or the source changed, or SPT was invalidated,
    // run a fresh Dijkstra from src.
    if (!spt_valid_ || last_source_ != src) {
        dijkstra_.compute(src);
        last_source_ = src;
        spt_valid_ = true;
    }
}
