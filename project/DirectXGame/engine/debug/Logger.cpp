#include "Logger.h"
#include<dxgidebug.h>

namespace Logger {
	void Logger::Log(const std::string& message)
	{
		OutputDebugStringA(message.c_str());
	}
}