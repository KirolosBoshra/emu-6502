; 6502 branch instruction test
; Tests all 8 branch opcodes: taken, not-taken, and page-crossing
; Results: $0300-$030F (01=pass, 00=fail)

.org $0400

; ---- BNE ----
; 1: BNE taken (Z=0)
  LDA #$01      ; Z=0
  BNE bne_taken
  LDA #$00
  JMP bne_taken_done
bne_taken:
  LDA #$01
bne_taken_done:
  STA $0300

; 2: BNE not taken (Z=1)
  LDA #$00      ; Z=1
  BNE bne_not_taken_fail
  LDA #$01      ; fell through = correct
  JMP bne_not_taken_done
bne_not_taken_fail:
  LDA #$00
bne_not_taken_done:
  STA $0301

; ---- BEQ ----
; 3: BEQ taken (Z=1)
  LDA #$00      ; Z=1
  BEQ beq_taken
  LDA #$00
  JMP beq_taken_done
beq_taken:
  LDA #$01
beq_taken_done:
  STA $0302

; 4: BEQ not taken (Z=0)
  LDA #$42      ; Z=0
  BEQ beq_not_taken_fail
  LDA #$01
  JMP beq_not_taken_done
beq_not_taken_fail:
  LDA #$00
beq_not_taken_done:
  STA $0303

; ---- BCC ----
; 5: BCC taken (C=0)
  CLC
  LDA #$01
  BCC bcc_taken
  LDA #$00
  JMP bcc_taken_done
bcc_taken:
  LDA #$01
bcc_taken_done:
  STA $0304

; 6: BCC not taken (C=1)
  SEC
  LDA #$01
  BCC bcc_not_taken_fail
  LDA #$01
  JMP bcc_not_taken_done
bcc_not_taken_fail:
  LDA #$00
bcc_not_taken_done:
  STA $0305

; ---- BCS ----
; 7: BCS taken (C=1)
  SEC
  LDA #$01
  BCS bcs_taken
  LDA #$00
  JMP bcs_taken_done
bcs_taken:
  LDA #$01
bcs_taken_done:
  STA $0306

; 8: BCS not taken (C=0)
  CLC
  LDA #$01
  BCS bcs_not_taken_fail
  LDA #$01
  JMP bcs_not_taken_done
bcs_not_taken_fail:
  LDA #$00
bcs_not_taken_done:
  STA $0307

; ---- BPL ----
; 9: BPL taken (N=0)
  LDA #$42      ; N=0
  BPL bpl_taken
  LDA #$00
  JMP bpl_taken_done
bpl_taken:
  LDA #$01
bpl_taken_done:
  STA $0308

; 10: BPL not taken (N=1)
  LDA #$80      ; N=1
  BPL bpl_not_taken_fail
  LDA #$01
  JMP bpl_not_taken_done
bpl_not_taken_fail:
  LDA #$00
bpl_not_taken_done:
  STA $0309

; ---- BMI ----
; 11: BMI taken (N=1)
  LDA #$80      ; N=1
  BMI bmi_taken
  LDA #$00
  JMP bmi_taken_done
bmi_taken:
  LDA #$01
bmi_taken_done:
  STA $030A

; 12: BMI not taken (N=0)
  LDA #$42      ; N=0
  BMI bmi_not_taken_fail
  LDA #$01
  JMP bmi_not_taken_done
bmi_not_taken_fail:
  LDA #$00
bmi_not_taken_done:
  STA $030B

; ---- BVC ----
; 13: BVC taken (V=0)
  CLV
  LDA #$01
  BVC bvc_taken
  LDA #$00
  JMP bvc_taken_done
bvc_taken:
  LDA #$01
bvc_taken_done:
  STA $030C

; 14: BVC not taken (V=1)
  LDA #$7F
  ADC #$01      ; V=1 (overflow: $7F + $01 + 0 = $80, positive+positive=negative)
  LDA #$01
  BVC bvc_not_taken_fail
  LDA #$01
  JMP bvc_not_taken_done
bvc_not_taken_fail:
  LDA #$00
bvc_not_taken_done:
  STA $030D

; ---- BVS ----
; 15: BVS taken (V=1)
  LDA #$7F
  ADC #$01      ; V=1
  LDA #$01
  BVS bvs_taken
  LDA #$00
  JMP bvs_taken_done
bvs_taken:
  LDA #$01
bvs_taken_done:
  STA $030E

; 16: BVS not taken (V=0)
  CLV
  LDA #$01
  BVS bvs_not_taken_fail
  LDA #$01
  JMP bvs_not_taken_done
bvs_not_taken_fail:
  LDA #$00
bvs_not_taken_done:
  STA $030F

; ---- Page-crossing forward test ----
; Fill page with NOPs to force page cross
  LDX #$00
fill_fwd:
  NOP
  INX
  BNE fill_fwd
; After fill, we're at the next page
; 17: BNE forward across page boundary
  LDA #$00      ; Z=1
  LDA #$42      ; Z=0
  BNE page_fwd_ok
  LDA #$00
  JMP page_fwd_done
page_fwd_ok:
  LDA #$01
page_fwd_done:
  STA $0310

; ---- Page-crossing backward test ----
; 18: BCC backward across page boundary
  SEC
  LDX #$00
fill_bwd:
  NOP
  INX
  BNE fill_bwd
; 19: BCS backward (after SEC, C=1)
  SEC
  BCS page_bwd_ok
  LDA #$00
  JMP page_bwd_done
page_bwd_ok:
  LDA #$01
page_bwd_done:
  STA $0311

; ---- Loop test (BNE backward in a counting loop) ----
; 20: Count from 0 to 10 using BNE
  LDA #$00
  TAX
count_loop:
  INX
  CPX #$0A
  BNE count_loop
; X should be $0A after loop
  STX $0312     ; should be $0A

; ---- Mixed flag test ----
; 21: CMP + BNE (most common pattern in C code)
  LDA #$20
  CMP #$20      ; Z=1, C=1
  BNE cmp_eq_fail
  LDA #$01
  JMP cmp_eq_done
cmp_eq_fail:
  LDA #$00
cmp_eq_done:
  STA $0313

; 22: CMP + BNE (not equal)
  LDA #$20
  CMP #$30      ; Z=0, C=0
  BNE cmp_ne_ok
  LDA #$00
  JMP cmp_ne_done
cmp_ne_ok:
  LDA #$01
cmp_ne_done:
  STA $0314

; 23: CMP + BCS (A >= value)
  LDA #$30
  CMP #$20      ; C=1 (A >= value)
  BCS cmp_ge_ok
  LDA #$00
  JMP cmp_ge_done
cmp_ge_ok:
  LDA #$01
cmp_ge_done:
  STA $0315

; 24: CMP + BCC (A < value)
  LDA #$10
  CMP #$20      ; C=0 (A < value)
  BCC cmp_lt_ok
  LDA #$00
  JMP cmp_lt_done
cmp_lt_ok:
  LDA #$01
cmp_lt_done:
  STA $0316

; ---- BRK ----
  BRK

.segment "VECTORS"
.word $0000  ; NMI
.word $0400  ; RESET
.word $0000  ; IRQ
