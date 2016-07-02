#include <avr/io.h>

	.global ark_sb_asm
	.global update_masks
	.global gmul_asm

	.extern gmul_table
	.extern ginv_table
	.extern aff_trans

alog_asm:
    .byte 0x01,0x1A,0x5F,0xF7,0xE5,0x6A,0x53,0x4F,0x4C,0x62,0x83,0x81,0xB5,0x0B,0xFE,0x2F
    .byte 0xFB,0x5D,0xC3,0x5B,0x9F,0x82,0x9B,0xEA,0xFC,0x1B,0x45,0xA8,0x12,0x8F,0x39,0x1C
    .byte 0x03,0x2E,0xE1,0x02,0x34,0xBE,0xF5,0xD1,0xD4,0xA6,0x9E,0x98,0xC4,0x1D,0x19,0x71
    .byte 0x16,0xE7,0x5E,0xED,0xBA,0x9D,0xB6,0x25,0x1F,0x2D,0xCF,0xE3,0x36,0x8A,0x4B,0x24
    .byte 0x05,0x72,0x38,0x06,0x5C,0xD9,0x04,0x68,0x67,0xF1,0xB9,0xB3,0x57,0x27,0x2B,0x93
    .byte 0x3A,0x32,0xE2,0x2C,0xD5,0xBC,0xC1,0x6F,0x21,0x77,0x4A,0x3E,0x5A,0x85,0xDD,0x6C
    .byte 0x0F,0x96,0x48,0x0A,0xE4,0x70,0x0C,0xB8,0xA9,0x08,0xD0,0xCE,0xF9,0x69,0x7D,0xAE
    .byte 0x4E,0x56,0x3D,0x74,0x64,0xDF,0x58,0xB1,0x63,0x99,0xDE,0x42,0xEE,0x94,0x7C,0xB4
    .byte 0x11,0xA1,0xD8,0x1E,0x37,0x90,0x14,0xD3,0xE0,0x18,0x6B,0x49,0x10,0xBB,0x87,0xE9
    .byte 0xD2,0xFA,0x47,0x9C,0xAC,0x7A,0xE8,0xC8,0xA5,0xB0,0x79,0xC6,0x29,0xA7,0x84,0xC7
    .byte 0x33,0xF8,0x73,0x22,0x59,0xAB,0x3C,0x6E,0x3B,0x28,0xBD,0xDB,0x30,0xD6,0x92,0x20
    .byte 0x6D,0x15,0xC9,0xBF,0xEF,0x8E,0x23,0x43,0xF4,0xCB,0x8B,0x51,0x7B,0xF2,0x97,0x52
    .byte 0x55,0x13,0x95,0x66,0xEB,0xE6,0x44,0xB2,0x4D,0x78,0xDC,0x76,0x50,0x61,0xAD,0x60
    .byte 0xB7,0x3F,0x40,0xDA,0x2A,0x89,0x65,0xC5,0x07,0x46,0x86,0xF3,0x8D,0x0D,0xA2,0xF6
    .byte 0xFF,0x35,0xA4,0xAA,0x26,0x31,0xCC,0xCD,0xD7,0x88,0x7F,0x9A,0xF0,0xA3,0xEC,0xA0
    .byte 0xC2,0x41,0xC0,0x75,0x7E,0x80,0xAF,0x54,0x09,0xCA,0x91,0x0E,0x8C,0x17,0xFD,0x00

log_asm:
    .byte 0xFF,0xFF,0x23,0x20,0x46,0x40,0x43,0xD8,0x69,0xF8,0x63,0x0D,0x66,0xDD,0xFB,0x60
    .byte 0x8C,0x80,0x1C,0xC1,0x86,0xB1,0x30,0xFD,0x89,0x2E,0x01,0x19,0x1F,0x2D,0x83,0x38
    .byte 0xAF,0x58,0xA3,0xB6,0x3F,0x37,0xE4,0x4D,0xA9,0x9C,0xD4,0x4E,0x53,0x39,0x21,0x0F
    .byte 0xAC,0xE5,0x51,0xA0,0x24,0xE1,0x3C,0x84,0x42,0x1E,0x50,0xA8,0xA6,0x72,0x5B,0xD1
    .byte 0xD2,0xF1,0x7B,0xB7,0xC6,0x1A,0xD9,0x92,0x62,0x8B,0x5A,0x3E,0x08,0xC8,0x70,0x07
    .byte 0xCC,0xBB,0xBF,0x06,0xF7,0xC0,0x71,0x4C,0x76,0xA4,0x5C,0x13,0x44,0x11,0x32,0x02
    .byte 0xCF,0xCD,0x09,0x78,0x74,0xD6,0xC3,0x48,0x47,0x6D,0x05,0x8A,0x5F,0xB0,0xA7,0x57
    .byte 0x65,0x2F,0x41,0xA2,0x73,0xF3,0xCB,0x59,0xC9,0x9A,0x95,0xBC,0x7E,0x6E,0xF4,0xEA
    .byte 0xF5,0x0B,0x15,0x0A,0x9E,0x5D,0xDA,0x8E,0xE9,0xD5,0x3D,0xBA,0xFC,0xDC,0xB5,0x1D
    .byte 0x85,0xFA,0xAE,0x4F,0x7D,0xC2,0x61,0xBE,0x2B,0x79,0xEB,0x16,0x93,0x35,0x2A,0x14
    .byte 0xEF,0x81,0xDE,0xED,0xE2,0x98,0x29,0x9D,0x1B,0x68,0xE3,0xA5,0x94,0xCE,0x6F,0xF6
    .byte 0x99,0x77,0xC7,0x4B,0x7F,0x0C,0x36,0xD0,0x67,0x4A,0x34,0x8D,0x55,0xAA,0x25,0xB3
    .byte 0xF2,0x56,0xF0,0x12,0x2C,0xD7,0x9B,0x9F,0x97,0xB2,0xF9,0xB9,0xE6,0xE7,0x6B,0x3A
    .byte 0x6A,0x27,0x90,0x87,0x28,0x54,0xAD,0xE8,0x82,0x45,0xD3,0xAB,0xCA,0x5E,0x7A,0x75
    .byte 0x88,0x22,0x52,0x3B,0x64,0x04,0xC5,0x31,0x96,0x8F,0x17,0xC4,0xEE,0x33,0x7C,0xB4
    .byte 0xEC,0x49,0xBD,0xDB,0xB8,0x26,0xDF,0x03,0xA1,0x6C,0x91,0x10,0x18,0xFE,0x0E,0xE0

// Saves non scratchable registers
.macro SAVE_REGISTERS
	 push r1
	 push r2
	 push r3
     push r4
     push r5
     push r6
     push r7
     push r8
     push r9
     push r10
     push r11
     push r12
     push r13
     push r14
     push r15
	 push r16
	 push r17
     .endm

// Reload non scratchable registers
.macro RESTORE_REGISTERS 
     pop        r17
     pop        r16
     pop        r15
     pop        r14
     pop        r13
     pop        r12
     pop        r11
     pop        r10
     pop        r9
     pop        r8
     pop        r7
     pop        r6
     pop        r5
     pop        r4
	 pop        r3
	 pop        r2
	 pop        r1
     .endm

// r24: a
// r22: b
gmul_asm:
	push	r15
	push	r16

	clr		r15
	inc		r15 // r15=1

	ldi		r30, lo8(log_asm)
	ldi		r31, hi8(log_asm)
	movw	r16, r30
	add		r30, r24
	adc		r31, r1
	elpm	r23, z // log[a]

	movw	r30, r16
	add		r30, r22
	adc		r31, r1
	elpm	r21, z // log[b]

	clr		r20
	add		r23, r21 // log[a]+log[b]
	adc		r20, r1

	add		r23, r20
	clr		r20
	add		r23, r15 // add 1
	adc		r20, r1 // get carry 
	eor		r20, r15 // xor 1
	sub		r23, r20 // modulo 255

	ldi		r30, lo8(alog_asm)
	ldi		r31, hi8(alog_asm)
	add		r30, r23
	adc		r31, r1
	elpm	r20, z // alog[log[a]+log[b] % 255]

    mul     r24, r22
    breq    gmul_zero_input
    clr     r1
    mov     r24, r20

    pop		r16
	pop		r15
	ret

gmul_zero_input:
    mov     r24, r1
    
	pop		r16
	pop		r15
	ret

	
// r24: a
ginv_asm:
	ldi		r30, lo8(log_asm)
	ldi		r31, hi8(log_asm)
	add		r30, r24
	adc		r31, r1
	elpm	r25, z // log[a]

	ldi		r23, 0x01
	ser		r22
	sub		r22, r25 // 255-log[a]
	clr		r25
	add		r22, r23 // add 1
	adc		r25, r1 //get carry
	eor		r25, r23 // carry XOR 1
	sub		r22, r25 // mod 255

	ldi		r30, lo8(alog_asm)
	ldi		r31, hi8(alog_asm)
	add		r30, r22
	adc		r31, r1
	elpm	r22, z // alog[255-log[a] % 255]

    tst     r24
    breq    ginv_zero_input
    mov     r24, r22
    ret
ginv_zero_input:
	ret

// r24: x
aff_trans_asm:
    //bit 0
    mov     r23, r24

    //bit 7
    lsl     r24
    clr     r22
    rol     r22
    eor     r24, r22
    eor     r23, r24

    //bit 6
    lsl     r24
    clr     r22
    rol     r22
    eor     r24, r22
    eor     r23, r24

    //bit 5
    lsl     r24
    clr     r22
    rol     r22
    eor     r24, r22
    eor     r23, r24
    
    //bit 4
    lsl     r24
    clr     r22
    rol     r22
    eor     r24, r22
    eor     r24, r23

    ret



// r24-25: state
// r22-23: _stored_key
// r20-r21: mask_state
// r18-19: mask_key
ark_sb_asm:
	SAVE_REGISTERS

	movw	r8, r24
	movw	r10, r22
	movw	r12, r18
	movw	r14, r20
	clr		r16

loop:
	movw	r30, r8
	add		r30, r16
	adc		r31, r1 
	ld		r2, z // state[i]
	movw	r6, r30

	movw	r26, r10
	add		r26, r16
	adc		r27, r1 
	ld		r3, x // key[i]

	eor		r2, r3 // state[i]^key[i]

	movw	r26, r12
	add		r26, r16
	adc		r27, r1
	ld		r4, x // mask_key[i]

	eor		r2, r4 // (state[i]^key[i])^mask_key[i]
	movw	r26, r14
	add		r26, r16
	adc		r27, r1
	ld		r3, x // mask_state[i]

	mov 	r24, r2
	mov 	r22, r3
	call	gmul_asm
	mov		r2, r24 // r2=(A XOR X) * X

	mov		r24, r3
	mov		r22, r3
	call	gmul_asm
	eor		r2, r24 // r2 = ((A XOR X) * X) ^ X2 = A * X

	mov		r24, r2
	call	ginv_asm
	mov		r2, r24 // r2 = (((A XOR X) * X) ^ X2)^-1 = A^-1 * X^-1
	ldi		r17, 0x01
	eor		r2, r17

	mov		r24, r2
	mov		r22, r3
	call	gmul_asm
    call    aff_trans_asm
	mov		r2, r24
	ldi		r17, 0x63
	eor		r2, r17 // F(A^-1) XOR F(X)
	
	mov		r24, r3
    call    aff_trans_asm

	eor		r2, r3
	eor		r2, r24 // F(A^-1) XOR X

	movw	r30, r6
	st		z, r2

	inc		r16
	cpi		r16, 0x10
	brne	loop
	

	RESTORE_REGISTERS
	ret

// r24-25: state
// r22-23: mask_state
// r20-21: mask_mC
update_masks:
    push    r2
    push    r3
    push    r4
    push    r8
    push    r9
    push    r10
    push    r11
    push    r16

	movw	r8, r22
	movw	r10, r20

	clr		r16
loop_udp:
	movw	r30, r24
	add		r30, r16
	adc		r31, r1
	ld		r2, z //state[i]

	movw	r26, r8
	add		r26, r16
	adc		r27, r1
	ld		r3, x // mask_state[i]
	eor		r2, r3

	movw	r26, r10
	add		r26, r16
	adc		r27, r1
	ld		r4, x // mask_mC[i]
	eor		r2, r4

	st		z, r2
	st		x, r3		


	inc		r16
	cpi		r16, 0x10
	brne	loop_udp

	pop     r16
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     r4
    pop     r3
    pop     r2
    
	ret





