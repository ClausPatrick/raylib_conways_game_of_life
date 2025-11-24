# Conway’s Game of Life

### Modern C++ Implementation with Raylib Rendering and Modular Rule Engine

---

## Overview

This project is a high-performance, extensible implementation of **Conway’s Game of Life**, written in **modern C++** and rendered with **Raylib**.
The design utilises object-oriented architecture, efficient memory handling, and a flexible rule system built around an **array of function pointers**, enabling alternative life/death behaviours beyond Conway’s original rules.

---

## Features

### Real-Time Simulation

* Uses Raylib for efficient 2D drawing.
* Each frame updates cell state using double-buffered grids to avoid state corruption.

### Modular Rule Engine (Function Pointer Array)

Rules are stored in an array indexed by neighbour count:

```cpp
bool (*m_rule_array[9])(bool value);
```

Each function determines the next value of a cell given its current state.
This enables alternative automata and experimentation without modifying the main simulation loop.

### Clean C++ Architecture

* Encapsulated `Cell` class holding position, state, and colour
* `World` class manages cell grid, buffers, neighbour calculation, and drawing
* Smart pointers (`std::unique_ptr<Cell>`) prevent ownership ambiguity
* Low-level performance with direct boolean buffers

### Configurable Compile-Time Options

The simulation can be tuned using preprocessor constants:

| Macro                   | Purpose                              |
| ----------------------- | ------------------------------------ |
| `SCREEN_W`, `SCREEN_H`  | Window resolution                    |
| `GRID_CELL_SIZE`        | Cell pixel size                      |
| `RAYLIB_ENABLED`        | Enable/disable rendering             |
| `ENABLE_SCREEN_CAPTURE` | Automatic screenshot output          |
| `LD_RATIO`              | Density of initial random live cells |

### Optional Screen Capture

Controlled via a compile-time flag, the simulation can automatically record output frames for visualization or analysis.

---

## Architecture

```
World
 ├── Cell objects (vector<unique_ptr<Cell>>)
 ├── m_cell_values[2]  → double-buffered boolean arrays
 ├── m_rule_array[9]   → function pointer rule table
 ├── cycle()           → simulation step
 ├── draw_cells()      → Raylib rendering
 └── neighbour lookup  → grid-index helper utilities
```

The separation of logic, rendering, and data buffers ensures clarity and makes the system easy to extend.

---

## Rule System

A core differentiator of this implementation is the **table-driven rule engine**:

```cpp
m_rule_array[0] = &rule_0_1;
m_rule_array[1] = &rule_0_1;
m_rule_array[2] = &rule_2__;
m_rule_array[3] = &rule_3__;
m_rule_array[4] = &rule_4_p;
...
```

During each cycle:

1. Neighbour count is computed.
2. That count indexes the rule array.
3. The function pointer determines the next state.
4. The new state is written to the off-buffer.
5. Buffers are swapped.

This results in a highly flexible, cleanly decoupled ruleset that can easily be replaced or expanded.

---

## Rendering

Rendering is handled strictly through Raylib:

* Grid lines drawn using `DrawRectangleLines`.
* Live cells drawn via `draw_colour_cell`.
* Grid is automatically centered using remainder offsets.
* Rendering can be fully disabled (`RAYLIB_ENABLED 0`) for console-based or headless runs.

This dual-mode configuration allows testing and batch runs without the graphical overhead.

---

## Build Instructions

### Dependencies

* C++17-compatible compiler
* Raylib library
* Standard C++ build tools (make, cmake, etc.)

### Example (Linux)

```bash
g++ -std=c++17 main.cpp -lraylib -lm -ldl -lpthread -o life
./life
```

Some platforms require:

```bash
-lGL -lX11
```

### Example Raylib Install (Linux)

```bash
sudo apt install libraylib-dev
```

---

## Recommended Project Layout (planned)


```
.
├── src/
│   ├── cell.cpp
│   ├── cell.hpp
│   ├── world.cpp
│   ├── world.hpp
│   ├── rules.hpp
│   └── main.cpp
├── screenshots/
├── CMakeLists.txt
└── README.md
```

---

## Future Enhancements

* Interactive editing (click to toggle cells)
* Multiple rulesets selectable at runtime
* Pattern library (gliders, guns, oscillators)
* GPU-accelerated update path
* Save/load world state
* Benchmark mode (headless execution)

---


