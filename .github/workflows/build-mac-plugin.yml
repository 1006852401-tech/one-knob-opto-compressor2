name: Build & Package macOS AU and VST3 Plugins

on:
  push:
    branches: [ "main", "master" ]
    tags: [ "v*" ]
  pull_request:
    branches: [ "main", "master" ]
  workflow_dispatch:

jobs:
  build-macos:
    name: Build Universal macOS AU & VST3
    runs-on: macos-14 # Native Apple Silicon M-series runner on GitHub

    steps:
      - name: Checkout Code
        uses: actions/checkout@v4
        with:
          submodules: recursive

      - name: Setup CMake
        uses: jwlawson/actions-setup-cmake@v2
        with:
          cmake-version: '3.28.x'

      - name: Install Ninja Build Tool
        run: brew install ninja

      - name: "Configure CMake (Universal Binary arm64 + x86_64, macOS 10.9+)"
        run: |
          cmake -B build -G Ninja \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
            -DCMAKE_OSX_DEPLOYMENT_TARGET="10.9"

      - name: Build AU, VST3 and Standalone
        run: |
          cmake --build build --config Release --parallel

      - name: Verify Universal Architecture with lipo
        run: |
          echo "=== AU Component Architecture ==="
          lipo -info build/OneKnobOpto_artefacts/Release/AU/OneKnobOpto.component/Contents/MacOS/OneKnobOpto || true
          echo "=== VST3 Architecture ==="
          lipo -info build/OneKnobOpto_artefacts/Release/VST3/OneKnobOpto.vst3/Contents/MacOS/OneKnobOpto || true

      - name: Package Plugins for Distribution
        run: |
          mkdir -p dist/OneKnobOpto-macOS-Universal/AU
          mkdir -p dist/OneKnobOpto-macOS-Universal/VST3
          cp -R build/OneKnobOpto_artefacts/Release/AU/OneKnobOpto.component dist/OneKnobOpto-macOS-Universal/AU/
          cp -R build/OneKnobOpto_artefacts/Release/VST3/OneKnobOpto.vst3 dist/OneKnobOpto-macOS-Universal/VST3/
          cp install_mac_plugins.sh dist/OneKnobOpto-macOS-Universal/ || true
          cp README.md dist/OneKnobOpto-macOS-Universal/ || true
          cd dist
          zip -r ../OneKnobOpto-macOS-Universal.zip OneKnobOpto-macOS-Universal/

      - name: Upload Build Artifact
        uses: actions/upload-artifact@v4
        with:
          name: OneKnobOpto-macOS-Universal
          path: OneKnobOpto-macOS-Universal.zip

      - name: Create GitHub Release (Triggered on v* Tag)
        if: startsWith(github.ref, 'refs/tags/v')
        uses: softprops/action-gh-release@v2
        with:
          files: OneKnobOpto-macOS-Universal.zip
          title: Release ${{ github.ref_name }} - Mac AU & VST3 Universal Plugins
          draft: false
          prerelease: false
          generate_release_notes: true
