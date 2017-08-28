
#if defined(_MSC_VER) && !defined(NDEBUG)

#include <sstream>
#include <iostream>
#include <windows.h>

// Debugger streambuf adapted from here: <http://www.codeproject.com/Articles/1053/Using-an-output-stream-for-debugging>

template <class CharT, class TraitsT = std::char_traits<CharT>>
class basic_debugbuf : public std::basic_stringbuf<CharT, TraitsT> {
public:
	virtual ~basic_debugbuf() {
		sync();
	}
protected:
	int sync() {
		output_debug_string(str().c_str());
		str(std::basic_string<CharT>()); // Clear the string buffer
		return 0;
	}

	void output_debug_string(const CharT *text);
};

template<> void basic_debugbuf<char>::output_debug_string(const char *text) {
	OutputDebugStringA(text);
}

template<> void basic_debugbuf<wchar_t>::output_debug_string(const wchar_t *text) {
	OutputDebugStringW(text);
}

static basic_debugbuf<char> dbg_buf;
static basic_debugbuf<char> dbg_err_buf;

void set_debug_buffers() {
	std::cout.rdbuf(&dbg_buf);
	std::cerr.rdbuf(&dbg_err_buf);
}

#endif
