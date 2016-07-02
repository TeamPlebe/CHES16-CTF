#include "aes-challenge.h"
#include <avr/eeprom.h>

uint8_t _stored_key[16*11] = {0}; // all round keys
uint8_t *round_key;

uint8_t state[16];
uint8_t mask_state[16];
uint8_t mask_key[16];
uint8_t mout[16],mmul[16],mmul_inv[16],r,mem;
uint8_t T[256];

/*
// Debug stuff

#define ROTL8(x,shift) ((uint8_t) ((x) << (shift)) | ((x) >> (8 - (shift))))

uint8_t aff_trans(uint8_t q) {
	return q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);
}

void AMToMM(uint8_t *x, uint8_t min, uint8_t b)
{
	uint8_t res, tmp;
	res = r;
	res = res ^ *x;
	res = res ^ min;
	res = gmul_asm(res, b);
	tmp = *x;
	mem = T[tmp];
	tmp = gmul_asm(b,mem);
	res = res ^ tmp;
	*x = res;
}

void Op(uint8_t *x, uint8_t *b)
{
	*x = ginv_asm(*x);
	*b = ginv_asm(*b);
}

void MMToAM(uint8_t *x, uint8_t binv, uint8_t b, uint8_t mout)
{
	uint8_t res, tmp;
	res = mem;
	res = res ^ mout;
	res = res ^ r;
	res = gmul_asm(binv,res);
	tmp = *x;
	res = res ^ tmp;
	res = gmul_asm(b,res);
	*x = res;
}


void ark_sb_g10sec(uint8_t *state, uint8_t *_stored_key, uint8_t *mask_state, uint8_t *mask_key)
{
	uint8_t i,aa,bb,binv,res,dbg1,dbg2;
	
	// ark + inv
	for(i=0;i<16;i++)
	{
		state[i] = (state[i]^_stored_key[i])^mask_key[i];
		//res=sbox[state[i]^mask_state[i]];
		//inv()
		aa = state[i];
		bb = mask_state[i];
		T[bb] ^= 1;
		
		//dbg1=gmul_asm((aa^bb),b);
		AMToMM(&aa,bb,mmul[i]);
		
		binv = ginv_asm(mmul[i]);
		aa = ginv_asm(aa);
		
		//dbg2=gmul_asm(aa,b);
		
		MMToAM(&aa, binv, mmul[i], mout[i]);
		
		T[bb] ^= 1;
		
		state[i] = aff_trans(aa)^0x63;
		mask_state[i] = aff_trans(mout[i]);
		
	}
}
*/

/* Sbox Lookup Table */
const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

const uint8_t mCM2[] = {
	0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,0x18,0x1a,0x1c,0x1e,
	0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,
	0x40,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,
	0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,0x70,0x72,0x74,0x76,0x78,0x7a,0x7c,0x7e,
	0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e,0x90,0x92,0x94,0x96,0x98,0x9a,0x9c,0x9e,
	0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae,0xb0,0xb2,0xb4,0xb6,0xb8,0xba,0xbc,0xbe,
	0xc0,0xc2,0xc4,0xc6,0xc8,0xca,0xcc,0xce,0xd0,0xd2,0xd4,0xd6,0xd8,0xda,0xdc,0xde,
	0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xee,0xf0,0xf2,0xf4,0xf6,0xf8,0xfa,0xfc,0xfe,
	0x1b,0x19,0x1f,0x1d,0x13,0x11,0x17,0x15,0x0b,0x09,0x0f,0x0d,0x03,0x01,0x07,0x05,
	0x3b,0x39,0x3f,0x3d,0x33,0x31,0x37,0x35,0x2b,0x29,0x2f,0x2d,0x23,0x21,0x27,0x25,
	0x5b,0x59,0x5f,0x5d,0x53,0x51,0x57,0x55,0x4b,0x49,0x4f,0x4d,0x43,0x41,0x47,0x45,
	0x7b,0x79,0x7f,0x7d,0x73,0x71,0x77,0x75,0x6b,0x69,0x6f,0x6d,0x63,0x61,0x67,0x65,
	0x9b,0x99,0x9f,0x9d,0x93,0x91,0x97,0x95,0x8b,0x89,0x8f,0x8d,0x83,0x81,0x87,0x85,
	0xbb,0xb9,0xbf,0xbd,0xb3,0xb1,0xb7,0xb5,0xab,0xa9,0xaf,0xad,0xa3,0xa1,0xa7,0xa5,
	0xdb,0xd9,0xdf,0xdd,0xd3,0xd1,0xd7,0xd5,0xcb,0xc9,0xcf,0xcd,0xc3,0xc1,0xc7,0xc5,
	0xfb,0xf9,0xff,0xfd,0xf3,0xf1,0xf7,0xf5,0xeb,0xe9,0xef,0xed,0xe3,0xe1,0xe7,0xe5};
	
const uint8_t mCM3[] = {
	0x00,0x03,0x06,0x05,0x0c,0x0f,0x0a,0x09,0x18,0x1b,0x1e,0x1d,0x14,0x17,0x12,0x11,
	0x30,0x33,0x36,0x35,0x3c,0x3f,0x3a,0x39,0x28,0x2b,0x2e,0x2d,0x24,0x27,0x22,0x21,
	0x60,0x63,0x66,0x65,0x6c,0x6f,0x6a,0x69,0x78,0x7b,0x7e,0x7d,0x74,0x77,0x72,0x71,
	0x50,0x53,0x56,0x55,0x5c,0x5f,0x5a,0x59,0x48,0x4b,0x4e,0x4d,0x44,0x47,0x42,0x41,
	0xc0,0xc3,0xc6,0xc5,0xcc,0xcf,0xca,0xc9,0xd8,0xdb,0xde,0xdd,0xd4,0xd7,0xd2,0xd1,
	0xf0,0xf3,0xf6,0xf5,0xfc,0xff,0xfa,0xf9,0xe8,0xeb,0xee,0xed,0xe4,0xe7,0xe2,0xe1,
	0xa0,0xa3,0xa6,0xa5,0xac,0xaf,0xaa,0xa9,0xb8,0xbb,0xbe,0xbd,0xb4,0xb7,0xb2,0xb1,
	0x90,0x93,0x96,0x95,0x9c,0x9f,0x9a,0x99,0x88,0x8b,0x8e,0x8d,0x84,0x87,0x82,0x81,
	0x9b,0x98,0x9d,0x9e,0x97,0x94,0x91,0x92,0x83,0x80,0x85,0x86,0x8f,0x8c,0x89,0x8a,
	0xab,0xa8,0xad,0xae,0xa7,0xa4,0xa1,0xa2,0xb3,0xb0,0xb5,0xb6,0xbf,0xbc,0xb9,0xba,
	0xfb,0xf8,0xfd,0xfe,0xf7,0xf4,0xf1,0xf2,0xe3,0xe0,0xe5,0xe6,0xef,0xec,0xe9,0xea,
	0xcb,0xc8,0xcd,0xce,0xc7,0xc4,0xc1,0xc2,0xd3,0xd0,0xd5,0xd6,0xdf,0xdc,0xd9,0xda,
	0x5b,0x58,0x5d,0x5e,0x57,0x54,0x51,0x52,0x43,0x40,0x45,0x46,0x4f,0x4c,0x49,0x4a,
	0x6b,0x68,0x6d,0x6e,0x67,0x64,0x61,0x62,0x73,0x70,0x75,0x76,0x7f,0x7c,0x79,0x7a,
	0x3b,0x38,0x3d,0x3e,0x37,0x34,0x31,0x32,0x23,0x20,0x25,0x26,0x2f,0x2c,0x29,0x2a,
	0x0b,0x08,0x0d,0x0e,0x07,0x04,0x01,0x02,0x13,0x10,0x15,0x16,0x1f,0x1c,0x19,0x1a
		};

const uint8_t rconst [] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c};


void last_ark_rnd(uint8_t* state){
	uint8_t i;
	for(i=0; i < 16; i++){
		state[i] ^= _stored_key[16*10+i] ^ mask_key[i];
	}
}


/* shiftRows                Before         After
 * Row 1: No change        0 4  8 12      0  4  8 12    
 * Row 2: Left Shift by 1  1 5  9 13  =>  5  9 13  1    
 * Row 3: Left Shift by 2  2 6 10 14     10 14  2  6   
 * Row 4: Left Shift by 3  3 7 11 15     15 3   7 11  
 */
void shiftRows(uint8_t* state){
	uint8_t temp;
	//Row 1
	// no changes
	//Row 2
	temp = state[1]; state[1] = state[5]; state[5] = state[9];
    state[9] = state[13]; state[13] = temp;
	//Row 3
	temp = state[10]; state[10] = state[2]; state[2] = temp;
	temp = state[14]; state[14] = state[6]; state[6] = temp;
	//Row 4
	temp = state[3]; state[3] = state[15]; state[15] = state[11]; 
    state[11] = state[7]; state[7] = temp;
}


void mixColumns(uint8_t* state){
		
	uint8_t tmp[4];
	uint8_t i;
		
	for(i = 0; i<16; i+=4){
			
		tmp[0] = (mCM2[state[i]]) ^ (mCM3[state[1+i]]) ^ (state[2+i]) ^ (state[3+i]);
		tmp[1] = (state[i]) ^ (mCM2[state[1+i]]) ^ (mCM3[state[2+i]]) ^ (state[3+i]);
		tmp[2] = (state[i]) ^ (state[1+i]) ^ (mCM2[state[2+i]]) ^ (mCM3[state[3+i]]);
		tmp[3] = (mCM3[state[i]]) ^ (state[1+i]) ^ (state[2+i]) ^ (mCM2[state[3+i]]);
			
		state[i  ] = (int) tmp[0];
		state[i+1] = (int) tmp[1];
		state[i+2] = (int) tmp[2];
		state[i+3] = (int) tmp[3];
	}
		
}

// all round keys
void computeKey(uint8_t* key, uint8_t round){
	uint8_t rcon = rconst[round];
	uint8_t buf0, buf1, buf2, buf3;
	short unsigned idxp = round*16;
	short unsigned idx = idxp+16;
	buf0 = sbox[ key[idxp+13] ];
	buf1 = sbox[ key[idxp+14] ];
	buf2 = sbox[ key[idxp+15] ];
	buf3 = sbox[ key[idxp+12] ];
	
	key[idx+0] = key[idxp+0] ^ buf0 ^ rcon;
    key[idx+1] = key[idxp+1] ^ buf1;
    key[idx+2] = key[idxp+2] ^ buf2;
    key[idx+3] = key[idxp+3] ^ buf3;

	key[idx+4] = key[idxp+4] ^ key[idx+0];
	key[idx+5] = key[idxp+5] ^ key[idx+1];
	key[idx+6] = key[idxp+6] ^ key[idx+2];
	key[idx+7] = key[idxp+7] ^ key[idx+3];

	key[idx+8]  = key[idxp+8] ^ key[idx+4];
	key[idx+9]  = key[idxp+9] ^ key[idx+5];
	key[idx+10] = key[idxp+10] ^ key[idx+6];
	key[idx+11] = key[idxp+11] ^ key[idx+7];

	key[idx+12] = key[idxp+12] ^ key[idx+8];
	key[idx+13] = key[idxp+13] ^ key[idx+9];
	key[idx+14] = key[idxp+14] ^ key[idx+10];
	key[idx+15] = key[idxp+15] ^ key[idx+11];
	
}


void aes(uint8_t *in){
	int i;

	// copy state
	for(i=0; i < 16; i++){
		state[i] = in[i];
	}

	inv_gen10sec_asm(_stored_key);
	shiftRows(state);shiftRows(mask_state);
	mixColumns(state);mixColumns(mask_state);	
	// AES Rounds
	for(i = 0; i < 8; i++){
		inv_gen10sec_asm(_stored_key+(i+1)*16);
		shiftRows(state);shiftRows(mask_state);
		mixColumns(state);mixColumns(mask_state);
	}
	inv_gen10sec_asm(_stored_key+(i+1)*16);
	shiftRows(state);shiftRows(mask_state);
	last_ark_rnd(state);
	
	for(i=0; i < 16; i++){
		in[i] = state[i] ^ mask_state[i];
	}

	return;
}


void aes_indep_init(void){
	uint8_t seed;
	seed = eeprom_read_byte(0);
	srand(seed);
	eeprom_write_byte(0, seed);
	for (uint8_t i = 0; i < 16; i++){
		mask_key[i] = random() & 0xFF;
	}
}

void aes_indep_key(uint8_t * key){
	// init key here
    for (uint8_t i = 0; i < 16; i++){
        _stored_key[i] = key[i];
    }
    
    for(uint8_t i=0; i<10;i++){
    	computeKey(_stored_key, i);
    }
		// get random
	for (uint8_t i = 0; i < 16; i++){
		mask_state[i] = random() & 0xFF;
		mout[i] = random() & 0xFF;
		mmul[i] = (random()&0xFF) | 1;
		mmul_inv[i] = ginv_asm(mmul[i]);
	}
	
	//b = (random()&0xFF) | 1;
	r = random()&0xFF;

	for(uint32_t i=0; i<256; i++)
	{
		T[i] = r;
	}

	// mask round key with mask_key
	for (uint32_t i = 0; i < 11; i++){
		for(uint32_t j = 0; j < 16; j++) {
			_stored_key[j+i*16] ^= mask_key[j];
		}
	}
}



void aes_indep_enc(uint8_t * pt){
	
	for(uint8_t i=0; i < 16; i++){
		pt[i] ^= mask_state[i];
	}
	aes(pt);
    
}

