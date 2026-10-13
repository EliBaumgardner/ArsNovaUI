# ArsNovaUI

The UI shared by my tools: one look-and-feel and the components drawn with it. It grew out of my project SequenceTree, after starting a new project I decided to modularize my UI so I can JUCE plugins quicker.

- **Theme and drawing**: `Theme` (palette and metrics) and `CustomLookAndFeel`, which draws every icon, node shape, arrow, stroke symbol, menu and scrollbar. Drawing functions take JUCE types or the plain structs in `Theme/Visuals.h`.
- **Components**: icon buttons and button panes, value sliders, context menus, item and colour selectors, a settings menu, popup windows, panel resizers, frosted or solid bars, and a zoom/pan viewport (`DynamicPort`).
- **Editors**: `ValueEditor`, which binds to a `ValueTree` property and shows it through a `ValueFormat` (numbers, text, pitch names, Greek letters, value pairs), and `FilePage`, a code editor over a `juce::CodeDocument`.
- **Assets**: the Geist fonts and the icon SVGs, compiled in as `ArsNovaUIFonts` and `ArsNovaUIIcons`.

The library depends only on JUCE and includes nothing outside itself.

## Using it

The library is a CMake `INTERFACE` target, so its sources compile inside the project that links it, under that project's JUCE settings. Add it after JUCE:

```cmake
add_subdirectory(JUCE)
add_subdirectory(path/to/ArsNovaUI ${CMAKE_CURRENT_BINARY_DIR}/ArsNovaUI)

target_link_libraries(MyApp PRIVATE ArsNovaUI)
```

Include headers from the repository root, for example `#include <ArsNovaUI/Theme/CustomLookAndFeel.h>`.

The editor (or top-level window) sets one `CustomLookAndFeel` on itself, and every child inherits it, so `CustomLookAndFeel::get(*this)` works in any component once it has a parent. A pop-up or window root sets it explicitly.

Saved-state keys the settings components write live in `InterfaceIdentifiers`.
