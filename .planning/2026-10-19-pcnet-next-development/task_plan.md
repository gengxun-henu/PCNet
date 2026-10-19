# Task Plan: PCNet 下一阶段开发

## Goal
在已合并的 PCNet 首期骨架之上，完成真实 ISIS 时间映射、模型感知质量评价与抽稀、可审计粗差过滤、CLI 完整流程和 1M 规模证据，使软件从可运行骨架进入可用于真实 ControlNet 数据的首个稳定版本。

## Next Step
P0 已完成；下一步进入 Milestone 2，验证真实 DetectorMap 曝光区间、midpoint 和按影像归一化时间契约。

## Current Phase
Phase 1: P0 环境与真实数据能力核验

## Phases

### Phase 1: P0 环境与真实数据能力核验
- [x] 核验 ISIS 10 头文件、库、ABI、CMake 导出和编译器信息
- [x] 完成真实 ControlNet 最小读写往返
- [ ] 验证 DetectorMap 曝光区间、midpoint 降级和行号插值能力
- [x] 明确 JIGSAW 可验证模型/约束事实；未验证内容不得进入接口
- [x] 记录 P0 报告、命令、输入摘要和失败原因
- **Status:** complete

### Phase 2: 时间契约与核心数据模型完善
- [ ] 统一 `TimingSource`/`ObservationTiming` 定义，消除重复头文件
- [ ] 为影像记录补充 acquisition group、channel/detector 和采集时间包络
- [ ] 按单张影像或采集组归一化时间，保留内部空档
- [ ] 让高阶模型按影像标记 timing unavailable/degraded
- [ ] 增加区间、midpoint、缺失时间和多通道夹具测试
- **Status:** pending

### Phase 3: 质量评价实现
- [ ] 实现默认 8、可配置 4–32 的时间分箱
- [ ] 实现横向 sample 分箱、空洞、起始/中部/终止覆盖
- [ ] 实现影像级/像对级/网络级指标及定义元数据
- [ ] 正确计算连通分量、桥边、弱影像、像对独立连接和 overlap fallback
- [ ] 排除 disabled 观测并验证 rank/condition/prediction variance
- **Status:** pending

### Phase 4: 模型感知抽稀
- [ ] 实现保护点、检查点、唯一连接像对和桥接保护
- [ ] 实现硬约束：预算、覆盖、连通性、模型秩、条件数和预测方差
- [ ] 实现增量 QR/Cholesky 与归一化 `logdet(N + λI)` 信息增益
- [ ] 实现确定性平局规则、无解审计和显式 best-effort 模式
- [ ] 与随机/Grid/现有 `cnetthinner` 做基线比较
- **Status:** pending

### Phase 5: 粗差过滤与 ISIS 写回
- [ ] 实现局部几何、可靠残差、MAD 和 DOM 辅助证据的多轮迭代
- [ ] 每轮检查连通性、保护项和模型可观测性
- [ ] 完整保存 reason code、阶段、阈值、单位、迭代和来源元数据
- [ ] 完善 sidecar 读取、ControlPoint/ControlMeasure 元数据保留和 `compact`
- **Status:** pending

### Phase 6: CLI、规模与发布
- [ ] 五个 CLI 命令完整支持 ControlNet 输入、配置、报告、错误码和原子写回
- [ ] 1M 验收记录运行时间、峰值内存、I/O、配置哈希和结果哈希
- [ ] 真实 ControlNet 子集回归测试和失败案例记录
- [ ] 更新开发文档、示例配置和复现命令
- **Status:** pending

### Phase 7: 论文规划与后续模块
- [ ] 软件稳定后生成 `PCNet_PAPER_SPEC.md`
- [ ] 设计 LRO NAC 主实验、独立检查点、消融和抽稀对照
- [ ] 单独评审 C4 DEM 桥接模块和 PCMatch v1 规格
- **Status:** pending

## Decisions Made
| Decision | Rationale |
|----------|-----------|
| 先做 P0 时间映射核验 | 时间来源会直接决定高阶模型质量评价和抽稀是否可信 |
| 首期单线程 | 已确认范围，先保证确定性和可审计性 |
| 高阶时间缺失采用分层处理 | 不伪造时间轴；可用影像参与高阶分析，其余影像仅做 constant/连通性分析 |
| 默认时间分箱 8，允许 4–32 | 兼顾稳定默认值与传感器/规模差异 |
| 默认可逆禁用，物理删除需显式 compact | 保留审计证据并降低误删风险 |
| ISIS ControlNet 是首期唯一标准格式 | 降低适配面，集中验证原生互操作 |
| C4 DEM 与 PCMatch v1 暂缓 | 与当前 C1/C2 核心目标解耦 |

## Errors Encountered
| Error | Resolution |
|-------|------------|
| 直接包含 ISIS 头文件缺少 `QMetaType`、`SpiceUsr.h`、`QLinkedList` | P0 CMake 导入补充 Qt6 Core、Qt6 Core5Compat、CSPICE include 路径 |
| 复核发现 CLI 初版仅返回成功码 | 后续修正为读取 PCNet 存储、执行 inspect/quality/thin/filter/validate 的基础流程，并支持 `.net` 输入路径 |
| 复核发现 timing source 与旧头文件重复定义 | 下一阶段 Phase 2 统一为单一 timing 契约 |

## Acceptance Gates

- P0 报告可由单条命令复现，且未验证 API 不进入接口承诺。
- 高阶模型在区间、midpoint、缺失时间和多通道样例上有可验算结果。
- 抽稀和过滤默认不破坏连通性、保护项和模型可观测性。
- ISIS 写回保留 ControlPoint 身份、禁用状态、旁车审计和可逆性。
- 1M ControlPoint 功能验收通过，并记录完整资源证据。
- 每个阶段完成后运行全套 CTest 并更新本目录 `progress.md`。
