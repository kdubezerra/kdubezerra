#ifndef _ZIG_STREAMTEST_FAKE_CONSOLE_H_
#define _ZIG_STREAMTEST_FAKE_CONSOLE_H_

#include <iostream>
#include "zig/console.h"

class fake_console_c: public console_c {

public:

	fake_console_c(std::ostream* _out) {
		this->out = _out;
	}

	virtual void draw_line(int line, char* buf) {}

	virtual void clear() {}

	virtual void clear_prompt() {}

	virtual void interprete_command(char* cmdstr) {}

	void write_string(const char* outstr);

protected:

	std::ostream* out;
};

#endif

