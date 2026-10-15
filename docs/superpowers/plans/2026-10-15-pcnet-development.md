# PCNet Development Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans (native execution) to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build the first single-threaded C++17 PCNet core and CLI for ISIS 10 ControlNet inspection, model-aware quality evaluation, auditable thinning, and reversible outlier filtering.

**Architecture:** Keep the core independent of ISIS types. Represent ControlNet data in a compact indexed network view, expose timing through `ObservationTiming`, compute model-aware metrics in the core, and isolate ISIS read/write and camera timing in `adapters/isis`. The CLI orchestrates versioned configuration and atomic output while preserving audit data in a sidecar JSON report.

**Tech Stack:** C++17, CMake, ISIS 10 native libraries from `/home/gengxun/miniconda3/envs/asp370`, single-threaded execution, CSV/JSON reports, CTest-based deterministic tests, optional Eigen only if the P0 environment confirms a compatible package; otherwise use a focused internal small-matrix implementation for the initial model metrics.

**Spec:** [docs/PCNet_DEV_SPEC.md](/home/gengxun/code/CNET_TOOLS/docs/PCNet_DEV_SPEC.md)

## Global Constraints

- ISIS `ControlNet` is the only first-release internal standard format and main I/O format.
- C4 DEM processing and PCMatch v1 are deferred; do not add them to this implementation.
- The public model names are exactly `constant`, `velocity`, and `acceleration`; one model is selected per run.
- Coordinates use `float64`; default indices use `uint32`; state flags use `uint8`; reason codes use versioned `uint16`.
- Timing priority is DetectorMap exposure interval, single ephemeris midpoint, explicit line interpolation, then unavailable; high-order models never fabricate an unavailable time axis.
- Time coverage defaults to 8 bins and accepts only configured values from 4 through 32.
- Quality reports expose raw metrics and sparse flags; thinning thresholds are explicit configuration values.
- Thinning is single-threaded, deterministic, and uses hard constraints before model information gain and coverage objectives.
- Outlier filtering is reversible by default: disable and preserve observations; physical deletion requires explicit `compact`.
- A 1M-ControlPoint run is a first-release functional acceptance gate; 10M is an extension stress target.

## Review Focus

- Missing or inconsistent observation timing: high-order analysis must mark affected images unavailable and exclude them from high-order thinning candidates; test in `tests/core/timing_policy_test.cpp`.
- Exposure intervals versus midpoint timing: interval first and midpoint fallback must produce different, auditable temporal moments; test in `tests/core/temporal_support_test.cpp`.
- Rank-deficient model geometry: quality and thinning must report rank deficiency without silently accepting an invalid high-order model; test in `tests/core/model_metrics_test.cpp`.
- No feasible thinning solution: no edited ControlNet is written unless the explicit best-effort flag is enabled; test in `tests/core/thinning_constraints_test.cpp`.
- Reversible ISIS writeback: disabled observations and sidecar reason codes must survive round trip, while `compact` is opt-in; test in `tests/adapters/isis_roundtrip_test.cpp`.

### Task 1: P0 environment probe and baseline capture

**Files:**
- Create: `CMakeLists.txt`
- Create: `cmake/FindIsis10.cmake`
- Create: `tools/p0_probe.cpp`
- Create: `tools/p0_probe_report.py`
- Create: `docs/p0/README.md`
- Create: `tests/p0_probe_test.cpp`

**Interfaces:**
- Produces `pcnet_p0_probe` with `--env /home/gengxun/miniconda3/envs/asp370 --output <report.json>`.
- Report fields: ISIS version, compiler, CMake import/link facts, headers, library/ABI facts, ControlNet read/write capability, timing capability, and explicit unknowns.

- [ ] **Step 1: Write the failing probe test** asserting the report schema contains environment path, library path, ControlNet capability, timing capability, and a failure list.
- [ ] **Step 2: Run `ctest -R p0_probe` and verify it fails because the probe target is absent.**
- [ ] **Step 3: Implement the CMake ISIS discovery and probe executable; inspect headers without assuming unverified JIGSAW APIs.**
- [ ] **Step 4: Implement report generation and the read-only minimum ControlNet compile/link probe.**
- [ ] **Step 5: Run `cmake -S . -B build -DCMAKE_PREFIX_PATH=/home/gengxun/miniconda3/envs/asp370`, build, and run the probe; record success or exact failure reasons in `docs/p0/README.md`.**
- [ ] **Step 6: Commit `chore: add ISIS 10 P0 probe and build skeleton`.**

### Task 2: Core indexed network view and versioned storage

**Files:**
- Create: `include/pcnet/core/network_view.hpp`
- Create: `include/pcnet/core/reason_codes.hpp`
- Create: `include/pcnet/core/storage.hpp`
- Create: `src/core/network_view.cpp`
- Create: `src/core/storage.cpp`
- Create: `tests/core/network_view_test.cpp`
- Create: `tests/core/storage_test.cpp`

**Interfaces:**
- `ImageRecord`, `PointRecord`, `MeasureRecord`, `TemporalSupport`, and `NetworkView` with `uint32_t` indices and `double` sample/line coordinates.
- `StorageWriter::write(path, view, StorageOptions)` and `StorageReader::read(path) -> NetworkView`.
- `ReasonCode` is a stable `uint16_t` enum plus `reason_code_name()`.

- [ ] **Step 1: Write failing tests for indexed image/point/measure references, optional quality columns, state flags, reason codes, and round-trip preservation.**
- [ ] **Step 2: Run the focused tests and verify they fail because core types and storage are absent.**
- [ ] **Step 3: Implement the compact SoA-oriented in-memory view and validation of index ranges, field widths, and required metadata.**
- [ ] **Step 4: Implement one versioned binary storage format with optional quality columns and an explicit header schema.**
- [ ] **Step 5: Run the focused tests and a malformed-file test; expected result is PASS with stable error messages.**
- [ ] **Step 6: Commit `feat: add indexed PCNet network view and storage`.**

### Task 3: Timing adapter contract and model design metrics

**Files:**
- Create: `include/pcnet/core/timing.hpp`
- Create: `include/pcnet/core/error_model.hpp`
- Create: `src/core/timing.cpp`
- Create: `src/core/error_model.cpp`
- Create: `tests/core/temporal_support_test.cpp`
- Create: `tests/core/timing_policy_test.cpp`
- Create: `tests/core/model_metrics_test.cpp`

**Interfaces:**
- `enum class ErrorModel { constant, velocity, acceleration };`
- `enum class TimingSource { detector_interval, ephemeris_midpoint, line_interpolation, unavailable };`
- `ObservationTiming { TemporalSupport support; TimingSource source; bool degraded; }`.
- `normalize_time(view, unit, bins=8)`, `temporal_moments(support)`, and `ModelMetrics analyze_model(view, ErrorModel, ModelConfig)`.

- [ ] **Step 1: Write failing deterministic tests for interval moments, midpoint fallback, missing timing, 4–32 bin validation, and `[1]`/`[1,tau]`/`[1,tau,tau^2]` rank/condition behavior.**
- [ ] **Step 2: Run the tests and verify the timing/model functions are missing.**
- [ ] **Step 3: Implement interval-union normalization to `[-1,1]`, preserving internal gaps and separating temporal integration weight from observation quality weight.**
- [ ] **Step 4: Implement model design metrics with rank-first checks, fixed column normalization, and a numerically stable small-matrix path.**
- [ ] **Step 5: Run tests covering rank-deficient velocity/acceleration cases and confirm unavailable timing is reported rather than fabricated.**
- [ ] **Step 6: Commit `feat: add timing contract and model metrics`.**

### Task 4: Image, pair, and network quality reports

**Files:**
- Create: `include/pcnet/metrics/quality.hpp`
- Create: `src/metrics/quality.cpp`
- Create: `src/metrics/report_writer.cpp`
- Create: `tests/metrics/quality_report_test.cpp`
- Create: `tests/metrics/report_schema_test.cpp`

**Interfaces:**
- `ImageQuality`, `PairQuality`, `NetworkQuality` structs with metric-definition metadata.
- `QualityReport evaluate_quality(const NetworkView&, const QualityConfig&)`.
- `write_quality_csv_json(report, output_dir)`.

- [ ] **Step 1: Write failing tests for time/cross-track coverage, 8-bin default, sparse flags, overlap fallback, connectivity summaries, and report denominator metadata.**
- [ ] **Step 2: Run the focused tests and verify they fail.**
- [ ] **Step 3: Implement image-level time and sample coverage, start/middle/end thirds, model rank/condition/prediction variance, and warnings.**
- [ ] **Step 4: Implement pair-level overlap and fallback diagnostics plus network components, bridges, weak images, and quantiles.**
- [ ] **Step 5: Implement deterministic CSV and JSON serialization with configuration/input/version hashes.**
- [ ] **Step 6: Run all quality tests and commit `feat: add model-aware quality reports`.**

### Task 5: ISIS ControlNet adapter and reversible round trip

**Files:**
- Create: `include/pcnet/adapters/isis/controlnet_adapter.hpp`
- Create: `src/adapters/isis/controlnet_adapter.cpp`
- Create: `tests/adapters/isis_roundtrip_test.cpp`
- Modify: `cmake/FindIsis10.cmake`

**Interfaces:**
- `ControlNetAdapter::read(path, TimingPolicy) -> NetworkView`.
- `ControlNetAdapter::write(path, view, WriteMode, AuditSidecar)`.
- `ObservationTimingProvider` backed by ISIS Camera/DetectorMap, with explicit midpoint and line interpolation fallbacks.

- [ ] **Step 1: Write a fixture-based failing test for ControlNet read, disabled-measure preservation, optional residual fields, and sidecar reason codes.**
- [ ] **Step 2: Run the test and verify it is blocked by the absent adapter implementation, recording any genuine ISIS API mismatch as a P0 finding.**
- [ ] **Step 3: Implement mapping from ISIS SerialNumber/ControlPoint/ControlMeasure to the indexed view without copying unused ISIS fields.**
- [ ] **Step 4: Implement DetectorMap timing priority and explicit degraded flags; do not expose unverified JIGSAW units or interfaces.**
- [ ] **Step 5: Implement atomic writeback, native disabled state mapping where available, sidecar audit JSON, and opt-in `compact`.**
- [ ] **Step 6: Run the round-trip fixture and P0 report; commit `feat: add ISIS ControlNet adapter`.**

### Task 6: Deterministic constrained thinning

**Files:**
- Create: `include/pcnet/thinning/thinner.hpp`
- Create: `src/thinning/thinner.cpp`
- Create: `tests/core/thinning_constraints_test.cpp`
- Create: `tests/core/thinning_determinism_test.cpp`

**Interfaces:**
- `ThinConfig` with point/measure budgets, explicit coverage/model thresholds, protection sets, and `allow_constraint_violations=false`.
- `ThinResult thin(const NetworkView&, const ThinConfig&)`.

- [ ] **Step 1: Write failing tests for protected points, bridge preservation, budget handling, rank/coverage hard constraints, no-feasible-result behavior, and deterministic tie breaks.**
- [ ] **Step 2: Run the tests and verify they fail.**
- [ ] **Step 3: Implement invalid/duplicate prefiltering while retaining original records and reason codes.**
- [ ] **Step 4: Implement single-threaded feasible candidate selection with incremental QR/Cholesky, rank-first checks, normalized `logdet(N + lambda I)` gain, and stable index tie breaks.**
- [ ] **Step 5: Implement constraint-conflict reporting and explicit best-effort mode; default result must be audit-only when infeasible.**
- [ ] **Step 6: Run deterministic repeated trials and commit `feat: add constrained model-aware thinning`.**

### Task 7: Auditable geometry/DOM outlier filtering

**Files:**
- Create: `include/pcnet/filtering/outlier_filter.hpp`
- Create: `src/filtering/outlier_filter.cpp`
- Create: `tests/core/outlier_filter_test.cpp`

**Interfaces:**
- `FilterConfig` with residual thresholds, MAD settings, iteration limit, and protection constraints.
- `FilterResult filter_outliers(const NetworkView&, const FilterEvidence&, const FilterConfig&)`.

- [ ] **Step 1: Write failing tests for local geometry first, external residual thresholds second, DOM evidence third, iteration stopping, and reversible disable reasons.**
- [ ] **Step 2: Run the tests and verify they fail.**
- [ ] **Step 3: Implement local relative-geometry checks and robust MAD fallback only when reliable residual fields are absent.**
- [ ] **Step 4: Implement explicit residual source/units/threshold handling and DOM evidence as supporting evidence rather than a sole decision.**
- [ ] **Step 5: Re-check connectivity, protected observations, and model observability after each disable; emit unresolved candidates in the audit report.**
- [ ] **Step 6: Run tests and commit `feat: add auditable outlier filtering`.**

### Task 8: CLI, configuration, and stable error contract

**Files:**
- Create: `include/pcnet/cli/config.hpp`
- Create: `src/cli/config.cpp`
- Create: `src/cli/main.cpp`
- Create: `tests/cli/cli_contract_test.cpp`
- Create: `examples/pcnet_config.yaml`
- Modify: `CMakeLists.txt`

**Interfaces:**
- Commands: `pcnet inspect`, `quality`, `thin`, `filter`, `validate`.
- Exit codes: `0` success, `10` warning, `20` input/config, `30` timing unavailable, `40` I/O/resource, `50` internal.
- Shared versioned YAML configuration and JSON status/audit output.

- [ ] **Step 1: Write failing CLI contract tests for command dispatch, config validation, exit codes, atomic output, and warning/error JSON.**
- [ ] **Step 2: Run the tests and verify the CLI target is absent or incomplete.**
- [ ] **Step 3: Implement config parsing and validation, including model names, 4–32 bin range, explicit thinning thresholds, and single-threaded execution.**
- [ ] **Step 4: Implement command orchestration over the core and ISIS adapter interfaces.**
- [ ] **Step 5: Implement stable errors, temporary-file writes, atomic replacement, and report fields for completed/uncompleted stages.**
- [ ] **Step 6: Run CLI contract tests and commit `feat: add pcnet CLI`.**

### Task 9: Scale validation, baseline comparison, and release evidence

**Files:**
- Create: `benchmarks/benchmark_1m.cpp`
- Create: `benchmarks/README.md`
- Create: `tools/compare_baseline.py`
- Create: `tests/integration/one_million_point_test.cpp`
- Create: `docs/validation/P0_REPORT.md`
- Create: `docs/validation/1M_RESULTS.md`

**Interfaces:**
- Deterministic synthetic generator with fixed seed and configurable ControlPoint/ControlMeasure counts.
- Benchmark output records input sizes, runtime, peak memory, I/O, configuration hash, and result hash.

- [ ] **Step 1: Write the integration test for a deterministic large synthetic network and expected count/constraint invariants.**
- [ ] **Step 2: Run a small scaled version to verify the harness fails before the benchmark generator exists.**
- [ ] **Step 3: Implement the generator, benchmark runner, and memory/time instrumentation without introducing multithreading.**
- [ ] **Step 4: Run the P0 environment probe and 1M acceptance workload; compare against the existing thinning baseline where inputs are compatible.**
- [ ] **Step 5: Record pass/fail evidence, limitations, and any unavailable ISIS capabilities in the validation documents.**
- [ ] **Step 6: Commit `test: add PCNet scale and baseline validation evidence`.**

## Execution Order

Tasks are sequential because each later task consumes interfaces from the previous one:

`1 P0/build → 2 core/storage → 3 timing/models → 4 quality → 5 ISIS adapter → 6 thinning → 7 filtering → 8 CLI → 9 scale evidence`

Do not begin C4 DEM or PCMatch implementation in this plan. They require separate approved specs.
