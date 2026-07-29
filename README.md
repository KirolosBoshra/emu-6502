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

Assemble `tests/test_prog.asm` with `ca65`/`ld65`, run the emulator, and verify results at the expected memory addresses.

## API

```c
load_program(&bus, 0x0200, program, sizeof(program));

load_program_from_file(&bus, 0x0200, "program.bin");
```

## Status

72/151 official opcodes implemented:

- ORA, AND, EOR, ASL — all addressing modes
- ADC, SBC — immediate, zero-page, absolute
- LDA, STA, LDX, STX, LDY, STY — common modes
- JMP (absolute, indirect), BRK, NOP
- PHA, PHP, PLA, PLP
- CMP, CPX, CPY — immediate
- INC, DEC — zero-page
- INX, DEX, INY, DEY
- CLC, SEC, CLD, SED, CLI, SEI, CLV
- BIT — zero-page, absolute
