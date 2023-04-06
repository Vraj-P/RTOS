#include "bit_array.h"

void BA_Toggle(char *array, unsigned int index)
{
    array[index / 8] ^= 1 << (index % 8);
}

char BA_Get(char *array, unsigned int index)
{
    return 1 & (array[index / 8] >> (index % 8));
}

void BA_Print(char *array, size_t len)
{
    for (int i = 0; i < len; ++i)
        printf("%d ", BA_Get(array, i));
    printf("\n");
}
