#!/usr/bin/env python3

import sys
import mido

NOTE_32_DURATION = 15


class Note:
    def __init__(self, note: int, time: int, trans: int = 1):
        self.frequency = self.note_to_freq(note, trans)
        self.duration = self.duration_to_32s(time)

    @staticmethod
    def duration_to_32s(time: int) -> int:
        return int(time / NOTE_32_DURATION)

    @staticmethod
    def note_to_freq(note: int, trans: int):
        if note == 0:
            return 0
        a = 440 * (2 ** trans)  # frequency of A (coomon value is 440Hz)
        return round((a / 32) * (2 ** ((note - 9) / 12)), 2)


def midi2beep(filename: str, trans: int = 1) -> None:
    notes = []
    mid = mido.MidiFile(filename)
    for track in mid.tracks:
        for msg in track:
            print(msg.__dict__)
            if msg.type != "note_on" and msg.type != "note_off":
                continue
            #print(msg.__dict__)
            if msg.type == "note_on":
                if int(msg.time) != 0:
                    notes.append(Note(note=0, time=int(msg.time)))
            if msg.type == "note_off":
                notes.append(Note(note=int(msg.note), time=int(msg.time), trans=trans))
    print_notes(notes)


def print_notes(notes: list[Note]) -> None:
    for note in notes:
        pass
        #print(f"{{.frequency = {note.frequency}, .duration_in_32s = {note.duration}}},")


def main():
    if len(sys.argv) < 2:
        raise Exception("Enter filename..")

    trans = int(sys.argv[2]) if len(sys.argv) > 2 else 1

    filename = sys.argv[1]
    midi2beep(filename, trans)


if __name__ == "__main__":
    main()
