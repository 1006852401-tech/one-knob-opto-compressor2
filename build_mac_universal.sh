#!/usr/bin/env bash
# ==============================================================================
# build_mac_universal.sh
# Builds Universal Binary (Apple Silicon arm64 + Intel x86_64) AU & VST3 plugins
# Target deployment: macOS 10.9 Mavericks through macOS 15+ Sequoia
# ==============================================================================

set -e

echo "======================================================="
echo " Building One-Knob Opto Compressor for macOS"
echo " Architectures: arm64 (M1/M2/M3/M4) + x86_64 (Intel)"
echo " Minimum macOS: 10.9 (Mavericks)"
echo " Formats: AudioUnit (.component) + VST3 (.vst3)"
echo "======================================================="

# Verify CMake and Xcode CLI tools
if ! command -v cmake &> /dev/null; then
    echo "Error: cmake is required. Please install via: brew install cmake"
    exit 1
fi

BUILD_DIR="build_mac"
mkdir -p "$BUILD_DIR"

echo "==> Configuring CMake with Universal Binary..."
cmake -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET="10.9"

echo "==> Compiling Release targets..."
cmake --build "$BUILD_DIR" --config Release --parallel $(sysctl -n hw.ncpu)

echo "==> Verifying Universal Binary output with 'lipo'..."
AU_BINARY="$BUILD_DIR/OneKnobOpto_artefacts/Release/AU/OneKnobOpto.component/Contents/MacOS/OneKnobOpto"
VST3_BINARY="$BUILD_DIR/OneKnobOpto_artefacts/Release/VST3/OneKnobOpto.vst3/Contents/MacOS/OneKnobOpto"

if [ -f "$AU_BINARY" ]; then
    echo "AU Binary architectures:"
    lipo -info "$AU_BINARY"
fi

if [ -f "$VST3_BINARY" ]; then
    echo "VST3 Binary architectures:"
    lipo -info "$VST3_BINARY"
fi

echo "==> Ad-hoc code signing for local DAW authorization..."
codesign --force --deep --sign - "$BUILD_DIR/OneKnobOpto_artefacts/Release/AU/OneKnobOpto.component" 2>/dev/null || true
codesign --force --deep --sign - "$BUILD_DIR/OneKnobOpto_artefacts/Release/VST3/OneKnobOpto.vst3" 2>/dev/null || true

echo "======================================================="
echo " Build successful!"
echo " AU Plugin:   $BUILD_DIR/OneKnobOpto_artefacts/Release/AU/OneKnobOpto.component"
echo " VST3 Plugin: $BUILD_DIR/OneKnobOpto_artefacts/Release/VST3/OneKnobOpto.vst3"
echo " To install to your Mac, run: ./install_mac_plugins.sh"
echo "======================================================="
