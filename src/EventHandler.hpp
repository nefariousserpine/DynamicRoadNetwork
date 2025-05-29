#ifndef EVENTHANDLER_HPP
#define EVENTHANDLER_HPP

#include "Graph.hpp"
#include "DynamicDijkstra.hpp"
#include <string>

/**
 * @brief Parses and handles dynamic events on the graph and shortest‐path module.
 * 
 * Supported commands (one per line, case‐sensitive):
 *   ADD u v w       — Add an undirected edge (u, v) with weight w.
 *   REMOVE u v      — Remove the undirected edge (u, v).
 *   UPDATE u v w    — Update the weight of existing edge (u, v) to w.
 *   QUERY u v       — Print the shortest‐path distance and node sequence from u to v.
 *   PRINT           — Print the current adjacency list of the graph.
 *   HELP            — Print a brief help message.
 *   EXIT            — Terminate the event loop (optional; also stops on EOF).
 * 
 * Notes on dynamic handling:
 * - After an UPDATE (weight change), if a shortest‐path tree (SPT) is already
 *   computed, we attempt a localized repair via DynamicDijkstra::update_edge().
 *   If that fails (e.g. edge not found), we defer to a full recompute on next QUERY.
 * - After ADD or REMOVE, we mark the SPT as invalid and force a full recompute
 *   on the next QUERY, because dynamic insertion/deletion is not fully supported.
 */
class EventHandler {
public:
    /**
     * @brief Construct a handler over the given graph and DynamicDijkstra module.
     * @param g  Reference to an existing Graph instance.
     * @param d  Reference to an existing DynamicDijkstra instance (wrapping the same Graph).
     */
    EventHandler(Graph& g, DynamicDijkstra& d);

    /**
     * @brief Start reading commands from stdin (std::cin) until EOF or EXIT.
     * 
     * For each line, parse the command and invoke the corresponding graph or
     * dynamic‐Dijkstra operation. Output is printed to stdout.
     */
    void run_event_loop();

private:
    Graph& graph_;
    DynamicDijkstra& dijkstra_;

    int last_source_;              ///< The source of the last successful SPT compute (or -1).
    bool spt_valid_;               ///< True if current SPT (in DynamicDijkstra) is valid.

    /**
     * @brief Parse one line of input and dispatch the appropriate operation.
     * 
     * @param line  A single line of text (one command).
     */
    void process_command(const std::string& line);

    /**
     * @brief Print usage instructions to stdout.
     */
    void print_help() const;

    /**
     * @brief Attempt to ensure that the SPT is valid for source = src.
     * 
     * If the last_source_ differs from src, or spt_valid_ is false, recompute
     * the SPT via dijkstra_.compute(src). Updates last_source_ and spt_valid_.
     * Otherwise, do nothing (reuse existing SPT).
     * 
     * @param src  Desired SPT source node.
     */
    void ensure_spt(int src);
};

#endif // EVENTHANDLER_HPP
