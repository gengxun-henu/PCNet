# 1M ControlPoint Validation

The first-release functional gate is `tests/integration/one_million_point_test.cpp`. It creates a deterministic single-image network with one million ControlPoints and one million ControlMeasures, runs the single-threaded thinning path, and checks feasibility and cardinality preservation. Runtime and memory must be recorded in the execution environment; no cross-machine absolute time limit is imposed.
