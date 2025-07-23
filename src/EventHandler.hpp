#ifndef EVENTHANDLER_HPP
#define EVENTHANDLER_HPP

#include "Graph.hpp"
#include "DynamicDijkstra.hpp"
#include <string>

/* 
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
    EventHandler(Graph& g, DynamicDijkstra& d);

    void run_event_loop();

    void test_process_command(const std::string& cmd);

private:
    Graph& graph_;               
    DynamicDijkstra& dijkstra_;  

    int last_source_;            
    bool spt_valid_;             

    void process_command(const std::string& line);

    void print_help() const;

    void ensure_spt(int src);
};

#endif 
