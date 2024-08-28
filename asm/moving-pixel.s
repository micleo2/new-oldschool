.setcpu		"6502"
.case		on

; Display memory spans 0x0300-0x04FF, or all memory in pages 0x3 and 0x4.
; The canvas size is 64x64, each pixel taking one bit.
; One row corresponds to 8 bytes. 64 rows of 8 bytes, gives us 512 bytes in total for the canvas.
DISP_PG0 = $0300
DISP_PG1 = $0400

; The following zero page addresses are all used to flip on a pixel at a specified (x,y) position
z_paint_pattern = 0
; The following two form an address to the place in display
; memory we want to stamp z_paint_pattern into.
z_paint_byte = 1
; This should only be 3 or 4
z_paint_page = 2
; x & y coordinates for a pixel we want to turn on
; both in the range [0, 63]
z_paint_x = 3
z_paint_y = 4

; Game zero page addresses
z_pong_x = 5
z_pong_y = 6
z_pong_vx = 7
z_pong_vy = 8
; game values
pong_w = 4
pong_h = 4

.segment	"CODE"

_start:
  ldx #1
  stx z_paint_pattern ; bit pattern to write to byte.

  ldx #0
  stx z_paint_byte ; current display byte.

  ldx #3
  stx z_paint_page ; current display page.

  ldx #0
  stx z_paint_x ; current x
  ldx #0
  stx z_paint_y ; current y

  ; init game logic
  lda #32
  sta z_pong_x
  sta z_pong_y

  jmp game_loop

game_loop:
; {
  jsr game_update
  jsr game_draw
  jsr mega_burn_cpu
  jsr mega_burn_cpu
  jmp game_loop
; }

game_update:
; {
  lda z_pong_x
  adc #1
  cmp #64
  bne valid_x
  lda #0
valid_x:
  sta z_pong_x
  lda #32
  sta z_pong_y
  rts
; }


game_draw:
; {
  jsr clear_screen
  jsr paint_pong
  rts
; }


clear_screen:
; {
  lda #0
  ldx #0
c_write_pg1:
  sta DISP_PG0,X
  inx
  cpx #0
  bne c_write_pg1
c_write_pg2:
  sta DISP_PG1,X
  inx
  cpx #0
  bne c_write_pg2
  rts
; }

paint_pong:
; {
  ldy z_pong_y
p_init_row:
  ldx z_pong_x
  lda #0
p_add_x:
  cmp #pong_w
  beq p_done_with_row
  stx z_paint_x
  sty z_paint_y
  jsr add_pixel
  inx
  clc
  adc #1
  jmp p_add_x
p_done_with_row:
  lda z_pong_y
  clc
  adc #pong_h
  sec
  sbc #1
  sta 240 ; *240 = pong_y + pong_h
  tya
  cmp 240
  beq p_done
  iny
  jmp p_init_row
p_done:
  rts
; }

add_pixel:
; {
  pha
  txa
  pha
  tya
  pha

  lda z_paint_x ; a = x, max of 63
  lsr A
  lsr A
  lsr A ; a = int(x / 8), max value of 7
  sta z_paint_byte

  lda #3
  sta z_paint_page
  lda z_paint_y ; a = y, max of 63
  asl A
  asl A
  sta 255
  clc
  adc 255 ; a = y * 8 (maybe overflowed)
  bcc no_carry
  ; if there's carry, this pixel belongs to the 2nd display page.
  ldx #4
  stx z_paint_page
no_carry:
  clc
  adc z_paint_byte
  sta z_paint_byte

  lda z_paint_x
  and #%00000111
  sta 255
  lda #7
  sec
  sbc 255
  sta 255 ; 255 contains (7 - (x % 8))
  lda #1
  sta 254 ; 254 will contain the shifted value.
begin_sft:
  lda 255
  beq end_sft
  sec
  sbc #1
  sta 255
  asl 254
  lda 254
  jmp begin_sft
end_sft:
  lda 254
  sta z_paint_pattern
  jsr or_to_display

  pla
  tay
  pla
  tax
  pla
  rts
; }

or_to_display:
; {
  ldx #0
  lda z_paint_pattern
  ora (z_paint_byte,X)
  sta (z_paint_byte,X)
  rts
; }

mega_burn_cpu:
; {
  jsr burn_cpu
  jsr burn_cpu
  jsr burn_cpu
  jsr burn_cpu
  jsr burn_cpu
  jsr burn_cpu
  jsr burn_cpu
  jsr burn_cpu
  jsr burn_cpu
  jsr burn_cpu
  jsr burn_cpu
  jsr burn_cpu
  jsr burn_cpu
  jsr burn_cpu
  jsr burn_cpu
  jsr burn_cpu
  rts
; }

burn_cpu:
; {
  pha
  ldx #0
b_outer:
  lda #0
b_inner:
  clc
  adc #1
  ; jsr burn_cpu2
  bcc b_inner
  inx
  cpx #$ff
  bne b_outer
  pla
  rts
; }
