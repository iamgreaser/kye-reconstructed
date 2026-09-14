Kye v2.0 source code reconstruction

==============================
What is this?
------------------------------

This is a reconstruction of the Kye v2.0 source code in such a way that it compiles to a byte-exact executable! ...eventually. See the "Status" section to find out what needs work.

It also doubles as a 32-bit Windows port with a few bugs fixed, mostly pertaining to the checked-use-after-free of the main window device context which results in several animations not working and the Kye mouse cursor ghost not being drawn. Also the status bar is the correct height regardless of your OS settings.

No, I will not fix the global tick counter wraparound problem, that is a feature at this point. Wait, you didn't know about that? Leave COUNTERS running for an hour and it'll happen in that timeframe and the diamond at the end will be blocked. Now that you know this, go make a level that abuses it!

==============================
Status
------------------------------

MILESTONE REACHED: The actual *code* is byte-exact! Hooray! There were 2 functions which were quite stubborn but I managed to convince them to behave.

- The app module name is not specified correctly (should be "Kye", ends up being "KYE" and/or "KYE.EXE").
  - Partially solved, but while the module name in one place is "Kye", the best I can do for the other place is "KYE.".
- The resource compiler can't seem to get the correct mismatch of "1 plane 4 bits" vs "4 planes 1 bit".
- Some relocations seem to be in a weird order.

Functionalitywise it should be identical to Kye v2.0 unless:

- the planes vs bits-per-plane mismatch causes a problem, or
- it doesn't like the embedded executable resident name, or
- the 1 byte offset of the resident names table causes a problem, or
- somehow the reordered relocations cause a problem.

==============================
QUICK WARNING
------------------------------

Some of the versions of tools mentioned may not be what I am actually using! I aim to keep things compatible but I'm not exactly going to be in a hurry to update this readme every time I update the software on my computer.

==============================
Common requirements
------------------------------

I use GNU Make 4.4.1 to build everything on a Linux machine.

This is the archive you will want:

MD5:
5895f517e31e000c0183cb654ec9e30e  KYE.ZIP

Which should contain the following game files:

MD5:
19d42a4e3df82ea4e8b6abea42fecb9a  BORDER.KYE
08d61e3d484279590855db31afcdd5d0  CREDITS.TXT
bc8da012f9a11f140e36cf6c6408e21c  DEFAULT.KYE
338c9f555ebc85d069b48ddf0c14e49d  KYEHELP.HLP
a8e24967f120d30700245c535e7a6b9b  README.TXT
234b05055909ece829cd457de172324f  TEMPLATE.KYE

Or alternatively, here's the `unzip -ll` output from said archive:

   Length   Method    Size  Cmpr    Date    Time   CRC-32   Name
  --------  ------  ------- ---- ---------- ----- --------  ----
       688  Defl:X      181  74% 1992-04-30 14:16 fbf46e30  BORDER.KYE
       744  Defl:X      417  44% 1992-05-06 12:24 e2abc626  CREDITS.TXT
     10455  Defl:X     2988  71% 1992-05-05 14:42 b581458d  DEFAULT.KYE
     90112  Defl:X    24809  73% 1992-05-06 14:53 7b42aeff  KYE.EXE
     12067  Defl:X     5190  57% 1992-04-30 14:16 57c364b4  KYEHELP.HLP
       503  Defl:X      295  41% 1992-05-06 12:24 52e1bfa6  README.TXT
       669  Defl:X       71  89% 1992-04-30 14:16 5e7ae1b8  TEMPLATE.KYE
  --------          -------  ---                            -------
    115238            33951  71%                            7 files

==============================
16-bit port
------------------------------

Run:

    make

You will need "Borland C++ & Application Frameworks 3.0" in 5.25in HD format.
There are 12 disks, 1228800 bytes each.
These are the expected checksums:

MD5:
0f608bc27364030c3daf89cf2dd05817  packages/bcp30af/disk01.img
4d696335706648eb255121977671f767  packages/bcp30af/disk02.img
2c60046f9160aa42a84adde27f9287c2  packages/bcp30af/disk03.img
d0b0268b406dac82efcd8b940895737e  packages/bcp30af/disk04.img
d332ca447c7e0e44f9ed172ccccabc42  packages/bcp30af/disk05.img
2d36f9be9045f6395b5058b54cec84b2  packages/bcp30af/disk06.img
b16b5a7b46430cfe433e3d3aea18e65c  packages/bcp30af/disk07.img
b688f40a619e7c06d9d94623e2231c1e  packages/bcp30af/disk08.img
36996359d76449bba78be1e917c9a01c  packages/bcp30af/disk09.img
17e8a19e3f69be511a6b5e0f9a650909  packages/bcp30af/disk10.img
a7df998f524fde0e9b70e5f6c5a2bde1  packages/bcp30af/disk11.img
e2430a261d09b5ce24aac0246f78ed00  packages/bcp30af/disk12.img

To extract the compiler, I use the following:

- GNU dd (coreutils 9.11) for stripping 4-byte headers from split .zip files
- GNU mtools 4.0.49 for extracting files from raw FAT12 floppy disk images
- GNU install (coreutils 9.11) for making directories and copying files
- Info-ZIP UnZip 6.00 for extracting the relevant .zip files
  - WARNING: Borland C++ 3.0 uses the "implode" method, NOT the "deflate" method that everyone uses these days!
    - In other words, BusyBox `unzip` won't cut it.

To actually run the compiler, I use the following:

- DOSBox-X 2026.01.02 SDL2 for running DOS code

For confirming how munted the results are, see `scripts/amidone.sh`, but basically I have ndisasm from the Nasm project (currently using 3.02 - 3.00's disassembler is broken!) and vimdiff from the Vim project (currently using 9.2.849 but I really want to find a version that doesn't time out constantly when trying to do `]p`). Sometimes I recomment the lines and use `hexdump` to output what needs to be compared instead of `ndisasm`, it depends on what I'm trying to force to match.

==============================
32-bit port
------------------------------

Run:

    make out/kye32.exe

I use the following:

- Zig 0.16.0 as a drop-in replacement for `rc`
- MinGW32-W64 2.46.0.20260210 for the `windres` utility
- TinyCC 0.9.28rc 2025-11-04 mob@ab2ce3b1 as a C compiler

I haven't confirmed Windows 95 compatibility yet. Once I start doing that I'll do whatever it takes to make sure it runs on there.

==============================
Licence
------------------------------

Makefile tooling and scripts are licensed under CC0-1.0. I don't care what you do with them, just don't be a dickhead.

.ico and .bmp files have been extracted from Kye v2.0 which is Copyright (C) 1992 Colin Garbutt. I'd like to actually make something that extracts the relevant data so I don't have to keep non-free assets in the repository. However, you can download it for free, so once I've done this, maybe I should just include the relevant .zip file in the repository and extract from there.

I'm not sure what licence I can feasibly slap on the source code reconstruction. So for now, you will ultimately have to be respectful. What I'm saying is don't try to sell this or anything.

