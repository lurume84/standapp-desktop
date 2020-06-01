#pragma once

#include <string>
#include "spdlog/spdlog.h"

namespace desktop { namespace core { namespace service {
	class LogService
	{
	public:
		LogService();
		~LogService();

		template<typename... Args>
		static inline void info(const std::string& fmt, const Args &... args)
		{
			if(spdlog::get("ApplicationLog") != nullptr)
			{
				spdlog::get("ApplicationLog")->info(fmt, args...);
			}
		}

		template<typename... Args>
		static inline void error(const std::string& fmt, const Args &... args)
		{
			if (spdlog::get("ApplicationLog") != nullptr)
			{
				spdlog::get("ApplicationLog")->error(fmt, args...);
			}
		}

		template<typename... Args>
		static inline void warn(const std::string& fmt, const Args &... args)
		{
			if (spdlog::get("ApplicationLog") != nullptr)
			{
				spdlog::get("ApplicationLog")->warn(fmt, args...);
			}
		}

		template<typename... Args>
		static inline void critical(const std::string& fmt, const Args &... args)
		{
			if (spdlog::get("ApplicationLog") != nullptr)
			{
				spdlog::get("ApplicationLog")->critical(fmt, args...);
			}
		}
	};
}}}