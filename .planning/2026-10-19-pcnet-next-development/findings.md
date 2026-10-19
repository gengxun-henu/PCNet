# Findings & Decisions

## Requirements

- 当前 PCNet 首期代码已合并到本地和远程 `main`，对应 PR #4。
- 下一阶段目标是从可运行骨架推进到真实 ISIS ControlNet 数据可用版本。
- 优先级是 P0 真实环境核验、时间契约、模型质量评价、模型感知抽稀、审计过滤、完整 CLI 和规模证据。
- 单线程、C++17、ISIS 10、ControlNet 主格式、C4/PCMatch 暂缓继续有效。

## Research Findings

- ISIS 环境：`/home/gengxun/miniconda3/envs/asp370`。
- 可见头文件包括 `ControlNet.h`、`ControlPoint.h`、`ControlMeasure.h`、`LineScanCamera.h` 和 `PushFrameCameraDetectorMap.h`。
- 关键依赖路径包括 Qt6 Core、Qt6 Core5Compat 和 CSPICE。
- `PushFrameCameraDetectorMap` 提供 `StartEphemerisTime()`、`exposureDuration()`、framelet rate 等接口；真实观测级映射仍需用相机/DetectorMap 夹具核验。
- `ControlMeasure` 提供 sample/line、DateTime、residual、sigma、ignored/rejected 等字段；DateTime 不自动等价于曝光区间。
- 当前质量、抽稀、过滤和 P0 探针已经有可运行最小实现，但仍需补齐真实数据语义和严格指标。
- 当前 1M 测试验证了功能和运行时间；峰值内存、I/O、配置哈希和结果哈希仍待补充。

## Technical Decisions

| Decision | Rationale |
|----------|-----------|
| 以 P0 真实 API 事实作为下一阶段入口 | 避免继续围绕未验证 API 扩展实现 |
| 对时间来源保留 source/degraded 标志 | 让 midpoint、线性插值和缺失时间可审计 |
| 按影像/采集组建立局部时间轴 | 避免不同影像绝对时间混用导致秩和条件数失真 |
| 抽稀采用硬约束优先、信息增益其次 | 保证连通性和模型可观测性不被预算优化破坏 |
| DOM 证据只能辅助粗差决定 | 防止把系统姿态或配准偏差误判为错配 |
| CLI 对 ControlNet 和轻量存储共享核心流程 | 便于测试并保持 ISIS 适配层边界清晰 |

## Issues Encountered

| Issue | Resolution |
|-------|------------|
| 当前仓库没有独立的后续路线记录 | 创建本目录的 task_plan/findings/progress 三文件 |
| 旧 timing 头文件与当前核心定义存在重复类型风险 | Phase 2 统一定义并删除重复接口 |
| 质量和抽稀最小实现与正式 SPEC 仍有差距 | 将差距拆为 Phase 3–5，禁止把当前骨架误称为最终算法 |

## Resources

- 开发 SPEC：[docs/PCNet_DEV_SPEC.md](../../docs/PCNet_DEV_SPEC.md)
- 首期实现计划：[docs/superpowers/plans/2026-10-15-pcnet-development.md](../../docs/superpowers/plans/2026-10-15-pcnet-development.md)
- P0 说明：[docs/validation/P0_REPORT.md](../../docs/validation/P0_REPORT.md)
- 远程 PR：[https://github.com/gengxun-henu/PCNet/pull/4](https://github.com/gengxun-henu/PCNet/pull/4)
