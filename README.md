# One-Knob Opto Compressor (Mac AU & VST3)

一款基于经典 **T4 光敏电致发光组件 (Opto / CdS Photocell)** 特性的向后兼容一键母带/音轨光电压缩器插件。

## ✨ 核心特性

1. **一键智能控制 (One-Knob Operation)**
   - 旋转中央唯一巨型旋钮即可同步控制输入驱动、非线性发光阈值与平滑光敏导通电阻。
2. **光电压缩物理特性 (Optical Compression Characteristics)**
   - 具备经典的软拐点 (Soft Knee) 响应，低电平时仅 1.5:1，大动态时平滑升高至 8:1。
   - **双阶程序自适应释放 (Program-Dependent Dual-Stage Release)**：前 50% 能量在 ~60ms 内快速平稳释放，剩余能量根据被压缩深度和持续时间在 0.5s - 2s 缓慢释放，杜绝低频抽动与呼吸失真。
3. **压缩多少自动补偿音量 (Automatic Makeup Gain)**
   - 传统压缩器压缩深时声音会变轻，本插件实时根据瞬时压缩量与平均衰减曲线，**毫秒级动态自动进行增益反向等响补偿**，转动旋钮时音量维持恒定，专注文态与饱满度！
4. **Mac AU & VST3 终极向后兼容 (Universal Binary & Backward Compatibility)**
   - 架构：同时编译 **Apple Silicon (M1/M2/M3/M4 arm64)** 与 **Intel (x86_64)**。
   - 系统兼容：支持从 **macOS 10.9 Mavericks** 到最新 **macOS 15+ Sequoia**。
   - 格式：
     - **AU (AudioUnit .component)**: 原生支持 Logic Pro X, GarageBand, Final Cut Pro。
     - **VST3 (.vst3)**: 原生支持 Ableton Live, Steinberg Cubase, Nuendo, Studio One, Reaper, FL Studio Mac。

---

## 🛠️ Mac 编译与安装步骤 (Build & Install)

### 前置准备
只需要一台 Mac 并安装有命令行工具：
```bash
xcode-select --install
brew install cmake
```

### 步骤 1：一键编译 Universal 插件包
解压工程包后，在终端进入项目根目录运行：
```bash
chmod +x build_mac_universal.sh install_mac_plugins.sh
./build_mac_universal.sh
```
CMake 会自动拉取 JUCE 框架，并自动生成支持 arm64 与 x86_64 双架构的通用二进制文件。

### 步骤 2：一键安装到本机系统
```bash
./install_mac_plugins.sh
```
脚本会自动将编译好的插件拷贝到系统标准目录：
- AU: `~/Library/Audio/Plug-Ins/Components/OneKnobOpto.component`
- VST3: `~/Library/Audio/Plug-Ins/VST3/OneKnobOpto.vst3`
并重置 macOS AudioComponentRegistrar，让 Logic Pro 与 Ableton 立即可用。

---

## 🎹 适用场景
- **人声平滑平整 (Vocal Leveling)**: 消除人声动态跳跃，保持在混音前沿。
- **木吉他与琴键 (Acoustic & Keys)**: 增加延音与木质温暖感。
- **贝斯平稳 (Bass Tracking)**: 80Hz 侧链高通滤除超低频引起的误触发，凝聚中低频冲击。
- **总线母带黏合 (Mix Bus Glue)**: 轻微转动 15-30% 即可获得模拟级的光电温润感。
