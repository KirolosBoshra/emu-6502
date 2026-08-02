#include "emu.h"

// Load Program into Memory

bool load_program(Bus *bus, u16 start_addr, const u8 *program,
                  usize program_size) {
  if (!bus || !bus->write || !program) {
    return false;
  }

  if (program_size > 0x10000u) {
    return false;
  }

  if ((u32)start_addr + (u32)program_size > 0x10000u) {
    return false;
  }

  for (usize i = 0; i < program_size; ++i) {
    u16 addr = (u16)((u32)start_addr + (u32)i);
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

  u8 *buffer = (u8 *)malloc((size_t)file_size);

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

static bool dump_memory(Bus *bus, const char *filename) {
  FILE *file = fopen(filename, "wb");
  if (!file) {
    perror("Failed to open file for dumping memory");
    return false;
  }
  usize written = fwrite(bus->memory, 1, sizeof(bus->memory), file);
  fclose(file);
  if (written != sizeof(bus->memory)) {
    fprintf(stderr, "Failed to write memory dump\n");
    return false;
  }
  return true;
}

i32 main(i32 argc, char **argv) {

  const char *program_file = NULL;
  const char *dump_file = NULL;

  if (argc == 2) {
    program_file = argv[1];
  } else if (argc == 4 && strcmp(argv[2], "-o") == 0) {
    program_file = argv[1];
    dump_file = argv[3];
  } else {
    fprintf(stderr, "Usage: %s <program_file> [-o <dump_file>]\n", argv[0]);
    return 1;
  }

  printf("Loading program: %s\n", program_file);

  static Bus bus;
  CPU cpu;

  init_bus(&bus);

  load_program_from_file(&bus, 0x0000, program_file);

  cpu_reset(&cpu, &bus);

  // Execute the program until BRK is encountered (PC = 0x00) or hit cycle limit
  u64 max_cycles = cpu.cycles + 5000000;
  while (cpu.PC != 0x00 && cpu.cycles < max_cycles) {
    cpu_step(&cpu, &bus);
  }

  // Print CPU state after execution
  printf("A = 0x%02X\n", (unsigned)cpu.A);
  printf("X = 0x%02X\n", (unsigned)cpu.X);
  printf("Y = 0x%02X\n", (unsigned)cpu.Y);
  printf("PC = 0x%04X\n", (unsigned)cpu.PC);
  printf("SP = 0x%02X\n", (unsigned)cpu.SP);
  printf("P = 0x%02X\n", (unsigned)cpu.P);

  printf("cycles = %llu\n", (u64)cpu.cycles);

  if (dump_file) {
    dump_memory(&bus, dump_file);
  }

  return 0;
}
