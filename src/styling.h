#pragma once

#include <string>

#ifdef _WIN32
#include <wchar.h>
#include <windows.h>
#endif


/**
 * Check if we can enable virtual terminal (needed for ANSI escape sequences)
 * From: https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#example-of-enabling-virtual-terminal-processing
 */
static bool enableVirtualTerminal()
{
#ifdef _WIN32
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return false;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return false;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
        return false;

	return true;
#endif
	return true;
}


namespace Styling
{
	static bool g_isVirtual = enableVirtualTerminal();

	static inline constexpr unsigned int reset			= 0;
	static inline constexpr unsigned int bold			= 1;
	static inline constexpr unsigned int dim			= 1 << 1;
	static inline constexpr unsigned int italic			= 1 << 2;
	static inline constexpr unsigned int underline		= 1 << 3;
	static inline constexpr unsigned int strikeout		= 1 << 4;
	static inline constexpr unsigned int normal			= 1 << 5;
	static inline constexpr unsigned int black			= 1 << 6;
	static inline constexpr unsigned int red			= 1 << 7;
	static inline constexpr unsigned int green			= 1 << 8;
	static inline constexpr unsigned int yellow			= 1 << 9;
	static inline constexpr unsigned int blue			= 1 << 10;
	static inline constexpr unsigned int magenta		= 1 << 11;
	static inline constexpr unsigned int cyan			= 1 << 12;
	static inline constexpr unsigned int white			= 1 << 13;
	static inline constexpr unsigned int brightBlack	= 1 << 14;
	static inline constexpr unsigned int brightRed		= 1 << 15;
	static inline constexpr unsigned int brightGreen	= 1 << 16;
	static inline constexpr unsigned int brightYellow	= 1 << 17;
	static inline constexpr unsigned int brightBlue		= 1 << 18;
	static inline constexpr unsigned int brightMagenta	= 1 << 19;
	static inline constexpr unsigned int brightCyan		= 1 << 20;
	static inline constexpr unsigned int brightWhite	= 1 << 21;

	static inline constexpr unsigned int debug		= brightBlack;
	static inline constexpr unsigned int info		= cyan;
	static inline constexpr unsigned int warning	= bold | yellow;
	static inline constexpr unsigned int error		= bold | red;
	static inline constexpr unsigned int success	= bold | green;


	static std::string style(const unsigned int style = reset, bool noReset = false)
	{
		if (!g_isVirtual) return "";

		if (style == reset) return "\033[0m";

		std::string styleString = noReset ? "\033[" : "\033[0;";

		if (style & bold) styleString			+= "1;";
		if (style & dim) styleString			+= "2;";
		if (style & italic) styleString			+= "3;";
		if (style & underline) styleString		+= "4;";
		if (style & strikeout) styleString		+= "9;";
		if (style & normal) styleString			+= "22;";

		if (style & black) styleString			+= "30;";
		if (style & red) styleString			+= "31;";
		if (style & green) styleString			+= "32;";
		if (style & yellow) styleString			+= "33;";
		if (style & blue) styleString			+= "34;";
		if (style & magenta) styleString		+= "35;";
		if (style & cyan) styleString			+= "36;";
		if (style & white) styleString			+= "37;";

		if (style & brightBlack) styleString	+= "90;";
		if (style & brightRed) styleString		+= "91;";
		if (style & brightGreen) styleString	+= "92;";
		if (style & brightYellow) styleString	+= "93;";
		if (style & brightBlue) styleString		+= "94;";
		if (style & brightMagenta) styleString	+= "95;";
		if (style & brightCyan) styleString		+= "96;";
		if (style & brightWhite) styleString	+= "97;";

		if (!styleString.empty() && styleString.back() == ';')
			styleString.pop_back();

		return styleString + 'm';
	}
}
