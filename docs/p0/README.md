# PCNet P0 环境核验

本目录记录 ISIS 10 环境的只读核验。报告由 `pcnet_p0_probe` 生成，不能把未验证的 API、单位或 JIGSAW 约束语义写成 PCNet 接口承诺。

## 当前环境

- Environment: `/home/gengxun/miniconda3/envs/asp370`
- Build: CMake + C++17
- ISIS library: `lib/libisis10.0.0.so`
- Verified header probes: `isis/ControlNet.h`, `isis/PushFrameCameraDetectorMap.h`
- JIGSAW: not asserted by this probe

## Reproduce

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/home/gengxun/miniconda3/envs/asp370
cmake --build build --target run_p0_probe
python3 tools/p0_probe_report.py build/p0_probe.json
ctest --test-dir build -R p0_probe --output-on-failure
```

The JSON report records environment and library paths, compile-time header availability, timing header availability, and an explicit `failures` array.
