#include <stdio.h>

#include "fake_console.h"

void fake_console_c::write_string(const char* outstr)
{
	OutputDebugString(outstr);
}
