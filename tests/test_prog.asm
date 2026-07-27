; 6502 opcode test program
; Load at $0400, reset vector at $FFFC -> $0400
; Results: $0300-$0308 (ADC), $0310-$0318 (SBC), $0320-$032C (CMP/CPX/CPY/INC/DEC/INX/DEX/INY/DEY)

.org $0400

; ---- setup zero-page operands ----
  LDA #$30
  STA $0040        ; $40 = $30
  LDA #$80
  STA $0050        ; $50 = $80
  LDA #$10
  STA $0060        ; $60 = $10

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

; ==== CMP tests (store carry via ADC #$00) ====

; 1: carry set (A > value)
  LDA #$40
  CMP #$30         ; C = 1
  LDA #$00
  ADC #$00         ; A = 1
  STA $0320

; 2: equal (A == value)
  LDA #$40
  CMP #$40         ; C = 1, Z = 1
  LDA #$00
  ADC #$00         ; A = 1
  STA $0321

; 3: carry clear (A < value)
  LDA #$40
  CMP #$50         ; C = 0, N = 1
  LDA #$00
  ADC #$00         ; A = 0
  STA $0322

; ==== CPX tests ====

; 4: carry set (X > value)
  LDX #$20
  CPX #$10         ; C = 1
  LDA #$00
  ADC #$00         ; A = 1
  STA $0323

; ==== CPY tests ====

; 5: carry clear (Y < value)
  LDY #$20
  CPY #$30         ; C = 0
  LDA #$00
  ADC #$00         ; A = 0
  STA $0324

; ==== INC / DEC tests ====

; 6: INC from 0
  LDA #$00
  STA $0070        ; $70 = 0
  INC $70          ; $70 = 1
  LDA $70
  STA $0325

; 7: INC again
  INC $70          ; $70 = 2
  LDA $70
  STA $0326

; 8: DEC from 2
  DEC $70          ; $70 = 1
  LDA $70
  STA $0327

; 9: DEC to 0
  DEC $70          ; $70 = 0, Z = 1
  LDA $70
  STA $0328

; ==== INX / DEX tests ====

; 10: INX to zero
  LDX #$FE
  INX              ; X = $FF
  INX              ; X = $00, Z = 1
  STX $0329

; 11: DEX to negative
  DEX              ; X = $FF, N = 1
  STX $032A

; ==== INY / DEY tests ====

; 12: INY
  LDY #$01
  INY              ; Y = $02
  STY $032B

; 13: DEY to zero
  DEY              ; Y = $01
  DEY              ; Y = $00, Z = 1
  STY $032C

; ---- halt ----
  BRK
