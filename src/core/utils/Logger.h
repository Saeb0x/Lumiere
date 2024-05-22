#pragma once

#include <iostream>
#include <sstream>

namespace Lumiere 
{ 
	namespace Utils
	{
		enum class LogLevel : unsigned int
		{
			Trace = 1,
			Debug,
			Info,
			Warning,
			Error,
			Fatal
		};

		class Logger
		{
		public:
			template<typename... Arguments>
			static void Log(LogLevel inputLogLevel, const Arguments&... args)
			{
				if (inputLogLevel <= m_CurrentLogLevel)
				{
					(std::cout << ... << args) << std::endl;
				}
			}

			static void SetLogLevel(LogLevel newLogLevel)
			{
				newLogLevel <= LogLevel::Fatal ? m_CurrentLogLevel = newLogLevel : m_CurrentLogLevel = LogLevel::Fatal;
			}
		private:
			static LogLevel m_CurrentLogLevel;
		};
	}
}