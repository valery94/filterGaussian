#include "logging.h"

void EasyLogging::initLogger()
{
	char* sizeOfLogFile = "1"; // 2097152 kb - 2 Mb
	el::Configurations defaultConf;
	defaultConf.set(el::Level::Info, el::ConfigurationType::Format, "%level %datetime %msg");
	defaultConf.set(el::Level::Info, el::ConfigurationType::ToStandardOutput, "");
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::MaxLogFileSize, sizeOfLogFile);
	el::Loggers::reconfigureLogger("default", defaultConf);
	
}
