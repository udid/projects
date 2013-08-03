#include "utils.h"

void string_truncate(char* str, int size)
{
int i = 0;

for (i = 0; (i < size) && (str[i] != '\0'); ++i);

if (i == size)
str[size - 1] = '\0';
}
