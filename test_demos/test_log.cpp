#include "ThreadPoolff.h"
#include "CurrentThreadff.h"
#include "Loggingff.h"


#include "stdio.h"

using namespace firey;

const char* LogLevelName[6]=
{
	"TRACE",
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"FATAL"
};

int number=0;

void func(){
	Loggerff::LogLevel level=Loggerff::logLevel();
	LOG_INFO<<LogLevelName[level];
	LOG_INFO<<"INFO";
	Loggerff::setLogLevel(Loggerff::TRACE);
	level=Loggerff::logLevel();
	LOG_TRACE<<LogLevelName[level];
	LOG_TRACE<<"TRACE";
//	LOG_TRACE<<firey::LogLevelName[Loggerff::logLevel()];
	LOG_DEBUG<<"DEBUG";
	LOG_WARN<<"WARN";
	LOG_ERROR<<"ERROR";
//	LOG_FATAL<<"FATAL";
	LOG_SYSERR<<"SYSERR";
	LOG_SYSFATAL<<"SYSFATAL";
}

int main()
{
	Threadff thread(func);
	thread.start();
	sleep(10);
	return 0;
}
