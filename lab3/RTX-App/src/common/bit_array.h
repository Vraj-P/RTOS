#ifndef BIT_ARRAY_H_
#define BIT_ARRAY_H_

#include "printf.h"
#include "common.h"

void BA_Toggle(char *array, unsigned int index);
char BA_Get(char *array, unsigned int index);
void BA_Print(char *array, size_t len);

#endif
