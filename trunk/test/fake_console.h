#ifndef __P2PSEL0_FAKE_CONSOLE_H__
#define __P2PSEL0_FAKE_CONSOLE_H__

#include <iostream>
#include "zig/console.h"

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__TOS_WIN__) || defined(_WIN64)
#include <windows.h>
#else
#define OutputDebugString(obj) { (*out) << obj; }
#endif

class fake_console_c: public console_c {

public:

	fake_console_c(std::ostream* _out) {
		this->out = _out;
	}

	virtual void draw_line(int line, char* buf) {}

	virtual void clear() {}

	virtual void clear_prompt() {}

	virtual void interprete_command(char* cmdstr) {}

	virtual void write_string(const char* outstr);

protected:

	std::ostream* out;
};

#endif

