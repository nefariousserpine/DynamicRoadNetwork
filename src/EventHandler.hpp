#ifndef EVENTHANDLER_HPP
#define EVENTHANDLER_HPP

#include "Graph.hpp"
#include "DynamicDijkstra.hpp"
#include <string>

/**
 * @brief Handles dynamic graph events and shortest-path queries.
 * 
 * Supports commands:
 *   ADD u v w       - Add an undirected edge (u, v) with weight w.
 *   REMOVE u v      - Remove the undirected edge (u, v).
 *   UPDATE u v w    - Update weight of edge (u, v) to w.
 *   QUERY u v       - Print shortest-path distance and nodes from u to v.
 *   EXISTS u v      - Check if undirected edge (u, v) exists.
 *   PRINT           - Print current graph adjacency list.
 *   HELP            - Print help message.
 *   EXIT            - Exit event loop (also ends on EOF).
 * 
 * Notes:
 * - After UPDATE, attempts localized SPT update via DynamicDijkstra::update_edge().
 *   If that fails, marks SPT invalid for full recompute on next QUERY.
 * - After ADD or REMOVE, marks SPT invalid to force full recompute on next QUERY.
 */
class EventHandler {
public:
    /**
     * @brief Construct EventHandler over given Graph and DynamicDijkstra.
     * @param g Reference to existing Graph instance.
     * @param d Reference to existing DynamicDijkstra instance.
     */
    EventHandler(Graph& g, DynamicDijkstra& d);

    /**
     * @brief Run interactive event loop reading commands from stdin.
     * Outputs results to stdout.
     */
    void run_event_loop();

    /**
     * @brief Method intended for unit tests only
     * Outputs results to stdout.
     */
    void test_process_command(const std::string& cmd);

private:
    Graph& graph_;                 ///< Reference to graph instance.
    DynamicDijkstra& dijkstra_;   ///< Reference to dynamic shortest-path module.

    int last_source_;              ///< Source node of last computed SPT (-1 if none).
    bool spt_valid_;               ///< True if current shortest-path tree is valid.

    /**
     * @brief Parse and execute one input command line.
     * @param line Single input command line.
     */
    void process_command(const std::string& line);

    /**
     * @brief Print usage help message.
     */
    void print_help() const;

    /**
     * @brief Ensure SPT is computed and valid for given source.
     * If needed, recomputes SPT from scratch.
     * @param src Source node for shortest-path tree.
     */
    void ensure_spt(int src);
};

#endif // EVENTHANDLER_HPP
