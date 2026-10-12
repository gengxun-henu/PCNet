# PCNet 开发设计 Brainstorm 会话交接

## Session checkpoint

- Primary workflow: standalone handoff（由 `writing-session-checkpoints` 自动检测；无 active plan/superpowers plan）
- CWD: `/home/gengxun/code/CNET_TOOLS`
- Branch/worktree: `main`, 工作区在保存前干净
- Goal: 基于仓库设计上下文，逐轮形成程序开发 SPEC；ISPRS 论文 SPEC 暂后处理
- Current phase or plan task: Superpowers brainstorming，已完成范围、架构、轻量数据表示和模型职责边界的初步确认，尚未完成质量指标/抽稀/错误处理/测试设计，尚未生成最终 `PCNet_DEV_SPEC.md`
- Completed:
  - 确认 C4（DEM 高程控制）本期不实现，作为未来外部模块集成。
  - 确认 C1/C2 为本期方法核心，C3 首期只围绕 ISIS `ControlNet` 做互操作扩展点。
  - 确认 `constant`、`velocity`、`acceleration` 为 PCNet 公共模型名，单张线阵影像内部沿采集时间变化，全网统一选择一个模型。
  - 确认 PCNet 不映射 JIGSAW 底层单位、类名或接口；用户在后续 JIGSAW 独立选择模型。
  - 确认正式交付面为 C++ 核心库 + CLI，Python 后续扩展。
  - 确认 ISIS `ControlNet` 是首期唯一内部标准格式；外部格式转换到 `ControlNet`；PCMatch v1 延后。
  - 确认核心算法与 ISIS 类型解耦，由 ISIS 适配层连接。
  - 确认 100 万 ControlPoint 为首期必须验收规模，1000 万为扩展压力测试目标。
  - 确认轻量网络视图：影像索引、点索引、观测索引；观测只保留必要坐标和状态；名称集中存储。
  - 确认坐标直接使用 `float64`；默认索引 `uint32`，状态 `uint8`，原因码 `uint8/uint16`。
  - 确认状态/原因码必须保留；质量值按需、按语义明确的可选列保留。
  - 确认采用单一版本化轻量格式，质量字段可选，可形成 minimal/quality 存储配置。
  - 已将当前设计写入 `docs/PCNet_DEV_DESIGN_CURRENT.md`。
- Files changed:
  - `docs/PCNet_DEV_DESIGN_CURRENT.md`
  - `docs/handoffs/2026-10-09-pcnet-dev-brainstorm.md`
- Validation completed:
  - 已读取 `PCNet_DESIGN_CONTEXT.md`。
  - 已读取并执行 brainstorming 与 writing-session-checkpoints 的工作流要求。
  - 已运行 workflow detection：standalone，ready=true。
  - 已检查 git 状态和运行进程；无开发/测试后台进程。
- Review status: 当前设计记录尚未完成用户最终审阅；仍处于 brainstorming 阶段。
- Running processes: 无本任务相关进程。
- Do not repeat:
  - 不要重新讨论 C4 是否本期实现。
  - 不要重新讨论 ISIS `ControlNet` 是否为首期主格式。
  - 不要重新引入 PCMatch v1 首期验收。
  - 不要把 JIGSAW 单位映射写回 PCNet 职责。
  - 不要把完整 ISIS `ControlMeasure` 复制到核心算法结构。
- Remaining risks:
  - 尚未核验本机 `/home/gengxun/miniconda3/envs/asp370` 的 ISIS 10 头文件、库、ABI、CMake 导出和 ControlNet API。
  - 尚未定位或检查外部 C4 仓库；当前按黑盒外部模块处理。
  - 时间映射/归一化、质量指标分母、有效重叠区域、网格尺度、抽稀目标函数和粗差过滤规则未定。
  - `velocity/acceleration` 的具体数据设计需保持与“单张线阵影像内部采集时间”一致，但不依赖 JIGSAW 底层单位。
- Exact next action: 继续 brainstorming，一次讨论一个关键问题；优先确认时间归一化与缺失时间策略，然后细化三类质量表、抽稀/粗差过滤、CLI、测试和 P0 验收。设计章节确认后再生成正式 `PCNet_DEV_SPEC.md`。
- Recommended resume skill: `superpowers:brainstorming`（继续当前架构型设计）；正式 SPEC 获用户确认后，再使用 `superpowers:writing-plans`。

## 主要对话决策记录

1. 用户要求：基于当前仓库形成程序开发 SPEC 和 ISPRS 论文 SPEC；随后明确先做程序开发 SPEC，并先进行 brainstorming。
2. 助手检查仓库：当前只有 `PCNet_DESIGN_CONTEXT.md`，其中包含 PCNet 的定位、C1–C4、架构草案、里程碑和待确认问题。
3. 用户确认：P0 先只读核验 ISIS 10 环境、JIGSAW 和已有 DEM 代码；C4 已在其他仓库实现，本期不做，后续集成。
4. 用户说明：误差模型是控制网处理阶段用户输入的 `constant`、`velocity`、`acceleration`；目标是让控制网编辑考虑后续 JIGSAW 将使用的模型。
5. 经确认：模型描述单张线阵影像内部沿采集时间的误差变化；JIGSAW 当前要求全网统一模型；PCNet 也采用全网单模型。
6. 用户确认：正式交付面为 C++ 核心库 + CLI；100 万 ControlPoint 首期必须通过，1000 万为扩展目标。
7. 用户澄清：所有控制网编辑处理基于 ISIS `ControlNet`；其他格式通过公开/外部转换导入 `ControlNet`；PCMatch v1 延后设计。
8. 用户选择：采用“核心算法 + ISIS 适配层”路线，核心更独立，ISIS 变化集中在适配层维护。
9. 用户要求控制网轻量化：不复制 ISIS `ControlMeasure` 的无用字段；影像名称用索引；坐标字段使用浮点；随后确认首期直接使用 `float64`。
10. 经讨论确认：状态与原因码必须保留；质量值可选；使用一个版本化格式，质量列可选，不拆成两个不兼容格式。
11. 用户补充：PCNet 的误差模型不需要考虑后续 JIGSAW 的单位；PCNet 自己处理控制网阶段的模型，后续 JIGSAW 独立使用控制网。
12. 当前已设计并写入 `docs/PCNet_DEV_DESIGN_CURRENT.md`：分层架构、数据流、轻量记录、字段宽度、状态/质量字段、模型边界、质量输出草案和性能边界。

## 恢复提示

恢复会话后先读取：

1. `PCNet_DESIGN_CONTEXT.md`
2. `docs/PCNet_DEV_DESIGN_CURRENT.md`
3. 本 checkpoint 文件

然后从“时间归一化与缺失时间策略”继续，一次只问一个关键问题。不要直接进入实现或正式 SPEC，除非用户已经确认剩余设计并要求继续。
