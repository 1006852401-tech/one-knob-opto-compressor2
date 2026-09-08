cmake_minimum_required(VERSION 3.15)
project(OneKnobOpto VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# macOS Backward Compatibility Configuration
if(APPLE)
    # Minimum deployment target: macOS 10.9 Mavericks for maximum backward compatibility
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X version" FORCE)
    
    # Universal Binary: Compile for both Apple Silicon (arm64) and Intel (x86_64)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Target architectures" FORCE)
    
    message(STATUS "Configuring for macOS Universal Binary (arm64 + x86_64)")
    message(STATUS "macOS Deployment Target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
endif()

# Include JUCE (Fetched via FetchContent or local JUCE submodule)
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# Declare JUCE Plugin with AU and VST3 formats
juce_add_plugin(OneKnobOpto
    COMPANY_NAME                "VintageAudioLab"
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    PLUGIN_MANUFACTURER_CODE    "Valb"
    PLUGIN_CODE                 "Okop"
    FORMATS                     AU VST3 Standalone
    PRODUCT_NAME                "OneKnobOpto"
    BUNDLE_ID                   "com.vintageaudiolab.oneknobopto"
    AU_MAIN_TYPE                "aufx" # AudioUnit Effect
)

target_sources(OneKnobOpto PRIVATE
    Source/OptoEngine.h
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
)

target_compile_definitions(OneKnobOpto PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(OneKnobOpto PRIVATE
    juce::juce_audio_utils
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
