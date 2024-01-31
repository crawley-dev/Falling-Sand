# Cellular Automata Simulation
## - Falling Sand Simulation
## - Conway's Game of Life

#### Inspired By: Noita's Falling sand, Sandspiel & The Powder Toy

#### Created With: SDL2, OpenGL & ImGui.

#### Using HIC++ Coding Standard.

## Roadmap:

##### Importance Hierarchy: ↓

### Vector Acceleration:

- Re-write cell structure:
  - add x,y acceleration
  - store as 4 bits each, 0-15, 1 byte of data total
- floats too expensive to store, not doing += (gravity \* deltaTime)..
- y = mx + c algorithm for traversing non 1x1 y:x vectors.

### Saving // Loading game state:

- Save:

  - interfaceData then cell state
  - to .txt file or .json? <-- https://github.com/nlohmann/json
  - as binary or numbers?

- Load:
  - read all interfaceData (known length in bytes)
  - read cell state, don't need width & height to parse.. but will get
  - ... load all this shite into correct objects & we good.

### Loading && saving Images:

- average pixels to materials, otherwise it breaks due to variants
  being used & not cell RGBA values.
- Therefore, need to implement more materials to get somewhat accurate results

### Multi-threading:

- Column Multi-threading:
  - Multi-thread in 2 passes, doing all even // odd columns for deterministic behaviour (so threads don't update adjacent cells position without respect for each other's updated cell state).
- Grid Multi-threading:
  - still 2 passes, but more threads dispatched for a patchwork grid of chunks

### Sleeping Chunks optimisation:

- Divide game state into chunks, storing 2 values, doUpdate this frame & next frame.
- update():
  - update_this_frame = update_next_frame
  - update_next_frame = false
- loop through, if no cells within a chunk move:
  - update_next_frame = false;
- if element on border of chunk updates: neighbouring chunks: update_next_frame = true
- if element in chunk updates: update_next_frame = true
