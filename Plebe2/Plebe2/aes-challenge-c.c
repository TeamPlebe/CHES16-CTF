#include "aes-challenge.h"
#include <avr/eeprom.h>
#define GETBIT(x,i) (((x) >> (i))&1)
#define SETBIT(x,i) ((x) << (i))
uint8_t _stored_key[16*11] = {0}; // all round keys

uint8_t mask_state[16];
uint8_t mask_key[16];
uint8_t md, mdinv, mhp, mlp;

/*
void map256To16x16(uint8_t *res, uint8_t a)
{
	uint8_t aA, aB, aC;

	aA = aB = aC = 0;
	aA = GETBIT(a,1) ^ GETBIT(a,7);
	aB = GETBIT(a,5) ^ GETBIT(a,7);
	aC = GETBIT(a,4) ^ GETBIT(a,6);

	*res = 0;

	*res |= SETBIT(aC ^ GETBIT(a,0) ^ GETBIT(a,5),0);
	*res |= SETBIT(GETBIT(a,1) ^ GETBIT(a,2),1);
	*res |= SETBIT(aA,2);
	*res |= SETBIT(GETBIT(a,2) ^ GETBIT(a,4),3);

	*res |= SETBIT(aC ^ GETBIT(a,5),4+0);
	*res |= SETBIT(aA ^ aC,4+1);
	*res |= SETBIT(aB ^ GETBIT(a,2) ^ GETBIT(a,3),4+2);
	*res |= SETBIT(aB,4+3);
}

void map16x16To256(uint8_t *res, uint8_t a)
{
	uint8_t aA, aB;
	aA = GETBIT(a,1)^GETBIT(a,4+3);
	aB = GETBIT(a,4+0)^GETBIT(a,4+1);
	
	*res = 0;
	*res |= SETBIT(GETBIT(a,0)^GETBIT(a,4+0),0);
	*res |= SETBIT(aB^GETBIT(a,4+3),1);
	*res |= SETBIT(aA^aB,2);
	*res |= SETBIT(aB^GETBIT(a,1)^GETBIT(a,4+2),3);
	*res |= SETBIT(aA^aB^GETBIT(a,3),4);
	*res |= SETBIT(aB^GETBIT(a,2),5);
	*res |= SETBIT(aA^GETBIT(a,2)^GETBIT(a,3)^GETBIT(a,4+0),6);
	*res |= SETBIT(aB^GETBIT(a,2)^GETBIT(a,4+3),7);
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

const uint8_t Mat256To16x16[256] = {
	0x00,0x01,0x26,0x27,0x4a,0x4b,0x6c,0x6d,0x40,0x41,0x66,0x67,0x0a,0x0b,0x2c,0x2d,0x39,0x38,0x1f,
	0x1e,0x73,0x72,0x55,0x54,0x79,0x78,0x5f,0x5e,0x33,0x32,0x15,0x14,0xd1,0xd0,0xf7,0xf6,0x9b,0x9a,
	0xbd,0xbc,0x91,0x90,0xb7,0xb6,0xdb,0xda,0xfd,0xfc,0xe8,0xe9,0xce,0xcf,0xa2,0xa3,0x84,0x85,0xa8,
	0xa9,0x8e,0x8f,0xe2,0xe3,0xc4,0xc5,0x31,0x30,0x17,0x16,0x7b,0x7a,0x5d,0x5c,0x71,0x70,0x57,0x56,
	0x3b,0x3a,0x1d,0x1c,0x08,0x09,0x2e,0x2f,0x42,0x43,0x64,0x65,0x48,0x49,0x6e,0x6f,0x02,0x03,0x24,
	0x25,0xe0,0xe1,0xc6,0xc7,0xaa,0xab,0x8c,0x8d,0xa0,0xa1,0x86,0x87,0xea,0xeb,0xcc,0xcd,0xd9,0xd8,
	0xff,0xfe,0x93,0x92,0xb5,0xb4,0x99,0x98,0xbf,0xbe,0xd3,0xd2,0xf5,0xf4,0xe4,0xe5,0xc2,0xc3,0xae,
	0xaf,0x88,0x89,0xa4,0xa5,0x82,0x83,0xee,0xef,0xc8,0xc9,0xdd,0xdc,0xfb,0xfa,0x97,0x96,0xb1,0xb0,
	0x9d,0x9c,0xbb,0xba,0xd7,0xd6,0xf1,0xf0,0x35,0x34,0x13,0x12,0x7f,0x7e,0x59,0x58,0x75,0x74,0x53,
	0x52,0x3f,0x3e,0x19,0x18,0x0c,0x0d,0x2a,0x2b,0x46,0x47,0x60,0x61,0x4c,0x4d,0x6a,0x6b,0x06,0x07,
	0x20,0x21,0xd5,0xd4,0xf3,0xf2,0x9f,0x9e,0xb9,0xb8,0x95,0x94,0xb3,0xb2,0xdf,0xde,0xf9,0xf8,0xec,
	0xed,0xca,0xcb,0xa6,0xa7,0x80,0x81,0xac,0xad,0x8a,0x8b,0xe6,0xe7,0xc0,0xc1,0x04,0x05,0x22,0x23,
	0x4e,0x4f,0x68,0x69,0x44,0x45,0x62,0x63,0x0e,0x0f,0x28,0x29,0x3d,0x3c,0x1b,0x1a,0x77,0x76,0x51,
	0x50,0x7d,0x7c,0x5b,0x5a,0x37,0x36,0x11,0x10
};
const uint8_t Mat16x16To256[256] = {
	0x00,0x01,0x5c,0x5d,0xe0,0xe1,0xbc,0xbd,0x50,0x51,0x0c,0x0d,0xb0,0xb1,0xec,0xed,0xff,0xfe,0xa3,
	0xa2,0x1f,0x1e,0x43,0x42,0xaf,0xae,0xf3,0xf2,0x4f,0x4e,0x13,0x12,0xbe,0xbf,0xe2,0xe3,0x5e,0x5f,
	0x02,0x03,0xee,0xef,0xb2,0xb3,0x0e,0x0f,0x52,0x53,0x41,0x40,0x1d,0x1c,0xa1,0xa0,0xfd,0xfc,0x11,
	0x10,0x4d,0x4c,0xf1,0xf0,0xad,0xac,0x08,0x09,0x54,0x55,0xe8,0xe9,0xb4,0xb5,0x58,0x59,0x04,0x05,
	0xb8,0xb9,0xe4,0xe5,0xf7,0xf6,0xab,0xaa,0x17,0x16,0x4b,0x4a,0xa7,0xa6,0xfb,0xfa,0x47,0x46,0x1b,
	0x1a,0xb6,0xb7,0xea,0xeb,0x56,0x57,0x0a,0x0b,0xe6,0xe7,0xba,0xbb,0x06,0x07,0x5a,0x5b,0x49,0x48,
	0x15,0x14,0xa9,0xa8,0xf5,0xf4,0x19,0x18,0x45,0x44,0xf9,0xf8,0xa5,0xa4,0xd6,0xd7,0x8a,0x8b,0x36,
	0x37,0x6a,0x6b,0x86,0x87,0xda,0xdb,0x66,0x67,0x3a,0x3b,0x29,0x28,0x75,0x74,0xc9,0xc8,0x95,0x94,
	0x79,0x78,0x25,0x24,0x99,0x98,0xc5,0xc4,0x68,0x69,0x34,0x35,0x88,0x89,0xd4,0xd5,0x38,0x39,0x64,
	0x65,0xd8,0xd9,0x84,0x85,0x97,0x96,0xcb,0xca,0x77,0x76,0x2b,0x2a,0xc7,0xc6,0x9b,0x9a,0x27,0x26,
	0x7b,0x7a,0xde,0xdf,0x82,0x83,0x3e,0x3f,0x62,0x63,0x8e,0x8f,0xd2,0xd3,0x6e,0x6f,0x32,0x33,0x21,
	0x20,0x7d,0x7c,0xc1,0xc0,0x9d,0x9c,0x71,0x70,0x2d,0x2c,0x91,0x90,0xcd,0xcc,0x60,0x61,0x3c,0x3d,
	0x80,0x81,0xdc,0xdd,0x30,0x31,0x6c,0x6d,0xd0,0xd1,0x8c,0x8d,0x9f,0x9e,0xc3,0xc2,0x7f,0x7e,0x23,
	0x22,0xcf,0xce,0x93,0x92,0x2f,0x2e,0x73,0x72
};
const uint8_t GF16_Square[16] = {
	0x00,0x01,0x04,0x05,0x03,0x02,0x07,0x06,0x0c,0x0d,0x08,0x09,0x0f,0x0e,0x0b,0x0a
};
const uint8_t GF16_Mult[256] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
	0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x03,0x01,0x07,0x05,0x0b,0x09,0x0f,0x0d,
	0x00,0x03,0x06,0x05,0x0c,0x0f,0x0a,0x09,0x0b,0x08,0x0d,0x0e,0x07,0x04,0x01,0x02,
	0x00,0x04,0x08,0x0c,0x03,0x07,0x0b,0x0f,0x06,0x02,0x0e,0x0a,0x05,0x01,0x0d,0x09,
	0x00,0x05,0x0a,0x0f,0x07,0x02,0x0d,0x08,0x0e,0x0b,0x04,0x01,0x09,0x0c,0x03,0x06,
	0x00,0x06,0x0c,0x0a,0x0b,0x0d,0x07,0x01,0x05,0x03,0x09,0x0f,0x0e,0x08,0x02,0x04,
	0x00,0x07,0x0e,0x09,0x0f,0x08,0x01,0x06,0x0d,0x0a,0x03,0x04,0x02,0x05,0x0c,0x0b,
	0x00,0x08,0x03,0x0b,0x06,0x0e,0x05,0x0d,0x0c,0x04,0x0f,0x07,0x0a,0x02,0x09,0x01,
	0x00,0x09,0x01,0x08,0x02,0x0b,0x03,0x0a,0x04,0x0d,0x05,0x0c,0x06,0x0f,0x07,0x0e,
	0x00,0x0a,0x07,0x0d,0x0e,0x04,0x09,0x03,0x0f,0x05,0x08,0x02,0x01,0x0b,0x06,0x0c,
	0x00,0x0b,0x05,0x0e,0x0a,0x01,0x0f,0x04,0x07,0x0c,0x02,0x09,0x0d,0x06,0x08,0x03,
	0x00,0x0c,0x0b,0x07,0x05,0x09,0x0e,0x02,0x0a,0x06,0x01,0x0d,0x0f,0x03,0x04,0x08,
	0x00,0x0d,0x09,0x04,0x01,0x0c,0x08,0x05,0x02,0x0f,0x0b,0x06,0x03,0x0e,0x0a,0x07,
	0x00,0x0e,0x0f,0x01,0x0d,0x03,0x02,0x0c,0x09,0x07,0x06,0x08,0x04,0x0a,0x0b,0x05,
	0x00,0x0f,0x0d,0x02,0x09,0x06,0x04,0x0b,0x01,0x0e,0x0c,0x03,0x08,0x07,0x05,0x0a
};

const uint8_t GF16_Inv[16] = {
	0x00,0x01,0x09,0x0e,0x0d,0x0b,0x07,0x06,0x0f,0x02,0x0c,0x05,0x0a,0x04,0x03,0x08
};

/*
uint8_t GF16_MultByE(uint8_t a)
{
	uint8_t res = 0;
	uint8_t aA, aB;

	aA = aB = 0;

	aA = GETBIT(a,0) ^ GETBIT(a,1);
	aB = GETBIT(a,2) ^ GETBIT(a,3);

	res |= SETBIT(GETBIT(a,1)^aB,0);
	res |= SETBIT(aA,1);
	res |= SETBIT(aA^GETBIT(a,2),2);
	res |= SETBIT(aA^aB,3);

	return res;
}*/


#define ROTL8(x,shift) ((uint8_t) ((x) << (shift)) | ((x) >> (8 - (shift))))

uint8_t aff_trans(uint8_t q) {
	return q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);
}

uint8_t algo2_c(uint8_t x, uint8_t r, uint8_t s)
{
	uint8_t a, R0, R1, cmp;
	R0 = R1 = s;
	
	for(a = 0; a < 16; a++)
	{
		if(a == r)
		{
			R0 = R0^GF16_Inv[x^a];
		}
		else
		{
			R1 = R1^GF16_Inv[x^a];
		}
	}
	
	if(R0 == R1)
	{
		cmp = 0;
	}
	else
	{
		cmp = 1;
	}
	return R0^(cmp*R1);
}

// a = x XOR m
void inv_wisa07(uint8_t *a, uint8_t *m)
{
	uint8_t m16, a16, d, dinv, a16ph, a16pl;
	uint8_t c1,c2,c3,c31,c4,c5,c6,c7,c8,c9;
	
	m16 = Mat256To16x16[*m]; //(mh,ml)
	a16 = Mat256To16x16[*a]; //(ah,al)
	
	c1 = GF16_Square[a16 >> 4]; //ah^2
	c2 = GF16_Mult[(c1<<4)|0x0e]; //ah^2 * e
	c3 = GF16_Mult[a16]; //ah*al
	c31 = GF16_Square[a16&0x0F]; //al^2
	c4 = GF16_Mult[(a16&0xF0)|(m16&0x0F)]; //ah*ml
	c5 = GF16_Mult[(m16&0xF0)|(a16&0x0F)]; //al*mh
	c6 = GF16_Square[m16>>4]; //mh^2
	c7 = GF16_Mult[(c6<<4)|0x0e]; //mh^2 * e
	c8 = GF16_Square[m16&0x0F]; //ml^2
	c9 = GF16_Mult[m16]; //mh*ml
	
	d = c2^c3^c31^md^c4^c5^c7^c8^c9;
	
	dinv = algo2_c(d,md,mdinv);
	
	c1 = GF16_Mult[(a16&0xF0)|dinv]; //ah*dinv
	c2 = GF16_Mult[(m16&0xF0)|dinv]; //mh*dinv
	c3 = GF16_Mult[(a16&0xF0)|mdinv]; //ah*mdinv
	c4 = GF16_Mult[(m16&0xF0)|mdinv]; //mh*mdinv
	a16ph = c1^mhp^c2^c3^c4; //a'h
	
	c1 = GF16_Mult[(dinv<<4)|(a16&0x0F)]; //dinv*al
	c2 = GF16_Mult[(dinv<<4)|(m16&0x0F)]; //dinv*ml
	c3 = GF16_Mult[(mdinv<<4)|(a16&0x0F)]; //mdinv*al
	c4 = GF16_Mult[(mdinv<<4)|(m16&0x0F)]; //mdinv*ml
	a16pl = c1^mlp^a16ph^c2^c3^mhp^c4; //a'l
	
	*a = Mat16x16To256[(a16ph<<4) | a16pl];
	*m = Mat16x16To256[(mhp << 4) | mlp];
	
	 
}

void ark_sb_wise07(uint8_t *state, uint8_t *_stored_key, uint8_t *mask_state, uint8_t *mask_key)
{
	uint8_t i;
	for(i=0;i<16;i++)
	{
		state[i] = (state[i]^_stored_key[i])^mask_key[i];
		inv_wisa07(&state[i],&mask_state[i]);
		state[i] = aff_trans(state[i])^0x63;
		mask_state[i] = aff_trans(mask_state[i]);
	}
}

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
	uint8_t state[16];
	
	// copy state
	for(i=0; i < 16; i++){
		state[i] = in[i];
	}

	ark_sb_wise07(state, _stored_key, mask_state, mask_key);
	shiftRows(state);shiftRows(mask_state);
	mixColumns(state);mixColumns(mask_state);	
	// AES Rounds
	for(i = 0; i < 8; i++){
		ark_sb_wise07(state, _stored_key+(i+1)*16, mask_state, mask_key);
		shiftRows(state);shiftRows(mask_state);
		mixColumns(state);mixColumns(mask_state);
	}
	ark_sb_wise07(state, _stored_key+(i+1)*16, mask_state, mask_key);
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
	}
	
	md = random()&0x0F;
	mdinv = random()&0x0F;
	mhp = random()&0x0F;
	mlp = random()&0x0F;

	// mask round key with mask_key
	for (uint32_t i = 0; i < 11; i++){
		for(uint32_t j = 0; j < 16; j++) {
			_stored_key[j+i*16] ^= mask_key[j];
		}
	}
}



void aes_indep_enc(uint8_t * pt){
	
	/*uint8_t a = 0xA5;
	uint8_t m = 0xBC;
	uint8_t x=a^m;
	uint8_t res=sbox[a];
	inv_wisa07(&x,&m);
	x = aff_trans(x)^0x63;
	m = aff_trans(m);
	uint8_t t = x^m;
	if(res != t)
	{
		printf("tt");
	}*/
	
	
	for(uint8_t i=0; i < 16; i++){
		pt[i] ^= mask_state[i];
	}
	aes(pt);
    
}

