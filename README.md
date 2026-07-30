# emu-6502

A minimal MOS 6502 CPU emulator in C.</br> 
I wrote this in order to continue my journey of learning CPU architecture,</br>
I'm hoping to make it a full NES emulator eventually, but for now it's just a CPU emulator.

## Build

```
cmake -B build
cmake --build build
```

## Run

```
emu6502.exe <program.bin> [-o mem_dump.bin]
```

The `-o` flag writes a memory dump after execution.

## Tests

Assembly: `ca65`/`ld65` assembles `tests/test_prog.asm`, run the emulator, and verify results at the expected memory addresses.

C: `cl65 -t none -C tests/test_prog_c.cfg -o tests/test_prog_c.bin tests/test_prog.c`

## API

```c
load_program(&bus, 0x0200, program, sizeof(program));

load_program_from_file(&bus, 0x0200, "program.bin");

step_cpu(&cpu, &bus); // wrap it in a loop
```

## Status

All 151 official 6502 opcodes implemented and tested.
