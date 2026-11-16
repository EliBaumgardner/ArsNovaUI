#pragma once

#include "Icon.h"

/*
    The icons the library draws, one Icon each, built once from the SVG glyphs in Resources/Icons.
    get returns the one library every component reads, so a button can name its icon before it has
    a parent or a look-and-feel (button.icon = &IconLibrary::get().play).

    Adding an icon is the SVG, its line in the library's CMakeLists.txt and one row in the
    constructor.
*/

namespace ArsNovaUI
{

class IconLibrary
{
public:

    IconLibrary();

    static const IconLibrary& get();

    Icon add;
    Icon arrow;
    Icon brush;
    Icon chevronDown;
    Icon chevronRight;
    Icon chevronUp;
    Icon circle;
    Icon close;
    Icon code;
    Icon dashedBox;
    Icon eraser;
    Icon eye;
    Icon fork;
    Icon hostNode;
    Icon import;
    Icon link;
    Icon marker;
    Icon metronome;
    Icon more;
    Icon outline;
    Icon pencil;
    Icon play;
    Icon quaver;
    Icon redo;
    Icon reload;
    Icon remove;
    Icon ringDot;
    Icon roundedSquare;
    Icon route;
    Icon search;
    Icon settings;
    Icon sidebar;
    Icon skipBack;
    Icon triangle;
    Icon undo;
    Icon upload;
    Icon wave;
};

}
