#include <avr/io.h>
#define counter r17
#define mask_reg r8

	.global ark_sb_asm

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

// r24-25: state
// r22-23: key
// r20-21: mask_state
// r18-19: mask_key
// r16: mask_in
// r14: mask_out
// r12-13: masked_sbox
#define data_mask_reg r17

ark_sb_asm:
	SAVE_REGISTERS

	ldi		r17,0x0A
	mov		mask_reg,r17
	clr		counter
	clr		r9

loop:
	eor		data_mask_reg, mask_reg
	movw	r30,r24 // Z = state
	add		r30,data_mask_reg
	adc		r31,r1
	ld		r3,z // r3=state[i]

	movw	r26,r22 // X = key
	add		r26,data_mask_reg
	adc		r27,r1
	ld		r4,x // r4=key[i]

	eor		r4,r3 // r4=state[i]^key[i]
	eor		r4,r16 // r4=state[i]^key[i]^mask_in
	
	movw	r26,r20 // X = mask_state
	add		r26,data_mask_reg
	adc		r27,r1
	ld		r5,x // r5=mask_state[i]

	eor		r4,r5 //r4=state[i]^key[i]^mask_in^mask_state[i]

	movw	r26,r18 // X = mask_key
	add		r26,data_mask_reg
	adc		r27,r1
	ld		r6,x // r6=mask_key[i]

	eor		r4,r6 //r4=state[i]^key[i]^mask_in^mask_state[i]^mask_key[i]
	
	mov		r9,r4 // HD(ARK(i), ARK(i+1))

	movw	r26,r12 //  X = masked_sbox
	add		r26,r4
	adc		r27,r1
	ld		r7,x // r7 = masked_sbox[r4]

	eor		r7,r5 // r7 = masked_sbox[r4]^mask_state[i]
	eor		r7,r14 // r7 = masked_sbox[r4]^mask_state[i]^mask_out

	st		z,r7

	eor		data_mask_reg, mask_reg
	inc		counter
	
	cpi		counter, 0x10
	brne	loop

	RESTORE_REGISTERS
	ret







