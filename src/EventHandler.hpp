#ifndef EVENTHANDLER_HPP
#define EVENTHANDLER_HPP

#include "Graph.hpp"
#include "DynamicDijkstra.hpp"
#include <string>

/**
 * Handles graph operations and shortest path queries.
 * 
 * Commands:
 *   ADD u v w       - Add undirected edge (u, v) with weight w
 *   REMOVE u v      - Remove undirected edge (u, v)
 *   UPDATE u v w    - Update weight of edge (u, v) to w
 *   QUERY u v       - Print shortest-path distance and path from u to v
 *   EXISTS u v      - Check if edge (u, v) exists
 *   PRINT           - Print the current adjacency list
 *   HELP            - Show usage help message
 *   EXIT            - Exit the event loop (also ends on EOF)
 */
class EventHandler {
public:
    // Constructs the event handler with references to the graph and the dynamic Dijkstra module.
    EventHandler(Graph& g, DynamicDijkstra& d);

    // Runs an interactive loop to read commands from standard input and execute them.
    void run_event_loop();

    // For unit testing: runs a single command (prints output to stdout).
    void test_process_command(const std::string& cmd);

private:
    Graph& graph_;                // Reference to the graph.
    DynamicDijkstra& dijkstra_;  // Reference to the dynamic Dijkstra module.

    int last_source_;            // Last source node used for SPT computation (-1 if none).
    bool spt_valid_;             // Whether the current shortest path tree is valid.

    // parse and execute a single command line.
    void process_command(const std::string& line);

    // print supported commands and usage.
    void print_help() const;

    // if spt_valid_ is false, recompute.
    void ensure_spt(int src);
};

#endif // EVENTHANDLER_HPP
