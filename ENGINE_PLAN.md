# Zoo Tycoon Engine — Build Plan (Low-Poly 3D)

**Stack:** C++17, SDL3, OpenGL 3.3, glm, stb_image, Dear ImGui (debug only)
**Art direction:** flat-shaded low-poly. No textures. No skeletal animation. Think *Islanders* / *Townscaper*.
**Budget:** 4–6 hrs/week → a sprint is 2 weeks ≈ 10 hours
**Target:** ~36 weeks (9 months) to something ugly and playable

---

## The art constraint is load-bearing

Flat-shaded, untextured, procedurally-generated meshes are the entire reason this project is survivable solo. The moment you add textures, PBR, or rigged animation, you've signed up for asset work you cannot produce and cannot afford. Animals bob, hop, and rotate via transform animation — that reads as *stylised*, not *unfinished*.

If you break this rule, break it in Phase 7, deliberately, with a working game already behind you.

---

## How to use this file

- One sprint at a time. Do not read ahead and start optimising for Sprint 14.
- Tick boxes as you go. The ticks are the progress bar.
- A sprint that overruns is fine. A sprint you skip ahead of is not.
- If a task turns out to be two weeks on its own, split it and shift everything right. The dates were always fiction; the order is not.

## Session ritual

1. Open `LOG.md`, read the last entry.
2. 20-minute minimum. You must start. You do not have to finish.
3. Stop **mid-task** — leave a broken build or a `// TODO: next` comment.
4. One commit, however trivial.
5. One line in `LOG.md`: what you did, what confused you.

## The anti-scope list

Do not build these. Each one has killed a hobby engine:

- [ ] ~~Skeletal animation / rigging~~ — Phase 7 at the earliest, ideally never
- [ ] ~~Textures, normal maps, PBR~~ — flat shading is the art direction, not a placeholder
- [ ] ~~glTF / FBX model loading~~ — you generate meshes in code until Phase 7
- [ ] ~~Shadow mapping~~ — Phase 7. Blob shadows are enough until then.
- [ ] ~~Generic ECS framework~~ — not until 3+ entity types demand it
- [ ] ~~Job system / thread pool~~ — not until you've profiled a real bottleneck
- [ ] ~~Custom scripting language~~
- [ ] ~~Asset pipeline / packer~~
- [ ] ~~Your own UI framework~~ — ImGui for tools, hand-rolled game UI in Phase 6 only
- [ ] ~~Vulkan~~
- [ ] ~~Physics engine~~ — you need ray-plane intersection, not Jolt
- [ ] ~~Networking / multiplayer~~

Ideas you want to build anyway go in `IDEAS.md`. They are respected there. They do not derail the sprint.

---

# PHASE 0 — Skeleton

## Sprint 1 — Window and loop

**Goal:** one command builds it, a window opens, a counter ticks.

- [X] `git init`, `.gitignore` for build artifacts, first commit
- [X] Repo layout: `src/`, `assets/`, `vendor/`, `CMakeLists.txt`
- [X] CMake building a hello-world binary
- [X] SDL3 vendored (FetchContent or submodule) and linking
- [X] Window opens, event loop handles quit and window close
- [X] Clear screen to a non-black colour
- [X] Fixed-timestep loop with an accumulator — separate `update(dt)` from `render(alpha)`
- [X] Frame timing: measure frame ms, track a rolling average
- [ ] Dear ImGui vendored, initialised, drawing a debug overlay
- [ ] Overlay shows frame ms, FPS, update count
- [ ] `README.md` with build instructions that actually work from a clean clone

**Done when:** clean clone → `cmake --build` → window with live FPS counter.

**Gotchas:** the accumulator loop is the single most important thing here. Read Gaffer On Games "Fix Your Timestep" before you write it. Getting this wrong now means every simulation bug for the next year is timing-related and undebuggable.

---

# PHASE 1 — 3D renderer

## Sprint 2 — First mesh in 3D space

**Goal:** a lit cube, rotating, in perspective, with correct depth.

- [ ] OpenGL 3.3 core context via SDL, function loading (glad)
- [ ] `glCheckError()` macro — write it now, call it everywhere, save yourself weeks
- [ ] Shader class: load from file, compile, link, report errors legibly
- [ ] Uniform setters (mat4, vec3, vec4, float, int)
- [ ] Mesh class: VAO/VBO/EBO, interleaved vertex format (position, normal, colour)
- [ ] Hardcoded cube mesh with per-face normals (24 verts, not 8 — flat shading needs split normals)
- [ ] glm integrated; model / view / projection matrices
- [ ] Perspective projection, sensible near/far planes
- [ ] Depth testing enabled, back-face culling enabled
- [ ] Rotating cube on screen

**Done when:** a lit cube spins, faces are distinctly shaded, nothing z-fights.

**Gotchas:** flat shading requires each face to have its own vertices with its own normal. If your cube looks smoothly gradient-shaded, you shared vertices — split them. Also: near plane at 0.1 not 0.001, or your depth buffer precision dies.

## Sprint 3 — Camera and mesh generation

**Goal:** you can fly around a scene of procedurally generated shapes.

- [ ] Orbit camera: target point, yaw, pitch, distance
- [ ] Middle-drag to orbit, scroll to zoom, right-drag or WASD to pan the target
- [ ] Pitch clamping (never flip over the top)
- [ ] Directional light + ambient term in the fragment shader
- [ ] Per-face flat shading via `flat` interpolation qualifier or split normals
- [ ] Mesh builder API: `addQuad()`, `addBox()`, `addPrism()`, `addCylinder(sides)`, `addCone(sides)`
- [ ] Vertex colour support — colour is per-vertex, no textures anywhere
- [ ] Generate a scene: ground plane, a few boxes, a cone "tree", a cylinder
- [ ] ImGui panel to tweak light direction, ambient strength, clear colour

**Done when:** a small procedural low-poly scene, and you can orbit it comfortably enough to lose five minutes just looking at it.

**Gotchas:** the mesh builder is the most reused code in the entire project. Every building, tree, animal and prop comes out of it. Spend the extra session making the API pleasant.

## Sprint 4 — Instancing and batching

**Goal:** thousands of objects, a handful of draw calls.

- [ ] Static mesh registry — build a mesh once, reference it by handle
- [ ] Instanced rendering: per-instance model matrix + colour tint in a second VBO
- [ ] Instance buffer rebuild per frame (optimise later, correctness now)
- [ ] Group draws by mesh handle
- [ ] Stress test: 20,000 instanced boxes at random transforms
- [ ] ImGui stats: draw calls, instances submitted, triangles, VRAM estimate
- [ ] Sorting: opaque geometry front-to-back for early-z
- [ ] Wireframe toggle for debugging

**Done when:** 20,000 objects at 60fps in single-digit draw calls.

**Gotchas:** if you're at 20,000 draw calls, your instancing isn't wired up — the counter in the overlay exists to catch exactly this. `glVertexAttribDivisor` is the function you'll get wrong first.

## Sprint 5 — Culling and picking

**Goal:** off-screen costs nothing, and the mouse can touch the world.

- [ ] AABB per mesh instance
- [ ] Frustum extraction from the view-projection matrix
- [ ] Frustum culling against instance AABBs
- [ ] Verify by panning — instance count in the overlay must change
- [ ] Screen-space mouse position → world-space ray (unproject)
- [ ] Ray / ground-plane intersection
- [ ] Ray / AABB intersection for object picking
- [ ] Debug draw: line renderer for rays, AABBs, and grid lines
- [ ] Selected-object ImGui inspector

**Done when:** click anything in the scene and it highlights; culling visibly cuts the instance count as you turn the camera.

**Gotchas:** unprojecting a mouse ray involves inverting the view-projection matrix and getting the NDC z convention right. Test it by drawing the ray as a debug line — if picking is broken you'll *see* where the ray actually went.

---

# PHASE 2 — The world grid

## Sprint 6 — Terrain

**Goal:** a real, chunked, walkable world surface.

- [ ] Tilemap data structure — flat `std::vector<Tile>`, width × height, index math
- [ ] `Tile` struct: terrain type, height, flags
- [ ] Terrain mesh generation from tile data — two triangles per tile, flat-shaded, vertex-coloured by terrain type
- [ ] Chunking: split the map into 16×16 chunks, one mesh per chunk
- [ ] Dirty-flag system — only regenerate chunks that changed
- [ ] Per-chunk frustum culling
- [ ] Tile height support with correct normals at slopes
- [ ] 256×256 map performance check

**Done when:** a 256×256 terrain with varied heights renders at 60fps and only rebuilds the chunks you edit.

**Gotchas:** naive "rebuild the whole terrain mesh on every edit" will feel fine at 64×64 and be unusable at 256×256. Build the chunk dirty-flag system now, not later.

## Sprint 7 — Grid interaction

**Goal:** the mouse knows exactly which tile it's over, even on slopes.

- [ ] Ray → tile coordinate (raycast against the terrain, not a flat plane)
- [ ] Hover highlight rendered on the tile surface, conforming to its slope
- [ ] Out-of-bounds handling (cursor off the map)
- [ ] Multi-tile selection by drag (rectangle)
- [ ] Terrain raise/lower tool with a brush radius
- [ ] Terrain smoothing, and slope-limit rules
- [ ] Terrain type painting (grass, sand, water, rock)
- [ ] Camera clamping to map bounds
- [ ] Debug view: tile coordinates as world-space text or gizmos

**Done when:** hover highlight is accurate on steep slopes at any camera angle, and you can sculpt a hill that looks intentional.

**Gotchas:** raycasting against heightmapped terrain is meaningfully harder than against a flat plane. March the ray in fixed steps, find the first step below the surface, then binary-search that interval. Good enough, and simple.

---

# PHASE 3 — Building and saving

## Sprint 8 — Placement tools

**Goal:** you can build things.

- [ ] Tool state machine: none / terrain / path / fence / object / demolish
- [ ] Ghost preview mesh following the cursor, tinted by validity
- [ ] Path placement, single tile and click-drag lines
- [ ] Fence placement on tile **edges**, not tile centres — different data model, think this through
- [ ] Fence mesh generation: posts and rails, corner handling
- [ ] Object placement (stalls, benches, trees) with rotation snapping
- [ ] Placement validation (terrain type, slope limit, occupancy)
- [ ] Invalid-placement feedback (red ghost)
- [ ] Demolish tool with hover confirmation
- [ ] ImGui toolbar to switch tools

**Done when:** you can lay out a recognisable zoo in under two minutes and it looks like a place.

**Gotchas:** fences-on-edges vs. objects-on-tiles is the first real data-modelling decision of the project. Get it wrong and Sprint 16's enclosure detection becomes miserable. Spend a full session thinking about it before writing code.

## Sprint 9 — Serialization

**Goal:** it survives a restart.

- [ ] Save format decision — binary blob, versioned with a magic number and version int
- [ ] Write tilemap, heights, placed objects, fences
- [ ] Read it all back and regenerate all chunk meshes
- [ ] Save/load hotkeys, then an ImGui file panel
- [ ] Handle corrupt or wrong-version files without crashing
- [ ] Autosave every N minutes
- [ ] Round-trip test: save, load, save again, diff the files — must be byte-identical

**Done when:** you build a zoo, quit, relaunch, and it is exactly as you left it.

**Gotchas:** version the format from the very first save. You *will* change these structs in Sprint 12 and you don't want that to mean deleting all your test zoos.

---

# PHASE 4 — Agents

## Sprint 10 — Entities that move

**Goal:** things that aren't terrain, walking around.

- [ ] Entity storage — struct-of-arrays, plain and boring. **Not** a generic ECS.
- [ ] Entity ID + generation counter (so stale IDs are detectable)
- [ ] Spawn / despawn with free-list reuse
- [ ] Position in world-space floats, snapped to terrain height
- [ ] Entities render as instanced low-poly meshes (a capsule-ish stack of boxes is fine)
- [ ] Facing direction from velocity; smooth turning
- [ ] Blob shadows — a dark, slightly-offset quad under each entity, conforming to ground height
- [ ] Straight-line movement toward a target with arrival tolerance
- [ ] Spawn 100 entities moving to random points

**Done when:** 100 little figures walk the terrain, follow the ground surface up hills, and don't look like they're floating.

**Gotchas:** blob shadows are the single cheapest thing you can do to stop low-poly objects looking like they're hovering. Do not skip them and do not upgrade to shadow mapping instead.

## Sprint 11 — Pathfinding

**Goal:** they route around obstacles.

- [ ] Walkability grid derived from tiles, slopes, fences and objects
- [ ] A* over the tile grid with a proper priority queue
- [ ] Octile heuristic; understand why not Euclidean
- [ ] Slope cost — steep tiles cost more, impassable above a threshold
- [ ] Path smoothing (drop redundant waypoints, string-pull where line of sight allows)
- [ ] Entities follow the waypoint list in world space
- [ ] Repath when the world changes under them
- [ ] Debug draw: selected entity's path as a 3D line strip
- [ ] Debug draw: visited-node heatmap on the terrain
- [ ] Handle "no path exists" without hanging or crashing

**Done when:** 100 entities navigate a maze of fences and hills without clipping through anything.

**Gotchas:** unbounded A* on 256×256 with 100 agents repathing every frame will destroy your framerate. Cap searched nodes, repath on demand only.

## Sprint 12 — Simulation tick

**Goal:** simulation time separates from render time.

- [ ] Simulation tick decoupled from render frame (e.g. 10 sim ticks/sec)
- [ ] Interpolated rendering between ticks — this is what `alpha` was for in Sprint 1
- [ ] Speed controls: pause, 1×, 2×, 4×
- [ ] Game clock: day/hour, shown in the overlay
- [ ] Day/night light colour and direction shift (cheap, and it sells the world enormously)
- [ ] Spread pathfinding across ticks so no single tick spikes
- [ ] Profile 500 entities; fix only what's actually slow
- [ ] Spatial hash for "entities near position X" queries

**Done when:** 500 agents pathfinding at 4× speed, still 60fps, movement still smooth, and the sun moves.

---

# PHASE 5 — Needs and behaviour

## Sprint 13 — Guest needs

**Goal:** guests want things.

- [ ] Need components: hunger, thirst, bathroom, energy, entertainment
- [ ] Linear decay per sim tick
- [ ] Placeable amenities: food stall, drink stall, toilet, bench — each a generated low-poly mesh
- [ ] Amenity registry queryable by need type
- [ ] Utility scoring — most urgent need weighted by distance to a satisfier
- [ ] Path to the chosen amenity, use it, satisfy the need over N ticks
- [ ] Use animation via transform only (bob, scale, rotate — no rigs)
- [ ] Selected-entity inspector with need bars and current goal
- [ ] Guests leave when overall satisfaction bottoms out

**Done when:** you can watch a hungry guest independently find and use a food stall, and see the bar refill.

**Gotchas:** this is the first sprint that feels like a *game*. Record a GIF. Post it somewhere public.

## Sprint 14 — Animals

**Goal:** the actual attraction.

- [ ] Animal species data in an external file (JSON or your own format) — not hardcoded
- [ ] Species defines: diet, space requirement, terrain preference, mesh recipe, colours
- [ ] Procedural animal meshes from the species recipe (body box, head, four leg boxes, tail)
- [ ] Animal needs: hunger, thirst, health, happiness
- [ ] Wander behaviour within bounds
- [ ] Confinement — animals must never path out through a fence
- [ ] Idle transform animation: head bob, tail sway, hop-walk
- [ ] Guests path toward enclosure edges and gain entertainment from viewing
- [ ] Viewing requires line of sight — raycast from guest to animal
- [ ] Animal inspector panel

**Done when:** guests gather at a fence to watch an animal, and the animal looks alive without a single bone in it.

**Gotchas:** "mesh recipe in a data file" is what lets you add a species in 10 minutes instead of a weekend. It's also where your low-poly art direction actually pays for itself.

## Sprint 15 — Staff

**Goal:** problems get fixed by someone other than you.

- [ ] Staff entities: keeper, janitor, mechanic — distinguished by colour, not model
- [ ] Task queue — problems post tasks, staff claim them
- [ ] Litter spawns from guests; janitors clean it
- [ ] Animals need feeding; keepers feed them
- [ ] Task claiming with no double-claiming (this is the bug you will hit)
- [ ] Staff idle behaviour when the queue is empty
- [ ] Assignable patrol zones, drawn on the terrain
- [ ] Debug view of the open task queue

**Done when:** you scatter litter across the map and janitors distribute themselves sensibly to deal with it.

---

# PHASE 6 — Enclosures and economy

## Sprint 16 — Enclosure detection

**Goal:** the game understands what a "pen" is.

- [ ] Flood fill from a tile, blocked by fence edges
- [ ] Detect whether a region is fully enclosed
- [ ] Enclosure struct: tile list, area, contained animals, contained objects
- [ ] Incremental recompute on fence change — not a full-map rebuild
- [ ] Quality score: size vs. species requirement, terrain match, foliage, water, cleanliness
- [ ] Animal happiness driven by enclosure score
- [ ] Warnings for unsuitable enclosures
- [ ] Debug view: tint each detected enclosure a distinct colour on the terrain

**Done when:** you close a fence loop and an enclosure appears instantly with a computed score; break the loop and it vanishes.

**Gotchas:** the most algorithmically interesting sprint in the project. Enjoy it. Full-map recompute on every fence placed will be visibly slow — that's your cue to go incremental.

## Sprint 17 — Economy

**Goal:** you can go bankrupt.

- [ ] Money, income, expenses
- [ ] Construction costs on every placeable
- [ ] Ticket pricing; arrival rate responds to price and reputation
- [ ] Staff wages, animal food costs, monthly upkeep
- [ ] Guest spending at stalls
- [ ] Reputation from animal happiness, guest satisfaction, cleanliness
- [ ] Monthly financial report
- [ ] Bankruptcy / game-over state
- [ ] Loans (optional, only if it's still fun to build)

**Done when:** a badly-run zoo goes bankrupt within 15 minutes of play.

## Sprint 18 — Real UI

**Goal:** ImGui is no longer the game.

- [ ] 2D overlay pass — orthographic, depth test off, drawn after the 3D scene
- [ ] Nine-slice panel rendering
- [ ] Bitmap font rendering with kerning
- [ ] Buttons with hover and press states
- [ ] Build menu with categories and thumbnails
- [ ] HUD: money, date, guest count, speed controls
- [ ] Entity inspector as a real panel
- [ ] Financial report screen
- [ ] Notification / alert feed
- [ ] World-space markers that project to screen space (e.g. "!" over an unhappy animal)
- [ ] ImGui now debug-only, behind a toggle key

**Done when:** you can play a full session without opening a single ImGui window.

---

# PHASE 7 — Your game

## Sprint 19+

Now — and only now — open `IDEAS.md`. Half of it will have been superseded by something better you thought of in Sprint 14. Pick the three that still excite you and scope them into sprints using this same format.

Also unlocked here, if and only if the game is playable first:

- Shadow mapping (single directional cascade — huge visual payoff for a low-poly scene)
- Simple post-processing: SSAO, FXAA, colour grading
- Water shader with vertex-displaced waves
- Particle system (weather, dust, litter)
- glTF loading, if you ever want externally-authored props
- Sound and music
- Tutorial and onboarding

Skeletal animation stays on the anti-scope list. If you still want it after a year of transform animation, you'll know exactly what you're signing up for.

---

## Escape hatches

Read this section on the night you want to quit.

- **Stuck on one bug for 3+ sessions?** Stub it out, comment `// FIXME`, move to the next task. Come back in a fortnight with fresh eyes.
- **Sprint dragging past 4 weeks?** It was too big. Split it. That's a planning failure, not a you failure.
- **Refactor still broken after one week?** `git revert`. Non-negotiable. Long-lived dead branches are the #1 killer of projects like this.
- **Lost momentum entirely?** Do a 20-minute session on the *easiest* remaining task in the current sprint. Not the most important one — the easiest one. Momentum first, priority second.
- **Bored of the current phase?** Allowed: one session on anything in `IDEAS.md`, then back. Not allowed: starting a new phase early.
- **Tempted to buy an asset pack?** That's the art wall arriving early. Re-read the top of this file.

---

## Progress log

| Sprint | Phase | Started | Finished | Screenshot? |
|--------|-------|---------|----------|-------------|
| 1 | Skeleton | | | |
| 2 | Renderer | | | |
| 3 | Renderer | | | |
| 4 | Renderer | | | |
| 5 | Renderer | | | |
| 6 | Grid | | | |
| 7 | Grid | | | |
| 8 | Building | | | |
| 9 | Building | | | |
| 10 | Agents | | | |
| 11 | Agents | | | |
| 12 | Agents | | | |
| 13 | Needs | | | |
| 14 | Needs | | | |
| 15 | Needs | | | |
| 16 | Economy | | | |
| 17 | Economy | | | |
| 18 | Economy | | | |
