# PCNet Milestone Registry

每个里程碑完成后必须通过新鲜验证、创建 PR、合并远程 main，并同步本地 main。

# Milestone Index: PCNet staged development

- Project ID: `pcnet`
- Schema version: `1`

| Order | ID | Title | Status | Dependencies | Plan | Last close |
|---:|---|---|---|---|---|---|
| 1 | `milestone-01-p0-isis` | P0 ISIS 10 / ControlNet / DetectorMap 核验 | `complete` | — | `.planning/milestone-01-p0-isis` | complete |
| 2 | `milestone-02-timing-core` | 时间契约与核心模型完善 | `in_progress` | `milestone-01-p0-isis` | `.planning/milestone-02-timing-core` | — |
| 3 | `milestone-03-quality` | 三级模型感知质量评价 | `pending` | `milestone-02-timing-core` | `.planning/milestone-03-quality` | — |
| 4 | `milestone-04-thinning` | 模型感知质量保持抽稀 | `pending` | `milestone-03-quality` | `.planning/milestone-04-thinning` | — |
| 5 | `milestone-05-filter-writeback` | 可审计粗差过滤与 ISIS 写回 | `pending` | `milestone-04-thinning` | `.planning/milestone-05-filter-writeback` | — |
| 6 | `milestone-06-cli-scale` | 完整 CLI 与百万级发布证据 | `pending` | `milestone-05-filter-writeback` | `.planning/milestone-06-cli-scale` | — |
| 7 | `milestone-07-paper-followup` | 论文规格与后续模块规划 | `pending` | `milestone-06-cli-scale` | `.planning/milestone-07-paper-followup` | — |

This file is generated from `.planning/milestones.v1.json`; do not edit it directly.
