//
// This file is part of LibreArp
//
// LibreArp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LibreArp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see https://librearp.gitlab.io/license/.
//

#include "BeatBar.h"
#include "PatternEditorView.h"

const Colour BACKGROUND_COLOUR = Colour(42, 40, 34);
const Colour BOTTOM_LINE_COLOUR = Colour(0, 0, 0);
const Colour BEAT_LINE_COLOUR = Colour(107, 104, 94);
const Colour BEAT_NUMBER_COLOUR = Colour(107, 104, 94);
const Colour LOOP_LINE_COLOUR = Colour(155, 36, 36);
const Colour LOOP_TEXT_COLOUR = Colour((uint8) 155, 36, 36);

const String LOOP_TEXT = "loop"; // NOLINT

const int TEXT_OFFSET = 4;

BeatBar::BeatBar(LibreArp &p, EditorState &e, PatternEditorView *ec)
        : processor(p), state(e), editorComponent(ec) {

    setSize(1, 1);
}

void BeatBar::paint(Graphics &g) {
    auto pattern = processor.getPattern();
    auto pixelsPerBeat = state.pixelsPerBeat;

    setSize(jmax(editorComponent->getRenderWidth(), getParentWidth()), getParentHeight());

    // Draw background
    g.setColour(BACKGROUND_COLOUR);
    g.fillRect(getLocalBounds());
    g.setColour(BOTTOM_LINE_COLOUR);
    g.drawLine(0, getHeight(), getWidth(), getHeight());

    auto loopLine = static_cast<int>((pattern.loopLength / static_cast<float>(pattern.getTimebase())) * pixelsPerBeat);

    // Draw beat lines
    g.setFont(20);
    int n = 1;
    for (float i = 0; i < getWidth(); i += pixelsPerBeat, n++) {
        g.setColour(BEAT_LINE_COLOUR);
        g.drawLine(i, 0, i, getHeight(), 4);

        g.setColour((i == loopLine) ? LOOP_TEXT_COLOUR : BEAT_NUMBER_COLOUR);
        g.drawText(String(n), static_cast<int>(i) + TEXT_OFFSET, 0, 32, getHeight(), Justification::centredLeft);
    }

    // Draw loop line
    g.setFont(16);
    g.setColour(LOOP_LINE_COLOUR);
    g.drawLine(loopLine, 0, loopLine, getHeight(), 4);

    g.setColour(LOOP_TEXT_COLOUR);
    auto loopTextWidth = g.getCurrentFont().getStringWidth(LOOP_TEXT);
    auto loopLineWithOffset = loopLine - loopTextWidth - TEXT_OFFSET;
    g.drawText(LOOP_TEXT, loopLineWithOffset, 0, loopTextWidth, getHeight(), Justification::centredRight);
}

void BeatBar::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel) {
    if (event.mods.isShiftDown()) {
        editorComponent->zoomPattern(0, wheel.deltaY);
    } else {
        editorComponent->zoomPattern(wheel.deltaY, 0);
    }
}
