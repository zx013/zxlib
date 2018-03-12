#ifndef UNIT_H
#define UNIT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "head.h"
#include "lib.h"
#include "package.h"

#define UNIT_LENGTH			8
#define UNIT_DATA_COUNT		((UNIT_LENGTH - 1) * (UNIT_LENGTH - 1) * (UNIT_LENGTH - 1))
#define UNIT_TOTAL_COUNT		(UNIT_LENGTH * UNIT_LENGTH * UNIT_LENGTH)

typedef package**** unit;


void unit_encode(unit u);

int unit_decode(unit u);

unit unit_malloc(int side);

void unit_free(unit u, int side);

int unit_reset(unit u);


#endif
