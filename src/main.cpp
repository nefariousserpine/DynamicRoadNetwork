#include <iostream>
#include "Graph.hpp"
#include "DynamicDijkstra.hpp"
#include "EventHandler.hpp"

int main() {
    // Instantiate the core graph
    Graph graph;

    // Wrap it in DynamicDijkstra module
    DynamicDijkstra dijkstra(graph);

    // Create the event handler that reads commands from stdin
    EventHandler handler(graph, dijkstra);

    // Start the interactive event loop
    handler.run_event_loop();

    return 0;
}
