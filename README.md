# Dynamic Dijkstra Graph System

This is a command-line application for building, modifying, and querying an undirected weighted graph using Dijkstra’s algorithm, with support for **dynamic updates** to the shortest path tree (SPT).

## Features

- Add, remove, or update weighted edges in the graph.
- Query shortest path distances and paths using Dijkstra’s algorithm.
- Efficiently update shortest paths dynamically when edge weights change.
- Interactive event loop for real-time command execution.

## Files

- `Graph.cpp / Graph.hpp` – Basic undirected weighted graph representation using adjacency lists.
- `DynamicDijkstra.cpp / DynamicDijkstra.hpp` – Dijkstra's algorithm with dynamic update capabilities.
- `EventHandler.cpp / EventHandler.hpp` – Command parser and event loop.
- `main.cpp` – Entry point for the application.

## Build Instructions

This project uses C++17 or later.

### Using g++

```bash
g++ -std=c++17 src/main.cpp src/Graph.cpp src/DynamicDijkstra.cpp src/EventHandler.cpp -o dynroad
```

### Test Instructions

## Running Tests

To compile and run the test programs, follow these steps:

### Compile individual tests

```bash
g++ -std=c++17 -Wall -Wextra -Isrc -o test_graph tests/test_graph.cpp src/Graph.cpp
g++ -std=c++17 -Wall -Wextra -Isrc -o test_dijkstra tests/test_dijkstra.cpp src/DynamicDijkstra.cpp src/Graph.cpp
g++ -std=c++17 -Wall -Wextra -Isrc -o test_events tests/test_events.cpp src/EventHandler.cpp src/DynamicDijkstra.cpp src/Graph.cpp