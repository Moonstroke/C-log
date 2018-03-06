#include "log.h"

#include <ctype.h> /* for isspace */
#include <time.h> /* for time_t, localtime, strftime */
#include <stdbool.h>



static FILE *logfile = NULL; /* cannot initialize to stderr :'( error is
                                "initializer element is not constant" */
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

static OutputAttribute _outputattrs = LOG_OUTPUT_MINIMAL;

static const char *_color_codes[] = {
	[LOG_DEBUG] = "34", /* blue */
	[LOG_VERBOSE] = "36", /* cyan */
	[LOG_INFO] = "32", /* green */
	[LOG_NOTICE] = "33", /* yellow */
	[LOG_WARNING] = "35", /* magenta */
	[LOG_ERROR] = "31", /* red */
	[LOG_FATAL] = "1;31", /* bold red */
};
#define BEGIN_COLOR(lvl) fprintf(logfile, "\x1b[%sm", _color_codes[lvl])
#define END_COLORS() fputs("\x1b[0m", logfile)


static inline bool msgblank(const char *const msg) {
	return *msg == '\0' || (isspace(*msg) && msgblank(msg + 1));
}

static inline void _printtime(void) {
	time_t t;
	char s[12];
	time(&t);
	strftime(s, 12, "[%H:%M:%S] ", localtime(&t));
	fputs(s, logfile);
}


void log_setlogfile(FILE *const f) {
	logfile = f;
}

FILE *log_getlogfile(void) {
	return logfile;
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

void log_setoutputattrs(const OutputAttribute a) {
	_outputattrs = a;
}

OutputAttribute log_getoutputattrs(void) {
	return _outputattrs;
}


void logmsg(const char *const file, const unsigned int line, const char *const func,
            const LogLevel level, const char *const fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vlogmsg(file, line, func, level, fmt, args);
	va_end(args);
}

void vlogmsg(const char *const file, const unsigned int line, const char *const func,
             const LogLevel lvl, const char *fmt, va_list a) {
	if(logfile == NULL) {
		/* logfile has not yet been initialized, we do it now */
		logfile = stderr;
	}
	if(logfilter <= lvl) {
		if(msgblank(fmt)) {
			fputs(fmt, logfile);
			return;
		}
		va_list args;
		if(*fmt == '\n') {
			fputs("\n", logfile);
			++fmt; /* omit the new line from the message */
		}

		/* Message header */
		if(_outputattrs & LOG_OUTPUT_COLORED)
			BEGIN_COLOR(lvl);
		if(_outputattrs & LOG_OUTPUT_TIME)
			_printtime();
		if(_outputattrs & LOG_OUTPUT_FILE) {
			fprintf(logfile, "%s:%u", file, line);
			if(_outputattrs & LOG_OUTPUT_FUNC)
				fputc(',', logfile);
			fputc(' ', logfile);
		}
		if(_outputattrs & LOG_OUTPUT_FUNC)
			fprintf(logfile, "%s() ", func);
		fprintf(logfile, "%s -- ", headers[lvl]);
		if(_outputattrs & LOG_OUTPUT_COLORED)
			END_COLORS();

		/* The mesage itself */
		va_copy(args, a);
		vfprintf(logfile, fmt, args);
		va_end(args);
		fprintf(logfile, "\n");
	}
}
