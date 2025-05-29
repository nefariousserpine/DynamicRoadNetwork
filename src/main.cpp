#include <iostream>
#include "Graph.hpp"
#include "DynamicDijkstra.hpp"
#include "EventHandler.hpp"

int main() {
    // 1) Instantiate the core graph
    Graph graph;

    // 2) Wrap it in our DynamicDijkstra module
    DynamicDijkstra dijkstra(graph);

    // 3) Create the event handler that reads commands from stdin
    EventHandler handler(graph, dijkstra);

    // 4) Start the interactive event loop
    handler.run_event_loop();

    return 0;
}
