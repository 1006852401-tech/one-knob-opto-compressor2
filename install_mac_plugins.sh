#!/usr/bin/env bash
# ==============================================================================
# install_mac_plugins.sh
# Installs compiled AU and VST3 into macOS user audio plug-in directories
# ==============================================================================

set -e

BUILD_DIR="build_mac/OneKnobOpto_artefacts/Release"
USER_AU_DIR="$HOME/Library/Audio/Plug-Ins/Components"
USER_VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"

mkdir -p "$USER_AU_DIR"
mkdir -p "$USER_VST3_DIR"

echo "==> Installing AudioUnit (.component) to: $USER_AU_DIR"
if [ -d "$BUILD_DIR/AU/OneKnobOpto.component" ]; then
    rm -rf "$USER_AU_DIR/OneKnobOpto.component"
    cp -R "$BUILD_DIR/AU/OneKnobOpto.component" "$USER_AU_DIR/"
    echo "AudioUnit installed successfully."
else
    echo "Warning: AU component not found in $BUILD_DIR/AU. Please run ./build_mac_universal.sh first."
fi

echo "==> Installing VST3 (.vst3) to: $USER_VST3_DIR"
if [ -d "$BUILD_DIR/VST3/OneKnobOpto.vst3" ]; then
    rm -rf "$USER_VST3_DIR/OneKnobOpto.vst3"
    cp -R "$BUILD_DIR/VST3/OneKnobOpto.vst3" "$USER_VST3_DIR/"
    echo "VST3 installed successfully."
else
    echo "Warning: VST3 bundle not found in $BUILD_DIR/VST3. Please run ./build_mac_universal.sh first."
fi

echo "==> Resetting macOS CoreAudio AU cache for Logic Pro / GarageBand..."
killall -9 AudioComponentRegistrar 2>/dev/null || true

echo "======================================================="
echo " Done! You can now open Logic Pro, Ableton Live, Cubase,"
echo " Studio One, or Reaper and insert 'OneKnobOpto'."
echo "======================================================="
