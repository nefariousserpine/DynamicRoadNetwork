#include <iostream>
#include "Graph.hpp"
#include "DynamicDijkstra.hpp"
#include "EventHandler.hpp"

int main() {
    Graph graph;

    DynamicDijkstra dijkstra(graph);

    EventHandler handler(graph, dijkstra);

    handler.run_event_loop();

    return 0;
}
