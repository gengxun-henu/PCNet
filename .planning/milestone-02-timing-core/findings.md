# PCNet Milestone Registry

每个里程碑完成后必须通过新鲜验证、创建 PR、合并远程 main，并同步本地 main。

# Findings: 时间契约与核心模型完善

## Verified Facts

- Milestone ID: `milestone-02-timing-core`.
- `TimingSource`, `TemporalSupport`, and `ObservationTiming` now have one canonical core definition.
- Timing normalization uses each atomic image envelope and excludes disabled measures.
- Midpoint timing is represented by zero-duration support with a degraded flag.
- Full CTest suite: 16/16 passed.

## Evidence-Based Inference

- None recorded.

## Unresolved Items

- Completion evidence is prepared in the milestone close command.

## Decisions

| Decision | Rationale |
|---|---|

## Resources

- Canonical registry: `.planning/milestones.v1.json`
