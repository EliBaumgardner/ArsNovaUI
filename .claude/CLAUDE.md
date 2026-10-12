# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Is

ArsNovaUI is the interface layer shared by the Ars Nova tools (SequenceTree first, then the audio concatenator): `Theme` and `CustomLookAndFeel`, which draw every icon, node shape, arrow, stroke symbol, menu and scrollbar, plus the generic components drawn with them — icon buttons, menus, popups, value editors and their formats, bars and the zoom/pan viewport. `README.md` lists them.

It was extracted from SequenceTree's `Source/UI`, with each file's history carried over. The plan it was built from is `GenericUILibrary.md` under `.claude/context/Proposals/` in the SequenceTree repository.

## Boundary Rules

- The library includes nothing outside itself and JUCE (`juce_gui_basics`, `juce_gui_extra`). A component that reaches for a consumer's type — a canvas, a document model, an audio type — does not belong here.
- Drawing functions take JUCE types or the plain structs in `Theme/Visuals.h` (`NodeVisual`, `ArrowVisual`, `ArrowTrail`, `CanvasGrid`, `SymbolStroke`, `SymbolMark`), never a component from a consumer. The consumer decides what a thing is and fills the struct; the library decides how it looks.
- Names say what is drawn, never what a consumer uses it for: `drawForkIcon`, `drawSquareNode`, `hasOuterRing`, `errorColour`.
- Saved-state keys the settings components write live in `InterfaceIdentifiers`. Their strings are in consumers' saved state, so they never change.
- `IconButton::Icon` is a pointer to a `CustomLookAndFeel` member, so a consumer's own icons go through `IconButton::painter`, or into the library.

## Build

The library is a CMake `INTERFACE` target: its `.cpp` files compile inside the project that links it, under that project's JUCE settings. It is never a static library, and never a JUCE module (a unity build would hide each file from clangd and the `refactor.*` commands). Every `.cpp` is listed explicitly in `CMakeLists.txt` — there is no glob — and assets are compiled in as `ArsNovaUIFonts` and `ArsNovaUIIcons`, never into JUCE's default `BinaryData` namespace.

When the library is the top-level project it also adds its JUCE submodule (pinned to SequenceTree's JUCE commit) and the gallery app, `ArsNovaUIGallery`. A consumer adds the library after its own JUCE and gets neither.

```bash
git submodule update --init
cmake -B cmake-build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_MAKE_PROGRAM=/Applications/CLion.app/Contents/bin/ninja/mac/aarch64/ninja
cmake --build cmake-build-debug --target ArsNovaUIGallery
```

`cmake-build-debug` is the only build tree. Never configure another one.

## Verifying a Change

The gallery shows every icon in its normal, hovered, selected and disabled states, a value editor per format, an item selector, a colour selector, a context menu, a bar frosted over a `DynamicPort` with a dot grid, every node shape and state, arrows in every shaft and head style with trails and a label, and stroke symbols. Run it, or snapshot it to a PNG:

```bash
"cmake-build-debug/ArsNovaUIGallery_artefacts/Debug/ArsNovaUI Gallery.app/Contents/MacOS/ArsNovaUI Gallery" --snapshot /private/tmp/gallery.png
```

A new component or drawing function gets a place in the gallery. A change the library's consumers depend on is then built and checked in each consumer as well, since both build from this working tree.

## Rules

The research-suite plugin injects the Key Design Rules at the start of every session, and they bind here as in SequenceTree. SequenceTree's Project Design Rules that concern UI code bind here too, unchanged:

- A class's `.cpp` defines its member functions in exactly the order the header declares them. `refactor.order` puts a file into header order.
- A header keeps one `public:` and one `private:` section, each reading constructors and destructor, `paint` and `resized`, functions grouped by return type with every `(` in a group aligned, nested types, then data members.
- Locals are declared together at a function's top, and statements are grouped into series of one operation with a blank line around each group.
- A constructor's member initializer list holds only short, one-line initializers; anything verbose is assigned in the body.
- A call's arguments stay on the call's line unless one of them is itself large.
- Every struct is declared in a header, never in a `.cpp`.
- `CustomLookAndFeel` draws each icon in the function that paints it; a shared glyph helper exists only once more than two draw functions call it.
- A component takes each dependency as its own constructor parameter, by reference. It never calls `CustomLookAndFeel::get` before it has a parent unless it set its look-and-feel explicitly — `get` is a `static_cast`, so every consumer's top-level window sets the library's `CustomLookAndFeel`, or a subclass of it.

Refactoring settings are in `.claude/refactor.toml`; extracting, inlining, renaming and multi-site rewrites go through the `refactor.*` commands.

## Commits

- **No attribution.** A commit message never carries a `Co-Authored-By` line, a "Generated with" line, or any other line naming Claude, Anthropic or a model.
- **Short and sweet.** One line, lower case, in the style of the existing history, with no body.
- **Stage only the change's files.**
