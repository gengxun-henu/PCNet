# PCNet — 完整设计上下文（供 Work / Codex App 使用）

版本：v0.1｜日期：2026-10-09｜状态：讨论设计，尚未本机核验

## 1. 项目定位与已确认决策

PCNet（Planetary Control Network Toolkit）是面向行星影像的大规模控制网构建、质量分析、优化和 DEM 高程控制的轻量级开源工具。目标论文为 *ISPRS Journal of Photogrammetry and Remote Sensing*（ISPRS P&RS）。

- 本地 ISIS 10 环境由用户指定为 `/home/gengxun/miniconda3/envs/asp370`；**尚未实际检查**头文件、库、ABI、版本、CMake 导出和 API。
- 默认 C++17/20，直接复用 ISIS 原生 C++ 的 Camera、ControlNet、ControlPoint、ControlMeasure 等能力；推荐独立 CMake 工程链接 ISIS 库，尽量不修改 ISIS 源码。可选 pybind11 Python API 和 CLI。
- 不重新实现 ISIS 相机模型、SPICE、完整 bundle adjustment；复用 `jigsaw`，需核验具体接口和约束语义。
- 用户已实现 DEM 渲染影像匹配并自动引入高程控制点的部分流程；先检查并复用原有代码，不能假定代码位置或成熟度。
- 交付计划：`PCNet_DEV_SPEC.md`（开发）与 `PCNet_PAPER_SPEC.md`（ISPRS 论文）；本文件用于跨 ChatGPT、Work、Codex App 传递设计背景。

## 2. 四项研究与软件贡献

### C1 — 相机改正模型感知的控制网质量评价（核心）

从固定连接点数升级为评价观测空间/时间分布能否支撑所选相机改正模型。按**每张影像、每个立体/重叠像对、全网**三级生成量化表格。通用指标：有效独立点数、每像对点数、Convex Hull 面积比、Grid Coverage、最大空洞、连接点密度、轨迹长度、连通分量、桥边、弱连接。沿轨指标：起始/中间/终止段覆盖、归一化时间跨度、最大时间空洞；必要时区分沿轨/跨轨。

初始模型：常差 `Δ(t)=a0`、速度/线性漂移 `Δ(t)=a0+a1 t`、加速度/二次项 `Δ(t)=a0+a1 t+a2 t²`。简化信息矩阵 `N=AᵀWA`，分析秩、条件数、信息量和预测方差 `h(t)ᵀΣ_a h(t)`；实际 ISIS 相机模型需进一步考虑地面点、其他参数、规范自由度和 Schur 消元。**均匀分布不总是数学最优**；尤其对高阶模型需起止与中部约束，采用“覆盖约束 + 模型信息优化”。HULL 不等于真实覆盖率，必须配合网格/空洞指标。跨经线、极区和不规则小天体应明确面积度量。

研究假设 H1：模型感知质量指标比固定点数或单独 HULL 更能预测平差稳定性与边缘外推误差。

### C2 — 百万级质量保持抽稀与 DOM 空间粗差剔除（核心）

首期验证 100 万 ControlPoint，扩展目标 1000 万；分别统计 ControlMeasure 数、内存、时间和 I/O。建议流程：质量预筛选 → 地表分块/候选压缩 → 关键桥接/弱连接保护 → 模型信息增益近似贪心选择 → 约束修复与验证。目标兼顾空间覆盖、网络连接、几何可靠性、参数可观测性；可研究 Schur 后信息矩阵 log-det，但要处理秩亏、参数尺度与计算成本。对比随机、Grid、KD-tree、ISIS 现有 `cnetthinner` 等方法。

DOM 粗差检测不能将有系统姿态误差的正射坐标偏移直接视为误匹配；先检验局部相对几何一致性，再结合平差标准化重投影残差、鲁棒统计和地表坐标差异迭代过滤。二维 DOM 检验不能取代高度/三维几何检验。记录每个剔除原因，防止误删。

研究假设 H2：在相同点数预算下，模型感知抽稀能更好保留独立检查点精度、连通性和稳定性。

### C3 — 深度学习匹配互操作与多视轨迹（软件贡献）

设计版本化 `PCMatch` 中转格式，兼容传统 `.key`/SIFT 与 SuperGlue、LightGlue 等外部匹配输出；`.key` 不作为唯一规范。PairMatch 至少保存 image_a/image_b、feature_id、sample/line、score、可选测量协方差、source_id；头部记录像素坐标原点、影像 ID ↔ ISIS SerialNumber、行星参考系、单位、版本与来源。Track 保存多影像 Observation、质量、可选三维坐标与协方差。提供二进制高性能格式和 CSV/JSONL 调试格式。

使用 DSU/Union-Find 快速构建候选轨迹，但**不能无条件传递合并**；同一轨迹同一影像出现两个不同观测时必须进行冲突检测、图分割或可信子图选择，再做多视几何验证。匹配分数不能未经校准直接充当统计权值。大规模数据采用 SoA、64 位索引、分块 I/O/外存排序。

研究假设 H3：统一格式及冲突消解改善跨匹配器轨迹可靠性与 ISIS 互操作效率；格式本身主要是工程贡献。

### C4 — 独立多分辨率 DEM 高程控制模块（重要方法贡献）

用户已有：DEM 渲染成影像，与 LRO NAC DOM 匹配，关联原始影像观测与 DEM 地形，自动生成高程控制点；DEM 平面精度有限。新问题：DEM 渲染分辨率较低，直接与高分辨率 NAC DOM 匹配困难。

支持路径 A：`DEM Render ↔ NAC DOM`；路径 B：`DEM Render ↔ 中间分辨率 DOM ↔ NAC DOM/待处理影像`。可依据分辨率比、纹理、初步内点率自动选路径。跨尺度匹配必须验证组合几何一致性与双向关系；记录来源链与共享数据相关性。建立 DEM 高程/平面分量不等精度的先验，优先在局部 ENU 坐标表达协方差；考虑 DEM 坡度、配准误差、系统偏差、相机误差及共享 DOM/DEM 相关性。ISIS 10 是否支持完整非对角协方差需核验；如果只支持各分量 sigma，应明确这是近似。模块应可独立启用、测试和发布。

研究假设 H4：中尺度 DOM 桥接提升经几何验证的有效高程控制点覆盖；合理的高程强约束/平面弱约束改善独立高程精度而不引入明显系统变形。

## 3. 目标工程架构

```text
PCNet/
├── core/{io,tracks,metrics,models,thinning,filtering}/
├── adapters/isis/        # ISIS 10 原生 C++ 适配
├── modules/dem/         # Render/Match/Bridge/Validate/Uncertainty/ControlNet
├── python/              # 可选 pybind11
├── cli/
├── schemas/             # PCMatch v1
├── tests/
├── benchmarks/
├── examples/
├── docs/{PCNet_DESIGN_CONTEXT.md,PCNet_DEV_SPEC.md,PCNet_PAPER_SPEC.md}
└── paper/               # LaTeX 与实验图表
```

核心算法尽量与 ISIS 类型解耦；ISIS 适配层使用原生 API，保证 ControlNet 点/观测/约束/SerialNumber 语义。所有拟议命令与 API 在检查本机环境前均视为草案。

## 4. 必须生成的量化质量表

**ImageQuality**：image_id、sensor、camera_correction_model、n_points、n_measures、n_neighbors、hull_ratio、grid_coverage、largest_gap、start/mid/end_coverage、time_span、max_time_gap、design_rank、condition_number、max_prediction_variance、warning_flags。

**PairQuality**：image_a、image_b、overlap_area、n_valid_tracks、n_independent_tracks、hull_ratio、grid_coverage、along/cross_track_span、max_gap、stereo_geometry_score、bridge_criticality、warning_flags。

**NetworkQuality**：n_images、n_points、n_measures、n_components、n_bridge_edges、point_count_quantiles、weak_images、weak_pairs、coverage_summary、information_summary、runtime、peak_memory。

必须明确指标分母、有效重叠区域、坐标/投影、网格尺度、无效值规则及模型之间可比性。输出 CSV/JSON，可选 Parquet、HTML、GeoJSON。综合评分只能辅助排序，不取代原始指标。

## 5. 分阶段开发与验收

| 阶段 | 工作 | 验收证据 |
|---|---|---|
| P0 | 只读检查 ISIS 10、已有 DEM 代码及 Git 项目；最小原生 C++ 读写 | 版本/API/编译报告，ControlNet 往返测试 |
| P1 | PCMatch v1、ISIS IO、多视轨迹及冲突消解 | 格式规范、往返测试、冲突样例 |
| P2 | 三级质量表、HULL/覆盖、0/1/2 阶模型指标 | 人工可验算的小样例、报告快照 |
| P3 | 百万级抽稀与 DOM 粗差检验 | 基线比较、性能/内存、误删统计 |
| P4 | 复用 DEM 现有代码，新增桥接、误差传播、ISIS 高程约束 | 直接/桥接对比、独立检查点实验 |
| P5 | Python API、CLI、打包、复现与论文数据 | CI、文档、开源许可、可重现实验 |

实施顺序可以依据 P0 调查调整，但 C1 的模型感知指标应先于最终抽稀算法稳定。

## 6. ISPRS P&RS 论文规格基础

暂定英文题目：*PCNet: A Model-Aware and Multi-Resolution Framework for Large-Scale Planetary Image Control Network Construction and Quality Optimization*。

建议章节：Introduction；Architecture and Data Model；Camera-Model-Aware Network Quality Assessment；Quality-Preserving Network Optimization；Matching Interoperability and Track Construction；Multi-Resolution DEM Elevation Control；Experiments and Discussion；Conclusions。

论文主线：**C1 + C2 是方法核心，C4 是重要摄影测量应用创新，C3 是开源软件与可复现性贡献**。若篇幅过长，C3 可压缩为架构/补充材料。采用期刊当前接受的 Elsevier LaTeX 模板；`elsarticle` 是候选，必须核验官方作者指南，不能用 ISPRS Congress 会议模板代替。

实验：月球 LRO NAC 为主要场景，建议另选火星 CTX/HiRISE 等独立场景；10万/100万/1000万点规模；不同改正模型和连接点分布；随机/Grid/KD-tree/ISIS 抽稀对照；普通 DSU 与冲突消解对照；DEM 直接与桥接对照；固定权与误差传播权值对照。报告独立检查点误差、收敛、残差、连通、覆盖、运行时间、峰值内存、消融及失败案例。全量控制网平差结果不可直接当作真值。

## 7. Superpowers Brainstorming 待确认事项

按优先级逐轮**每次只问一个问题**，已确认事项不要重新争论：

1. P0 检查：ISIS 10 实际 API、jigsaw 支持的改正模型和先验约束方式；现有 DEM 代码位置及复用边界。
2. C1 首版模型支持：先用通用 0/1/2 阶代理模型，还是直接从 ISIS 真实参数偏导建立信息矩阵？推荐先代理、后严格验证。
3. 质量表中“影像起止段”的时间定义、分段阈值、有效像对重叠区域、投影与分辨率尺度。
4. PCMatch 是否以特征 ID 为强标识，无法保证 ID 稳定时如何进行坐标容差/冲突处理。
5. DEM 高程约束在 ISIS 10 中的类型与权值映射；如何处理共享 DEM 系统误差。
6. 目标数据集、独立高程/平面真值与资源预算（CPU/RAM/存储）。
7. 开源许可证、跨平台支持范围、发布与论文时间节点。

## 8. 交给 Codex App / Work 的启动指令

> 先完整阅读 `docs/PCNet_DESIGN_CONTEXT.md`。将“已确认决定”“尚待验证假设”“开放问题”分别列出。先只读检查 `/home/gengxun/miniconda3/envs/asp370` 的 ISIS 10 开发环境和已有 DEM 代码；不要声称已验证未检查的 API。采用 Superpowers brainstorming 逐轮提问，一次一个关键问题，不重复已确认的决策。在用户确认设计后，生成可执行 `PCNet_DEV_SPEC.md`（C++/ISIS 10）和 `PCNet_PAPER_SPEC.md`（ISPRS P&RS LaTeX），随后使用 writing-plans 拆分为可测试、可验收任务。正式批准前不大规模修改代码。

## 9. 资料完整性声明

本文件是截至 2026-10-09 对当前对话中 PCNet 设计内容的结构化归档，**不是逐字聊天转录**，也不是已经验证的 ISIS 10 API 说明。凡涉及环境、已有实现、期刊模板和算法效果的事实，均须在后续阶段独立核实。
