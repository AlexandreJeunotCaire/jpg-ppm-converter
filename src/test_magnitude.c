#include <stdint.h>
#include <stdio.h>

#include "magnitude.h"

#if 0

int main()
{
	uint8_t magnitude;
	uint16_t index;

	for (int16_t value = -2047; value < 2047; value++) {
		printf("%i -> ", value);
		to_magnitude_index(value, &magnitude, &index);
		printf("(%i, %i)", magnitude, index);
		int16_t decoded = from_magnitude_index(magnitude, index);
		printf(" -> %i\n", decoded);
	}

	return 0;
}

#endif
