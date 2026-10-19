# PCNet Milestone Registry

每个里程碑完成后必须通过新鲜验证、创建 PR、合并远程 main，并同步本地 main。

# Progress: P0 ISIS 10 / ControlNet / DetectorMap 核验

## Session Log

### Initialized 2026-10-19

- **Status:** complete
- Created the durable milestone plan from the reviewed manifest.

### P0 execution 2026-10-19

- Added a self-contained CTest fixture that generates the probe report before validation.
- Extended `pcnet_p0_probe` with native ControlNet write/reopen verification, ABI pointer width, and explicit runtime camera-context status.
- Updated `docs/validation/P0_REPORT.md` and `docs/p0/p0_probe.json` with reproducible evidence.

## Test Results

| Test | Passed | Failed | Skipped | Status |
|---|---:|---:|---:|---|

## External Prerequisites

| Path | Status | Evidence |
|---|---|---|
| `/home/gengxun/miniconda3/envs/asp370` | verified | ISIS headers/library and native ControlNet round trip |

## Git Evidence

- Branch: `fix/p0-probe-ctest-fixture`
- Worktree: repository root (`.`)
- Commit: `5a8bf6443eb299dcb28e4a81f3e5950fc55267fd`

## 5-Question Reboot Check

| Question | Answer |
|---|---|
| Where am I? | Milestone `milestone-01-p0-isis` is complete |
| Where am I going? | Its declared completion gate |
| What's the goal? | See `task_plan.md` Goal |
| What have I learned? | See `findings.md` |
| What have I done? | Initialized durable state |
| What am I about to do? | Activate `milestone-02-timing-core` after PR merge and local main sync |
