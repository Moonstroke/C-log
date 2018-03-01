#include "log.h"

#include <ctype.h> /* for isspace */
#include <stdbool.h>
#include <time.h> /* for time_t, time, strftime, localtime */



static FILE *logfile = NULL; /* cannot initialize to stderr :'( error is
                                "initializer element is not constant" */
static const char *header = NULL;
static const char *timefmt = "%c";
static LogLevel logfilter = LOGF_ALL;
static const char* headers[] = {
		[LOG_DEBUG] = "DEBUG",
		[LOG_VERBOSE] = "VERBOSE",
		[LOG_INFO] = "INFO",
		[LOG_NOTICE] = "NOTICE",
		[LOG_WARNING] = "WARNING",
		[LOG_ERROR] = "ERROR",
		[LOG_FATAL] = "FATAL"
};


static inline void printheader(FILE *const out) {
	static char date[32] = {0};
	time_t t = time(NULL);
	strftime(date + 1, 30, timefmt, localtime(&t));
	fprintf(out, "%s %s\n", header, date);
}

static inline bool msgblank(const char *const msg) {
	return *msg == '\0' || (isspace(*msg) && msgblank(msg + 1));
}


void log_setlogfile(FILE *const f) {
	logfile = f ? f : stderr;
}

void log_setheader(const char *const value) {
	header = value;
	/* determine if datetime should be printed */
	/*
	const int l = strlen(header) - 1;
	if(l >= 0 && (print_datetime = (header[l] == '+'))) {
		header[l] = '\0'; // <--- SEGFAULT here
	}
	*/
}

void log_setfilter(const LogLevel lvl) {
	logfilter = lvl;
}

LogLevel log_getfilter(void) {
	return logfilter;
}

const char *log_getfiltername(void) {
	return headers[logfilter];
}


void log_settimefmt(const char *const fmt) {
	timefmt = fmt;
}

const char *log_gettimefmt(void) {
	return timefmt;
}


void logmsg(const LogLevel level, const char *const fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vlogmsg(level, fmt, args);
	va_end(args);
}

void vlogmsg(const LogLevel lvl, const char *const fmt, va_list a) {
	/* Initialize logfile if it has not been already */
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
