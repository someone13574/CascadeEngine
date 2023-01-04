#pragma once

#include <chrono>
#include <string>

namespace Acorn_Logging
{
	enum Severity_Level
	{
		ACORN_SEVERITY_TRACE,
		ACORN_SEVERITY_DEBUG,
		ACORN_SEVERITY_INFO,
		ACORN_SEVERITY_WARN,
		ACORN_SEVERITY_ERROR,
		ACORN_SEVERITY_FATAL
	};

	struct Log_Message
	{
		std::string message;

		unsigned int occurrence_line;
		std::string occurrence_file_name;
		std::chrono::time_point<std::chrono::system_clock> occurrence_time;

		Severity_Level severity_level;
	};
}	 // namespace Acorn_Logging