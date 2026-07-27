; 6502 ADC + SBC test program
; Assembles to tests/test_prog.bin
; Load at $0200, reset vector at $FFFC -> $0200
; Results stored at $0300-$0308 (ADC) and $0310-$0318 (SBC)

.org $0200

; ---- setup zero-page operands ----
  LDA #$30
  STA $40          ; $40 = $30
  LDA #$80
  STA $50          ; $50 = $80
  LDA #$10
  STA $60          ; $60 = $10

; ==== ADC tests ====

; 1: imm simple
  LDA #$10
  ADC #$20         ; A = $30
  STA $0300

; 2: imm carry out
  ADC #$D0         ; $30 + $D0 = $100 -> A = $00, C = 1
  STA $0301

; 3: imm carry in
  ADC #$0F         ; $00 + $0F + 1 = $10, C = 0
  STA $0302

; 4: zero page
  ADC $40          ; $10 + $30 = $40
  STA $0303

; 5: zp carry out
  LDA #$F0
  ADC $40          ; $F0 + $30 = $120 -> A = $20, C = 1
  STA $0304

; 6: absolute
  LDA #$E0
  ADC $0060        ; $E0 + $10 + 1 = $F1, C = 0, N = 1
  STA $0305

; 7: overflow neg+neg
  LDA $50
  ADC $50          ; $80 + $80 = $100 -> A = $00, C = 1, V = 1
  STA $0306

; 8: overflow pos+pos
  LDA #$7F
  ADC #$00         ; $7F + $00 + 1 = $80, C = 0, V = 1, N = 1
  STA $0307

; 9: zero result
  LDA #$00
  ADC #$00         ; $00 + $00 = $00, Z = 1
  STA $0308

; ==== SBC tests ====

; 1: imm no borrow
  SEC
  LDA #$50
  SBC #$30         ; $50 - $30 = $20, C = 1
  STA $0310

; 2: imm zero result
  LDA #$30
  SBC #$30         ; $30 - $30 = $00, Z = 1
  STA $0311

; 3: imm borrow in
  CLC
  LDA #$20
  SBC #$30         ; $20 - $30 - 1 = $EF, C = 0, N = 1
  STA $0312

; 4: zero page no borrow
  SEC
  LDA #$50
  SBC $40          ; $50 - $30 = $20
  STA $0313

; 5: zero page borrow
  CLC
  LDA #$10
  SBC $40          ; $10 - $30 - 1 = $DF, C = 0, N = 1
  STA $0314

; 6: absolute no borrow
  SEC
  LDA #$40
  SBC $60          ; $40 - $10 = $30
  STA $0315

; 7: absolute borrow
  CLC
  LDA #$05
  SBC $60          ; $05 - $10 - 1 = $F4, C = 0, N = 1
  STA $0316

; 8: overflow pos - neg
  SEC
  LDA #$7F
  SBC #$FF         ; $7F - (-1) = $80, C = 0, V = 1, N = 1
  STA $0317

; 9: overflow neg - pos
  SEC
  LDA #$80
  SBC #$01         ; $80 - 1 = $7F, C = 1, V = 1
  STA $0318

; ---- halt ----
  BRK
