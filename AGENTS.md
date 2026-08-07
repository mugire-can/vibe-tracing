# AGENTS.md

## Repository mission
`vibe-tracing` is a high-performance C11 ray tracer focused on image quality, predictable builds, and practical rendering performance.

## Project analysis (current state)
- **Language/toolchain:** C11 + GCC + GNU Make.
- **Build outputs:** `raytracer` (single-frame), `raytracer_anim` (animation mode).
- **Runtime model:** CPU path tracing with POSIX threads.
- **Core architecture:** Header-oriented modular design (`vec3`, `ray`, `camera`, `material`, `texture`, `scene`, primitives).
- **Entry point:** `main.c` owns rendering configuration, scene construction, threading, and frame/image export.
- **Output formats:** PPM frames and optional MP4 conversion through FFmpeg.

## Canonical structure
- `/home/runner/work/vibe-tracing/vibe-tracing/main.c` — render loop, scene assembly, animation control, thread orchestration.
- `/home/runner/work/vibe-tracing/vibe-tracing/vec3.h` — vector math primitives.
- `/home/runner/work/vibe-tracing/vibe-tracing/ray.h` — ray model and operations.
- `/home/runner/work/vibe-tracing/vibe-tracing/camera.h` — camera and depth-of-field ray generation.
- `/home/runner/work/vibe-tracing/vibe-tracing/material.h` — BSDF/material scattering logic.
- `/home/runner/work/vibe-tracing/vibe-tracing/texture.h` — procedural textures (solid/checker/Perlin).
- `/home/runner/work/vibe-tracing/vibe-tracing/{sphere.h,plane.h,triangle.h}` — geometry intersections.
- `/home/runner/work/vibe-tracing/vibe-tracing/scene.h` — scene storage + hit dispatch.
- `/home/runner/work/vibe-tracing/vibe-tracing/color.h` — sampling accumulation + output encoding.
- `/home/runner/work/vibe-tracing/vibe-tracing/Makefile` — build, run, debug, benchmark, animation/video, cleanup targets.
- `/home/runner/work/vibe-tracing/vibe-tracing/README.md` — user/developer documentation and roadmap.

## Agent operating rules
1. Keep changes modular; preserve separation between math, geometry, materials, textures, and scene orchestration.
2. Prefer extending existing modules over adding monolithic logic to `main.c`.
3. Keep defaults stable unless explicitly requested (image size, samples, depth, threads, frame count).
4. Use existing Make targets for verification: `make`, `make debug`, `make benchmark`, and feature-specific runs.
5. Maintain portability (Linux/macOS/WSL) and avoid platform-locked dependencies.
6. Update README whenever behavior, workflows, or performance characteristics change.

## Continuous technology-improvement policy
Always prioritize improvements that increase correctness, maintainability, or performance without breaking baseline behavior:

### Near-term (highest impact)
- Introduce acceleration structures (BVH first, optional KD-tree later) to reduce intersection cost.
- Decouple render settings from source constants (CLI/env/config profile support).
- Add deterministic render mode (fixed seed) for reproducible benchmarking and regression checks.
- Add lightweight image-quality regression artifacts for representative scenes.

### Mid-term
- Refactor scene construction into dedicated scene modules/files.
- Add mesh/asset loaders (`.obj` first, optional `.gltf` next).
- Add importance sampling and variance-reduction strategies.
- Add progressive rendering mode for faster visual iteration.

### Long-term
- Evaluate SIMD intrinsics paths where profiling shows hotspots.
- Explore optional GPU backend (CUDA/OpenCL) while preserving CPU reference path.
- Add denoising pipeline integration as a post-processing stage.

## Decision framework for future changes
- **Measure first:** benchmark before/after for performance claims.
- **Preserve quality:** ensure physically plausible output remains intact.
- **Fail safe:** keep debug-friendly paths available (`make debug`).
- **Document always:** every meaningful feature/performance change must include docs updates.
