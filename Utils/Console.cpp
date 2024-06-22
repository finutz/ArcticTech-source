#include "Console.h"

#include "../SDK/Interfaces.h"


CGameConsole* Console = new CGameConsole;

void CGameConsole::ArcticTag() {
	CVar->ConsoleColorPrintf(Color(130, 185, 245), "[arctictech] ");
}

void CGameConsole::Print(const std::string& msg) {
	CVar->ConsolePrintf(msg.c_str());
}

void CGameConsole::ColorPrint(const std::string& msg, const Color& color) {
	CVar->ConsoleColorPrintf(color, msg.c_str());
}

void CGameConsole::Log(const std::string& msg) {
	CVar->ConsoleColorPrintf(Color(130, 185, 245), "[arctictech] ");
	CVar->ConsoleColorPrintf(Color(240, 240, 240), msg.c_str());
	CVar->ConsolePrintf("\n");
}

void CGameConsole::Error(const std::string& error) {
	ArcticTag();

	CVar->ConsoleColorPrintf(Color(255, 50, 50), error.c_str());
	CVar->ConsolePrintf("\n");
}