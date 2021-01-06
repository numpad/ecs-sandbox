#pragma once

#include <string>

#undef ERROR

struct LogEvent {
	using LogLevel = int;

	static const int ERROR = 1;
	static const int WARN  = 2;
	static const int LOG   = 4;
	static const int DEBUG = 8;
	
	std::string text;
	LogLevel level = LogEvent::DEBUG;
};
