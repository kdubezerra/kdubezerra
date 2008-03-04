#include <stdarg.h>
#include <iostream>
using namespace std;

#include "fake_console.h"

#define MAX_LINE_LENGTH 65536

void fake_console_c::write_string(const char* outstr)
{
	(*out) << outstr;
}
