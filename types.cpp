#include "types.h"

int CalculateLastByteIndex(long long val) {
	ByteLongLong conv;
	conv.value = val;

	// get the first non-null index of the number
	int i;
	for (i = 7; conv.bytes[i] == 0; i--);

	return i;
}