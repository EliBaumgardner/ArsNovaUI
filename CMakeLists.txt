cmake_minimum_required(VERSION 3.22)

project(ArsNovaUI VERSION 0.1.0)

if(NOT COMMAND juce_add_binary_data)
    message(FATAL_ERROR "ArsNovaUI is added after JUCE by the project that uses it")
endif()

juce_add_binary_data(ArsNovaUIFonts
        HEADER_NAME ArsNovaUIFonts.h
        NAMESPACE ArsNovaUIFonts
        SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Fonts/Geist-Regular.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Fonts/Geist-SemiBold.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Fonts/GeistMono-Regular.ttf
)

juce_add_binary_data(ArsNovaUIIcons
        HEADER_NAME ArsNovaUIIcons.h
        NAMESPACE ArsNovaUIIcons
        SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Add.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Arrow.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/BoxDash.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/BoxDots.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/BrushHandle.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/BrushTip.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/ChevronDown.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/ChevronUp.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Circle.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/CircledH.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Code.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/DashedBox.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Dot.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Eraser.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Eye.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Fork.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Import.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Link.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Metronome.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Pause.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Pencil.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Play.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/QuaverHead.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/QuaverStem.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Redo.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Reload.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Remove.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Ring.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/RoundedSquare.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Route.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Settings.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/SkipBar.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/SkipTriangle.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Triangle.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Undo.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Upload.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Wave.svg
)

add_library(ArsNovaUI INTERFACE)

target_sources(ArsNovaUI INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Theme/CustomLookAndFeel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Theme/CustomLookAndFeelNodes.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Theme/CustomLookAndFeelArrows.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Theme/CustomLookAndFeelButtons.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Theme/CustomLookAndFeelOrnaments.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Theme/CustomLookAndFeelMenus.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Theme/CustomTextCaret.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Theme/InterfaceIdentifiers.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Buttons/IconButton.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Buttons/ButtonPane.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Buttons/ValueSlider.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Menus/ContextMenu.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Menus/ItemSelector.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Menus/ColourSelector.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Menus/SettingsMenu.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/PopupWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/ScaledView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/PanelResizer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/LabelPanel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Editors/ValueEditor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Editors/FileLabel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Editors/FilePage.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Editors/Formats/NumberFormat.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Editors/Formats/TextFormat.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Editors/Formats/DualIntFormat.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Editors/Formats/PitchFormat.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Editors/Formats/GreekLetterFormat.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Bars/Bar.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ArsNovaUI/Canvas/DynamicPort.cpp
)

target_include_directories(ArsNovaUI INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(ArsNovaUI INTERFACE cxx_std_20)
target_link_libraries(ArsNovaUI INTERFACE juce::juce_gui_basics juce::juce_gui_extra ArsNovaUIFonts ArsNovaUIIcons)
