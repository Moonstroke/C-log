#include "log.h"

#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>


static FILE *logfile = NULL; // cannot initialize to stderr :'( gcc says
                             //"error: initializer element is not constant"

static const char *header = NULL;

static const char *timefmt = " %c";

static LogLevel logfilter = LOGF_ALL;

static const char* headers[] = {
		[LOG_DEBUG] = "DEBUG",
		[LOG_VERBOSE] = "VERBOSE",
		[LOG_INFO] = "INFO",
		[LOG_WARNING] = "WARNING",
		[LOG_ERROR] = "ERROR",
		[LOG_FATAL] = "FATAL"
};




void log_setlogfile(FILE *const f) {
	logfile = f ? f : stderr;
}

void log_setheader(const char *const value) {
	header = value;
	// determine if datetime should be printed
	//const int l = strlen(header) - 1;
	//if(l >= 0 && (print_datetime = header[l] == '+')) {
	//	header[l] = '\0'; // <--- SEGFAULT here
	//}
}

void log_setfilter(const LogLevel lvl) { logfilter = lvl; }
LogLevel log_getfilter(void) { return logfilter; }

const char *log_getfiltername(void) {
	return headers[logfilter];
}

void log_settimefmt(const char *const fmt) { timefmt = fmt; }
const char *log_gettimefmt(void) { return timefmt; }


static inline void printheader(FILE *const out) {
	time_t t = time(NULL);
	char date[32];
	strftime(date, 32, timefmt, localtime(&t));
	fprintf(out, "%s%s\n", header, date);
}

static bool msgblank(const char *const msg) {
	return *msg == '\0' || (isspace(*msg) && msgblank(msg + 1));
}

void vmlog(const LogLevel lvl, const char *const fmt, va_list a) {
	// initialize logfile if it has not been already
	if(logfile == NULL)
		logfile = stderr;
	if(msgblank(fmt))
		fputs(fmt, logfile);
	else if(logfilter <= lvl) {
		va_list args;
		if(*fmt == '\n')
			fputs("\n", logfile);
		if(header) {
			printheader(logfile);
		}
		fprintf(logfile, "%s -- ", headers[lvl]);
		va_copy(args, a);
		vfprintf(logfile, fmt + (*fmt == '\n'), args);
		va_end(args);
		fprintf(logfile, "\n");
	}
}

extern void debug(const char *fmt, ...);
extern void verbose(const char *fmt, ...);
extern void info(const char *fmt, ...);
extern void warning(const char *fmt, ...);
extern void error(const char *fmt, ...);
extern void fatal(const char *fmt, ...);
