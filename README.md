# emu-6502

A minimal MOS 6502 CPU emulator in C.
I wrote this in order to continue my journey of learning CPU architecture 
I'm hoping to make it a full NES emulator eventually, but for now it's just a CPU emulator.

## Build

```
cmake -B build
cmake --build build
```

## Run

```
./build/emu6502
```

## API examples

Load a byte array into memory:

```c
static const u8 program[] = {
    0xA9, 0x42,       // LDA #$42
    0x8D, 0x00, 0x03, // STA $0300
    0x4C, 0xEF, 0xFF  // JMP $FFEF
};
load_program(&bus, 0x0200, program, sizeof(program));
```

Load a binary file into memory:

```c
load_program_from_file(&bus, 0x0200, "program.bin");
```

## Status

Partial — implements LDA, STA, JMP, BRK, NOP. WIP.
