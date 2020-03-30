#pragma once

#include <string>

struct LogEvent {
	enum LogLevel {
		ERROR = 1,
		WARN  = 2,
		LOG  = 4,
		DEBUG = 8
	};
	
	std::string text;
	LogLevel level = LogEvent::DEBUG;
};
