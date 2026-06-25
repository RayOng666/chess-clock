# 棋钟 (Chess Clock)

极简跨平台棋钟桌面程序，基于 C++17 + FLTK 1.4.x 静态编译。

## 功能

- **双方独立倒计时**：白方/黑方各自计时，最后 10 秒显示 0.1 秒精度
- **三种计时模式**：
  - 绝对限时 (Sudden Death)：无补偿，先归零者负
  - 加秒制 (Fischer)：每次切换自动加秒
  - 延迟制 (Bronstein)：切换后延迟一段时间再扣减
- **快捷键**：空格切换/启动、P 暂停/继续、R 重置
- **鼠标点击**：点击左半区启动黑方计时，点击右半区启动白方计时
- **预设方案**：1+0, 3+0, 5+0, 3+2, 5+3, 10+0
- **自定义设置**：自由设置主时间和增量秒数
- **窗口置顶**：一键切换
- **声音控制**：超时蜂鸣报警、低时间预警，可静音
- **配置持久化**：自动保存/恢复设置、窗口位置

## 编译

### 前置条件

- CMake 3.20+
- C++17 编译器（MSVC 2019+ / GCC 9+ / Clang 10+）

### 构建步骤

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

FLTK 会通过 CMake FetchContent 自动下载编译，无需手动安装。

### Windows (MSVC)

```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

生成的 `chess_clock.exe` 在 `build/Release/` 目录下。

## 使用说明

| 操作 | 方式 |
|------|------|
| 启动计时 | 点击任一半区 / 空格键 |
| 切换计时方 | 点击对方半区 / 空格键 |
| 暂停/继续 | P 键 / 开始按钮 |
| 重置 | R 键 / 重置按钮 |
| 选择预设 | 预设按钮弹出菜单 |
| 自定义时间 | 设置按钮打开对话框 |
| 窗口置顶 | 置顶按钮切换 |
| 静音 | 声音按钮切换 |

## 配置文件

`clock.conf` 位于可执行文件同级目录，纯文本键值对格式：

```
main_time=300
increment=0
mode=sudden_death
window_x=100
window_y=100
always_on_top=0
muted=0
```

## 项目结构

```
clock/
├── CMakeLists.txt          # 构建配置（FetchContent 自动拉取 FLTK）
├── readme.md               # 本文件
├── 棋钟桌面程序 PRD.md      # 产品需求文档
└── src/
    ├── types.h             # 枚举、常量、系统蜂鸣
    ├── player_timer.h      # 单方计时器（header-only）
    ├── prefs.h             # 配置管理声明
    ├── prefs.cpp           # 配置读写实现
    ├── settings_dialog.h   # 设置对话框声明
    ├── settings_dialog.cpp # 设置对话框实现
    ├── chess_clock.h       # 主窗口声明
    ├── chess_clock.cpp     # 主窗口实现（UI 绘制、事件、回调）
    └── main.cpp            # 入口
```

## 技术指标

| 指标 | 目标 |
|------|------|
| 可执行文件 | ≤ 800 KB |
| 内存占用 | ≤ 8 MB |
| CPU 占用 | ≤ 0.5% |
| 启动时间 | ≤ 0.2 秒 |
| 刷新频率 | 20 Hz (50ms) |
| 时间精度 | steady_clock，误差 ≤ 0.01 秒 |
