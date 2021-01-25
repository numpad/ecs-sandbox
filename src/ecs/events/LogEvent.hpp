#pragma once

#include <string>

struct LogEvent {
	enum LogLevel {
		ERR = 1,
		WARN  = 2,
		LOG  = 4,
		DEBUG = 8
	};
	
	std::string text;
	LogLevel level = LogEvent::DEBUG;
};
