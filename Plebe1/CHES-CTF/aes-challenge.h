/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2015 NewAE Technology Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef AES_INDEPENDANT_
#define AES_INDEPENDANT_

#include <stdint.h>
#include <stdlib.h> // rand

void aes_indep_init(void);
void aes_indep_key(uint8_t * key);
void aes_indep_enc(uint8_t * pt);


void ark_sb_asm(uint8_t *a, uint8_t* b, uint8_t* c, uint8_t* d, uint8_t e, uint8_t f, uint8_t *g);

#endif
