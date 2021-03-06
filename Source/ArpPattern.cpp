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

#include <map>
#include "ArpPattern.h"
#include "exception/ArpIntegrityException.h"

const Identifier ArpPattern::TREEID_PATTERN = Identifier("pattern"); // NOLINT
const Identifier ArpPattern::TREEID_TIMEBASE = Identifier("timebase"); // NOLINT
const Identifier ArpPattern::TREEID_LOOP_LENGTH = Identifier("loopLength"); // NOLINT
const Identifier ArpPattern::TREEID_NOTES = Identifier("notes"); // NOLINT

ArpPattern::ArpPattern(int timebase) {
    this->timebase = timebase;
    this->loopLength = timebase;
    this->notes = std::vector<ArpNote>();
}

ArpPattern::~ArpPattern() = default;


int ArpPattern::getTimebase() {
    return this->timebase;
}

std::vector<ArpNote> &ArpPattern::getNotes() {
    return this->notes;
}


ArpBuiltEvents ArpPattern::buildEvents() {
    std::map<int64, ArpBuiltEvents::Event> eventMap;
    ArpBuiltEvents result;

    result.timebase = this->timebase;
    result.loopLength = this->loopLength;

    for (unsigned long i = 0; i < this->notes.size(); i++) {
        auto &note = this->notes[i];

        auto dataIndex = result.data.size();
        result.data.push_back(ArpBuiltEvents::EventNoteData::of(note.data, i));

        int64 onTime = note.startPoint % loopLength;
        ArpBuiltEvents::Event &onEvent = eventMap[onTime];
        onEvent.time = onTime;
        onEvent.ons.insert(dataIndex);

        int64 offTime = note.endPoint % loopLength;
        ArpBuiltEvents::Event &offEvent = eventMap[offTime];
        offEvent.time = offTime;
        offEvent.offs.insert(dataIndex);

        ArpBuiltEvents::Event &totalOffEvent = eventMap[0];
        totalOffEvent.time = 0;
        totalOffEvent.offs.insert(dataIndex);
    }

    for (std::pair<int64, ArpBuiltEvents::Event> pair : eventMap) {
        result.events.push_back(pair.second);
    }

    return result;
}

ValueTree ArpPattern::toValueTree() {
    ValueTree result = ValueTree(TREEID_PATTERN);

    result.setProperty(TREEID_TIMEBASE, this->timebase, nullptr);
    result.setProperty(TREEID_LOOP_LENGTH, this->loopLength, nullptr);

    ValueTree noteTree = result.getOrCreateChildWithName(TREEID_NOTES, nullptr);
    for (ArpNote note : this->notes) {
        noteTree.appendChild(note.toValueTree(), nullptr);
    }

    return result;
}


ArpPattern ArpPattern::fromValueTree(ValueTree &tree) {
    if (!tree.isValid() || !tree.hasType(TREEID_PATTERN)) {
        throw std::invalid_argument("Input tree must be valid and of the correct type!");
    }

    int timebase = DEFAULT_TIMEBASE;
    if (tree.hasProperty(TREEID_TIMEBASE)) {
        timebase = tree.getProperty(TREEID_TIMEBASE);
    }

    ArpPattern result = ArpPattern(timebase);

    if (tree.hasProperty(TREEID_LOOP_LENGTH)) {
        result.loopLength = tree.getProperty(TREEID_LOOP_LENGTH);
    }

    ValueTree notesTree = tree.getChildWithName(TREEID_NOTES);
    if (notesTree.isValid()) {
        for (int i = 0; i < notesTree.getNumChildren(); i++) {
            ValueTree noteTree = notesTree.getChild(i);
            result.notes.push_back(ArpNote::fromValueTree(noteTree));
        }
    }

    return result;
}
