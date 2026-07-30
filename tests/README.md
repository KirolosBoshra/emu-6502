| File | Description | Source |
|------|-------------|--------|
| `test_prog.asm` | Assembly opcode test,  exercises all 151 official opcodes | self |
| `test_prog.cfg` | Linker config for asm test | self |
| `test_prog.c` | C end-to-end test, function calls, loops, arithmetic | self |
| `test_prog_c.cfg` | Linker config for C test | self |
| `vectors.asm` | Reset vector stub ($FFFA-$FFFF) for C test | self |
| `6502_functional_test.bin` | Klaus Dormann's 6502 functional test suite | [external](https://github.com/Klaus2m5/6502_65C02_functional_tests) |

## Build

```sh
# Asm test
ca65 test_prog.asm -o test_prog.o
ld65 -C test_prog.cfg test_prog.o -o test_prog.bin

# C test
ca65 vectors.asm -o vectors.o
cl65 -t none -C test_prog_c.cfg -o test_prog_c.bin test_prog.c vectors.o
```
