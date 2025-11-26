# **Cellular Automata Renderer**

A high-performance, multithreaded C implementation of a parametric cellular automata simulation.  
The program supports multiple rule sets (Life-like automata), live mode-switching, gradients, randomization density, and interactive controls.  
Rendering is done directly to the terminal using block characters for efficiency.

---

## **Features**

### **Automata Rules**
The engine supports multiple *Life-like* automata. Rules follow the classical `Bx/Sy` notation:

- **Conway’s Life** — `B3/S23`  
- **HighLife** — `B36/S23`  
- **Diamoeba** — `B35678/S5678`  
- **Seeds** — `B2/S`  
- **Live Free or Die** — `B2/S0`  
- **Serviettes** — `B234/S`  
- **Anneal** — `B4678/S35678`  
- **Replicator** — `B1357/S1357`  


Internally, the engine applies rule logic via a function-pointer array, allowing each “species” to define its own update behavior (birth, survival, flip, or perish) without complicating the core simulation loop.

New automata can be added by simply adding a rule function and plugging it into the rule-dispatch array.

---

## **Terminal Controls**

### **Simulation**
- **Space** — pause/resume simulation   
- **R** — randomize field (followed by digit (0~9) for density)  
- **g** — gradient rendering  
- **m** — enter mode-selection UI to select automata species 
- **q** — quit

### **Mouse**
- **Left Click** — toggle/add live cell  
- **Right Click** — clear cell  


Mouse input works in both paused and running states.

---

## **Mode Selection UI**

Press **m** to open the rule-selection window.

- Use **↑ / ↓** to scroll through available automata species  
- Press **Enter** to apply the selected rule  
- The simulation resumes using rule and preserves density/visual settings  
- Press **Esc** to cancel

This makes switching between Life-like species trivial.

---

## **Display / Rendering**

- Full-terminal rendering using Unicode block characters   
- HUD line at the bottom showing:

  - current automaton  
  - generation count  
  - living cell count  
  - density   


---

## **Building**

```bash
g++ -std=c++17 main.cpp -lraylib -lm -ldl -lpthread -o life ./life
```



---

## **Adding a New Automaton**

1. Define a rule function, e.g.:

```c
uint8_t rule_birth3(uint8_t v) { return 1; }
```

2. Add it to your rule table:

```c
m_rule_array[3] = &rule_birth3;
```

3. Add an entry to the automaton list for the UI:

```c
{ "Replicator", B1357, S1357, RULE_FLIP }
```

The engine will automatically incorporate it into the selector and apply it at runtime.

---

## **Randomization Density**

Press **d** or **D** to adjust the percentage (0–100%) used when reseeding the grid.  
Displayed in the HUD as a readable value.

---

## **Gradient**

- gradient mode with **g**

---

## **Notes**

- Designed for Linux terminals that support mouse input (xterm, Alacritty, etc.)  
- Performance improves significantly on wider terminals  
- Perfect for experimentation with Life-like automata behavior
