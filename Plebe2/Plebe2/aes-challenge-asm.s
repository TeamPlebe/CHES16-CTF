#include <avr/io.h>

	.global algo2

GF16_Inv:
	.byte 0x00,0x01,0x09,0x0e,0x0d,0x0b,0x07,0x06,0x0f,0x02,0x0c,0x05,0x0a,0x04,0x03,0x08

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

// r24: d
// r22: md
// r23: mdinv
algo2:
	ldi		r30,lo8(GF16_Inv)
	ldi		r31,hi8(GF16_Inv)	
	movw	r20,r30	
	clr		r17 // a
	mov		r2,r23 // R0=s
	mov		r3,r23 // R1=s
loop:
	eor		r17,r24 // d XOR a
	movw	r30,r20
	add		r30,r17
	adc		r31,r1
	ld		r4,z // Inv(d XOR a)

	eor		r17,r24 // a

	cp		r17,r22 // a == r?
	breq	eq
	nop
	eor		r3,r4

eq:
	eor		r2,r4 // R0 XOR Inv(d XOR a)

	cpi		r17,16
	brne	loop

	clr		r5
	cp		r2,r3 //R0==R1?
	breq	label1
	nop
	inc		r17 //useless
label1:
	inc		r5


	ret

// r24-25: state
// r22-23: _stored_key
// r20-r21: mask_state
// r18-19: mask_key
ark_sb_tfc:
	SAVE_REGISTERS

	RESTORE_REGISTERS
	ret

