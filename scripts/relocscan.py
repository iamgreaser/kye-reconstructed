#!/usr/bin/env python3

from __future__ import annotations

import struct, sys

IDMAP: dict[int, str] = {
    0x0001001B: "Rectangle",
    0x0001001F: "SetPixel",
    0x00010021: "TextOut",
    0x00010022: "BitBlt",
    0x0001002D: "SelectObject",
    0x00010033: "CreateCompatibleBitmap",
    0x00010034: "CreateCompatibleDC",
    0x00010044: "DeleteDC",
}

def main() -> None:
    with open(sys.argv[1], "rb") as fp:
        fp.seek(0x890B)  # hardcoded start of CODE relocs / end of CODE image octets
        reloc_count, = struct.unpack("<H", fp.read(2))
        for irel in range(reloc_count):
            rtyp, rflg, woffs, rseg, roffs = struct.unpack("<BBHHH", fp.read(8))
            knownid = IDMAP.get(roffs|(rseg<<16), "?")
            #print(f"{irel:04X}: {rtyp:02X} {rflg:02X} {woffs:04X} {rseg:04X}:{roffs:04X} {knownid}")
            print(f"{rtyp:02X} {rflg:02X} {woffs:04X} {rseg:04X}:{roffs:04X} {knownid}")
    return

if __name__ == "__main__":
    main()
