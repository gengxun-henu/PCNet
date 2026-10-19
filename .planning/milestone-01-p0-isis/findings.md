# PCNet Milestone Registry

每个里程碑完成后必须通过新鲜验证、创建 PR、合并远程 main，并同步本地 main。

# Findings: P0 ISIS 10 / ControlNet / DetectorMap 核验

## Verified Facts

- Milestone ID: `milestone-01-p0-isis`.
- ISIS environment `/home/gengxun/miniconda3/envs/asp370` contains `ControlNet.h`, `PushFrameCameraDetectorMap.h`, and `libisis10.0.0.so`.
- Native ControlNet construction, PVL write, reopen, and point/measure count round trip passed.
- Probe report records compiler `12.4.0`, 64-bit pointers, and compile-time DetectorMap header availability.
- A bare ControlNet does not provide runtime camera context for observation-specific exposure intervals; this remains the explicit next-milestone boundary.

## Evidence-Based Inference

- None recorded.

## Unresolved Items

- Completion evidence is prepared in the milestone close command.

## Decisions

| Decision | Rationale |
|---|---|

## Resources

- Canonical registry: `.planning/milestones.v1.json`
