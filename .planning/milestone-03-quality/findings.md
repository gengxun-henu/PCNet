# PCNet Milestone Registry

每个里程碑完成后必须通过新鲜验证、创建 PR、合并远程 main，并同步本地 main。

# Findings: 三级模型感知质量评价

## Verified Facts

- Milestone ID: `milestone-03-quality`.
- ImageQuality now reports configured time bins, cross-track bins, start/middle/end coverage, active-measure counts, and model rank/condition.
- PairQuality reports shared tracks, spans, and explicit overlap fallback.
- NetworkQuality computes active-image connected components and weak images.
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
