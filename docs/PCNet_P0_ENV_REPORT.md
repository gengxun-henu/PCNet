# PCNet P0 环境核验报告

日期：2026-10-13
工作树：`pcnet-p0`
环境：`/home/gengxun/miniconda3/envs/asp370`

## 已核验事实

- ISIS 头文件目录存在：`$ENV/include/isis`。
- 关键头文件存在：`ControlNet.h`、`ControlPoint.h`、`ControlMeasure.h`。
- ISIS 动态库存在：`$ENV/lib/libisis10.0.0.so`，并提供 `libisis10.so`/`libisis.so` 链接名。
- Qt 6 头文件位于 `$ENV/include/qt6`，核心库为 `libQt6Core.so`。
- 环境中的 GNU C++ 编译器为 GCC 12.4.0，CMake 为 4.4.2。

## 最小编译与运行探针

使用以下 include 和 link 组合，最小程序成功编译并运行：

```text
g++ -std=c++17
  -I$ENV/include/isis
  -I$ENV/include
  -I$ENV/include/qt6
  -I$ENV/include/qt6/QtCore
  probe.cpp
  -L$ENV/lib -Wl,-rpath,$ENV/lib
  -lisis10 -lQt6Core
```

已验证：

1. `Isis::ControlNet` 默认构造成功；
2. `GetNumPoints()` 和 `GetNumMeasures()` 可调用；
3. 空 `ControlNet` 可通过 `Write(path, false)` 写出；
4. 写出的网络可通过 `ControlNet(path)` 重新读取；
5. 往返后的点数和观测数均为零，进程返回码为 0。

## 已确认的 API 线索

- `ControlNet` 提供 `ControlNet(QString)`、`ReadControl(QString)` 和 `Write(QString, bool)`；
- `ControlNet` 提供点数、观测数、有效观测数和忽略观测数统计；
- `ControlMeasure` 提供 sample/line、SerialNumber、忽略状态、sample/line residual、residual magnitude 的读写接口；
- `ControlPoint` 提供点 ID、观测访问、忽略状态和有效观测数接口。
- `Camera` 提供 `exposureDuration(sample, line, band)`；
- `LineScanCameraDetectorMap` 提供 `StartTime()`、`LineRate()` 和逐像素/逐线 `exposureDuration(...)`；
- `VariableLineScanCameraDetectorMap` 提供分段 `LineRateChange`，支持按扫描线变化的起始时间、扫描速率和曝光时长；
- ISIS 已有 `HrscCamera`（继承 `LineScanCamera`）和 `LroNarrowAngleCamera`，说明通用适配器可以从相机模型抽象出固定/可变线速时间，而不把 HRSC 或 NAC 字段写入核心库。

## 尚未核验

- 填充真实点和观测后的完整 ControlNet 往返语义；
- ISIS 10 中从相机/影像元数据获取通用 `ObservationTiming` 的具体接口；
- NAC、HRSC 以及其他传感器的时间字段映射；
- CMake 导出目标和正式安装包配置；当前探针使用显式 include/link 路径；
- JIGSAW 内部模型、单位和先验语义。它们不进入 PCNet 核心接口。
