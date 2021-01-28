
#include "utils.h"

bool isEqual(float a, float b) {
	if (abs(a - b) < ESP)
		return true;
	return false;
}