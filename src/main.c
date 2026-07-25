#include "types.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Type Definitions

typedef struct {
  u8 A, X, Y, SP; // Accumulator, Index Registers, Stack Pointer
  u16 PC;         // Program Counter
  u8 P;           // Processor Status
  u64 cycles;     // Cycle Count
} CPU;

typedef enum {
  FLAG_C = 1u << 0,
  FLAG_Z = 1u << 1,
  FLAG_I = 1u << 2,
  FLAG_D = 1u << 3,
  FLAG_B = 1u << 4,
  FLAG_U = 1u << 5,
  FLAG_V = 1u << 6,
  FLAG_N = 1u << 7,
} CPUFlag;

typedef struct {
  u8 memory[1u << 16]; // 65536

  u8 (*read)(void *self, u16 addr);
  void (*write)(void *self, u16 addr, u8 value);

  void *self; // Pointer to it self
} Bus;

// Opcode Handler Trait
typedef void (*OpcodeHandler)(CPU *cpu, Bus *bus, u8 opcode);

// Function Declarations

static void cpu_reset(CPU *cpu, Bus *bus) {
  cpu->A = 0;
  cpu->X = 0;
  cpu->Y = 0;
  cpu->SP = 0xFD; // Stack Pointer starts at 0xFD
  cpu->PC = bus->read(bus->self, 0xFFFC) |
            (bus->read(bus->self, 0xFFFD) << 8); // Reset Vector
  cpu->P = FLAG_I | FLAG_U; // Unused flag and Interrupt Disable flag set
  cpu->cycles = 0;
}

static void cpu_set_flag(CPU *cpu, CPUFlag flag, bool condition) {
  if (condition)
    cpu->P |= flag;
  else
    cpu->P &= ~flag;
}

static bool cpu_get_flag(CPU *cpu, CPUFlag flag) {
  return (cpu->P & flag) != 0;
}

static void cpu_update_zero_and_negative_flags(CPU *cpu, u8 value) {
  cpu_set_flag(cpu, FLAG_Z, value == 0);
  cpu_set_flag(cpu, FLAG_N, (value & 0x80) != 0);
}

static u8 bus_read(void *self, u16 addr) {
  Bus *bus = self;
  return bus->memory[addr];
}

static void bus_write(void *self, u16 addr, u8 value) {
  Bus *bus = self;
  bus->memory[addr] = value;
}

void init_bus(Bus *bus) {
  bus->read = bus_read;
  bus->write = bus_write;
  bus->self = bus;
}

static u8 fetch8(CPU *cpu, Bus *bus) { return bus->read(bus->self, cpu->PC++); }

static u16 fetch16(CPU *cpu, Bus *bus) {
  u16 lo = fetch8(cpu, bus);
  u16 hi = fetch8(cpu, bus);

  // Little Endian
  return lo | (hi << 8);
}

// Instruction Handlers

static void op_illegal(CPU *cpu, Bus *bus, u8 opcode) {
  (void)bus;

  fprintf(stderr, "Illegal/unimplemented opcode: 0x%02X at PC=0x%04X\n",
          (unsigned)opcode, (unsigned)(u16)(cpu->PC - 1));

  abort();
}

static void op_nop(CPU *cpu, Bus *bus, u8 opcode) {
  (void)bus;
  (void)opcode;
  cpu->cycles += 2; // NOP takes 2 cycles
}

static void op_lda_imm(CPU *cpu, Bus *bus, u8 opcode) {
  u8 value = fetch8(cpu, bus);
  cpu->A = value;
  cpu_update_zero_and_negative_flags(cpu, cpu->A);
  cpu->cycles += 2; // LDA immediate takes 2 cycles
}

static void op_lda_zp(CPU *cpu, Bus *bus, u8 opcode) {
  u8 addr = fetch8(cpu, bus);
  u8 value = bus->read(bus->self, addr);
  cpu->A = value;
  cpu_update_zero_and_negative_flags(cpu, cpu->A);
  cpu->cycles += 3; // LDA zero page takes 3 cycles
}

static void op_sta_abs(CPU *cpu, Bus *bus, u8 opcode) {
  (void)opcode;

  u16 addr = fetch16(cpu, bus);

  bus->write(bus->self, addr, cpu->A);

  cpu->cycles += 4; // STA absolute takes 4 cycles
}

static void op_jmp_abs(CPU *cpu, Bus *bus, u8 opcode) {
  (void)opcode;

  cpu->PC = fetch16(cpu, bus);

  cpu->cycles += 3; // JMP absolute takes 3 cycles
}

static void op_jmp_ind(CPU *cpu, Bus *bus, u8 opcode) {
  (void)opcode;

  u16 addr = fetch16(cpu, bus);

  // Emulate the 6502 bug where the high byte does not wrap correctly
  u16 lo = bus->read(bus->self, addr);
  u16 hi = bus->read(bus->self, (addr & 0xFF00) | ((addr + 1) & 0x00FF));

  cpu->PC = lo | (hi << 8);

  cpu->cycles += 5; // JMP indirect takes 5 cycles
}

static void op_brk(CPU *cpu, Bus *bus, u8 opcode) {
  (void)opcode;

  // Push PC and P onto the stack
  bus->write(bus->self, 0x0100 + cpu->SP--, (cpu->PC >> 8) & 0xFF);
  bus->write(bus->self, 0x0100 + cpu->SP--, cpu->PC & 0xFF);
  bus->write(bus->self, 0x0100 + cpu->SP--, cpu->P | FLAG_B);

  // Set the interrupt disable flag
  cpu_set_flag(cpu, FLAG_I, true);

  // Load the interrupt vector
  u16 lo = bus->read(bus->self, 0xFFFE);
  u16 hi = bus->read(bus->self, 0xFFFF);
  cpu->PC = lo | (hi << 8);

  cpu->cycles += 7; // BRK takes 7 cycles
}

static OpcodeHandler const opcode_table[256] = {
    [0xEA] = op_nop,     [0xA5] = op_lda_zp,  [0xA9] = op_lda_imm,
    [0x8D] = op_sta_abs, [0x4C] = op_jmp_abs, [0x6C] = op_jmp_ind,
    [0x00] = op_brk,
    // other opcodes are initialized to null
};

void cpu_step(CPU *cpu, Bus *bus) {
  u8 opcode = fetch8(cpu, bus);

  OpcodeHandler handler = opcode_table[opcode];

  if (handler) {
    handler(cpu, bus, opcode);
  } else {
    op_illegal(cpu, bus, opcode);
  }
}

// Load Program into Memory

bool load_program(Bus *bus, u16 start_addr, const u8 *program,
                  usize program_size) {
  if (!bus || !bus->write || !program) {
    return false;
  }

  if (program_size > 0x10000u) {
    return false;
  }

  if ((uint32_t)start_addr + (uint32_t)program_size > 0x10000u) {
    return false;
  }

  for (usize i = 0; i < program_size; ++i) {
    u16 addr = (u16)((uint32_t)start_addr + (uint32_t)i);
    bus->write(bus->self, addr, program[i]);
  }

  return true;
}

bool load_program_from_file(Bus *bus, u16 start_addr, const char *filename) {
  if (!bus || !bus->write || !filename) {
    return false;
  }

  FILE *file = fopen(filename, "rb");

  if (!file) {
    perror("Failed to open file");
    return false;
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    perror("Failed to seek file");
    fclose(file);
    return false;
  }

  long file_size = ftell(file);

  if (file_size < 0) {
    perror("Failed to get file size");
    fclose(file);
    return false;
  }

  rewind(file);

  if (file_size == 0) {
    fprintf(stderr, "File is empty\n");
    fclose(file);
    return false;
  }

  if (file_size > 0x10000L) {
    fprintf(stderr, "File is too large for 64 KB address space\n");
    fclose(file);
    return false;
  }

  u8 *buffer = malloc((size_t)file_size);

  if (!buffer) {
    perror("Failed to allocate memory for program");
    fclose(file);
    return false;
  }

  size_t read_count = fread(buffer, 1, (size_t)file_size, file);

  if (read_count != (size_t)file_size) {
    fprintf(stderr, "Failed to read complete file\n");
    free(buffer);
    fclose(file);
    return false;
  }

  fclose(file);

  bool ok = load_program(bus, start_addr, buffer, (usize)file_size);

  free(buffer);

  return ok;
}

// CPU Cycle:
// fetch opcode
// decode opcode
// execute instruction
// update flags
// add cycles
// repeat

int main(void) {
  static Bus bus;
  CPU cpu;

  init_bus(&bus);

  /*
      0x0200: A9 42       LDA #$42
      0x0202: 8D 00 03    STA $0300
      0x0205: 4C 05 02    JMP $FFEF
  */

  static const u8 program[] = {
      0xA9, 0x42,       // LDA #$42
      0x8D, 0x00, 0x03, // STA $0300
      0x4C, 0xEF, 0xFF  // JMP $FFEF
  };

  load_program(&bus, 0x0200, program, (usize)sizeof(program));

  static const u8 reset_vector[] = {0x00, 0x02};

  load_program(&bus, 0xFFFC, reset_vector, (usize)sizeof(reset_vector));

  cpu_reset(&cpu, &bus);

  while (cpu.PC != 0xFFEF) {
    cpu_step(&cpu, &bus);
  }

  printf("A = 0x%02X\n", (unsigned)cpu.A);
  printf("memory[0x0300] = 0x%02X\n", (unsigned)bus.read(bus.self, 0x0300));
  printf("PC = 0x%04X\n", (unsigned)cpu.PC);
  printf("cycles = %llu\n", (unsigned long long)cpu.cycles);

  return 0;
}
