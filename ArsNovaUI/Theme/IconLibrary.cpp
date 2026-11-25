#include "IconLibrary.h"
#include "ArsNovaUIIcons.h"

#include <juce_gui_extra/juce_gui_extra.h>

namespace ArsNovaUI
{

static juce::Path loadGlyph(const char* svgData, int svgSize)
{
    const std::unique_ptr<juce::Drawable> svg = juce::Drawable::createFromImageData(svgData, static_cast<size_t>(svgSize));

    juce::Path glyph;

    for (juce::Component* shape : svg->getChildren()) {
        glyph.addPath(static_cast<juce::DrawablePath*>(shape)->getPath());
    }

    return glyph;
}

static Icon makeSelectableOutline(const juce::Path& glyph)
{
    Icon icon;

    icon.layers         = { { glyph, GlyphPaint::Stroked } };
    icon.selectedLayers = { { glyph, GlyphPaint::Tinted }, { glyph, GlyphPaint::Stroked } };

    return icon;
}

IconLibrary::IconLibrary()
{
    const juce::Path circleGlyph        = loadGlyph(ArsNovaUIIcons::Circle_svg, ArsNovaUIIcons::Circle_svgSize);
    const juce::Path hostNodeGlyph      = loadGlyph(ArsNovaUIIcons::CircledH_svg, ArsNovaUIIcons::CircledH_svgSize);
    const juce::Path roundedSquareGlyph = loadGlyph(ArsNovaUIIcons::RoundedSquare_svg, ArsNovaUIIcons::RoundedSquare_svgSize);
    const juce::Path triangleGlyph      = loadGlyph(ArsNovaUIIcons::Triangle_svg, ArsNovaUIIcons::Triangle_svgSize);
    const juce::Path brushTipGlyph      = loadGlyph(ArsNovaUIIcons::BrushTip_svg, ArsNovaUIIcons::BrushTip_svgSize);

    add          .layers = { { loadGlyph(ArsNovaUIIcons::Add_svg, ArsNovaUIIcons::Add_svgSize), GlyphPaint::Stroked } };
    arrow        .layers = { { loadGlyph(ArsNovaUIIcons::Arrow_svg, ArsNovaUIIcons::Arrow_svgSize), GlyphPaint::Stroked } };
    chevronDown  .layers = { { loadGlyph(ArsNovaUIIcons::ChevronDown_svg, ArsNovaUIIcons::ChevronDown_svgSize), GlyphPaint::Stroked } };
    chevronLeft  .layers = { { loadGlyph(ArsNovaUIIcons::ChevronLeft_svg, ArsNovaUIIcons::ChevronLeft_svgSize), GlyphPaint::Stroked } };
    chevronRight .layers = { { loadGlyph(ArsNovaUIIcons::ChevronRight_svg, ArsNovaUIIcons::ChevronRight_svgSize), GlyphPaint::Stroked } };
    chevronUp    .layers = { { loadGlyph(ArsNovaUIIcons::ChevronUp_svg, ArsNovaUIIcons::ChevronUp_svgSize), GlyphPaint::Stroked } };
    close        .layers = { { loadGlyph(ArsNovaUIIcons::Close_svg, ArsNovaUIIcons::Close_svgSize), GlyphPaint::Stroked } };
    code         .layers = { { loadGlyph(ArsNovaUIIcons::Code_svg, ArsNovaUIIcons::Code_svgSize), GlyphPaint::Stroked } };
    eraser       .layers = { { loadGlyph(ArsNovaUIIcons::Eraser_svg, ArsNovaUIIcons::Eraser_svgSize), GlyphPaint::Stroked } };
    eye          .layers = { { loadGlyph(ArsNovaUIIcons::Eye_svg, ArsNovaUIIcons::Eye_svgSize), GlyphPaint::Stroked } };
    fork         .layers = { { loadGlyph(ArsNovaUIIcons::Fork_svg, ArsNovaUIIcons::Fork_svgSize), GlyphPaint::Stroked } };
    import       .layers = { { loadGlyph(ArsNovaUIIcons::Import_svg, ArsNovaUIIcons::Import_svgSize), GlyphPaint::Stroked } };
    link         .layers = { { loadGlyph(ArsNovaUIIcons::Link_svg, ArsNovaUIIcons::Link_svgSize), GlyphPaint::Stroked } };
    metronome    .layers = { { loadGlyph(ArsNovaUIIcons::Metronome_svg, ArsNovaUIIcons::Metronome_svgSize), GlyphPaint::Stroked } };
    more         .layers = { { loadGlyph(ArsNovaUIIcons::More_svg, ArsNovaUIIcons::More_svgSize), GlyphPaint::Stroked } };
    outline      .layers = { { loadGlyph(ArsNovaUIIcons::Outline_svg, ArsNovaUIIcons::Outline_svgSize), GlyphPaint::Stroked } };
    pencil       .layers = { { loadGlyph(ArsNovaUIIcons::Pencil_svg, ArsNovaUIIcons::Pencil_svgSize), GlyphPaint::Stroked } };
    redo         .layers = { { loadGlyph(ArsNovaUIIcons::Redo_svg, ArsNovaUIIcons::Redo_svgSize), GlyphPaint::Stroked } };
    reload       .layers = { { loadGlyph(ArsNovaUIIcons::Reload_svg, ArsNovaUIIcons::Reload_svgSize), GlyphPaint::Stroked } };
    remove       .layers = { { loadGlyph(ArsNovaUIIcons::Remove_svg, ArsNovaUIIcons::Remove_svgSize), GlyphPaint::Stroked } };
    route        .layers = { { loadGlyph(ArsNovaUIIcons::Route_svg, ArsNovaUIIcons::Route_svgSize), GlyphPaint::Stroked } };
    save         .layers = { { loadGlyph(ArsNovaUIIcons::Save_svg, ArsNovaUIIcons::Save_svgSize), GlyphPaint::Stroked } };
    search       .layers = { { loadGlyph(ArsNovaUIIcons::Search_svg, ArsNovaUIIcons::Search_svgSize), GlyphPaint::Stroked } };
    settings     .layers = { { loadGlyph(ArsNovaUIIcons::Settings_svg, ArsNovaUIIcons::Settings_svgSize), GlyphPaint::Stroked } };
    sidebar      .layers = { { loadGlyph(ArsNovaUIIcons::Sidebar_svg, ArsNovaUIIcons::Sidebar_svgSize), GlyphPaint::Stroked } };
    terminal     .layers = { { loadGlyph(ArsNovaUIIcons::Terminal_svg, ArsNovaUIIcons::Terminal_svgSize), GlyphPaint::Stroked } };
    undo         .layers ={ { loadGlyph(ArsNovaUIIcons::Undo_svg, ArsNovaUIIcons::Undo_svgSize), GlyphPaint::Stroked } };
    upload       .layers = { { loadGlyph(ArsNovaUIIcons::Upload_svg, ArsNovaUIIcons::Upload_svgSize), GlyphPaint::Stroked } };
    wave         .layers = { { loadGlyph(ArsNovaUIIcons::Wave_svg, ArsNovaUIIcons::Wave_svgSize), GlyphPaint::Stroked } };

    ringDot.layers = { { loadGlyph(ArsNovaUIIcons::Ring_svg, ArsNovaUIIcons::Ring_svgSize), GlyphPaint::Stroked },
                       { loadGlyph(ArsNovaUIIcons::Dot_svg, ArsNovaUIIcons::Dot_svgSize), GlyphPaint::Filled } };

    skipBack.layers = { { loadGlyph(ArsNovaUIIcons::SkipBar_svg, ArsNovaUIIcons::SkipBar_svgSize), GlyphPaint::Stroked },
                        { loadGlyph(ArsNovaUIIcons::SkipTriangle_svg, ArsNovaUIIcons::SkipTriangle_svgSize), GlyphPaint::Filled } };

    quaver.layers = { { loadGlyph(ArsNovaUIIcons::QuaverHead_svg, ArsNovaUIIcons::QuaverHead_svgSize), GlyphPaint::Filled },
                      { loadGlyph(ArsNovaUIIcons::QuaverStem_svg, ArsNovaUIIcons::QuaverStem_svgSize), GlyphPaint::Stroked } };

    dashedBox.layers = { { loadGlyph(ArsNovaUIIcons::DashedBox_svg, ArsNovaUIIcons::DashedBox_svgSize), GlyphPaint::Dashed },
                         { loadGlyph(ArsNovaUIIcons::BoxDots_svg, ArsNovaUIIcons::BoxDots_svgSize), GlyphPaint::Filled },
                         { loadGlyph(ArsNovaUIIcons::BoxDash_svg, ArsNovaUIIcons::BoxDash_svgSize), GlyphPaint::Stroked } };

    brush.layers = { { brushTipGlyph, GlyphPaint::Tinted },
                     { brushTipGlyph, GlyphPaint::Stroked },
                     { loadGlyph(ArsNovaUIIcons::BrushHandle_svg, ArsNovaUIIcons::BrushHandle_svgSize), GlyphPaint::Stroked } };

    play.layers         = { { loadGlyph(ArsNovaUIIcons::Play_svg, ArsNovaUIIcons::Play_svgSize), GlyphPaint::Filled } };
    play.selectedLayers = { { loadGlyph(ArsNovaUIIcons::Pause_svg, ArsNovaUIIcons::Pause_svgSize), GlyphPaint::Filled } };

    marker.layers = { { loadGlyph(ArsNovaUIIcons::Play_svg, ArsNovaUIIcons::Play_svgSize), GlyphPaint::Filled } };
    marker.colour = IconColour::Accent;

    circle        = makeSelectableOutline(circleGlyph);
    hostNode      = makeSelectableOutline(hostNodeGlyph);
    roundedSquare = makeSelectableOutline(roundedSquareGlyph);
    triangle      = makeSelectableOutline(triangleGlyph);
}

const IconLibrary& IconLibrary::get()
{
    static const IconLibrary library;

    return library;
}

}
