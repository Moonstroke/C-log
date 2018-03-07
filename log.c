#include "log.h"

#include <ctype.h> /* for isspace */
#include <time.h> /* for time_t, localtime, strftime */
#include <stdbool.h>



static FILE *_logfile = NULL; /* cannot initialize to stderr :'( error is
                                "initializer element is not constant" */
static LogLevel _logfilter = LOGF_ALL;
static const char *const _levelheaders[] = {
		[LOG_DEBUG] = "DEBUG",
		[LOG_VERBOSE] = "VERBOSE",
		[LOG_INFO] = "INFO",
		[LOG_NOTICE] = "NOTICE",
		[LOG_WARNING] = "WARNING",
		[LOG_ERROR] = "ERROR",
		[LOG_FATAL] = "FATAL"
};

static OutputAttribute _outputattrs = LOG_OUTPUT_MINIMAL;

static const char *const _colorcodes[] = {
	[LOG_DEBUG] = "34", /* blue */
	[LOG_VERBOSE] = "36", /* cyan */
	[LOG_INFO] = "32", /* green */
	[LOG_NOTICE] = "33", /* yellow */
	[LOG_WARNING] = "35", /* magenta */
	[LOG_ERROR] = "31", /* red */
	[LOG_FATAL] = "1;31", /* bold red */
};
#define BEGIN_COLOR(lvl) fprintf(_logfile, "\x1b[%sm", _colorcodes[lvl])
#define END_COLORS() fputs("\x1b[0m", _logfile)


static inline bool _msgblank(const char *const msg) {
	return *msg == '\0' || (isspace(*msg) && _msgblank(msg + 1));
}

static inline void _printtime(void) {
	time_t t;
	char s[12];
	time(&t);
	strftime(s, 12, "[%H:%M:%S] ", localtime(&t));
	fputs(s, _logfile);
}


void log_setlogfile(FILE *const f) {
	_logfile = f;
}

FILE *log_getlogfile(void) {
	return _logfile;
}

void log_setfilter(const LogLevel lvl) {
	_logfilter = lvl;
}

LogLevel log_getfilter(void) {
	return _logfilter;
}

const char *log_getfiltername(void) {
	return _levelheaders[_logfilter];
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
	if(_logfile == NULL) {
		/* _logfile has not yet been initialized, we do it now */
		_logfile = stderr;
	}
	if(_logfilter <= lvl) {
		if(_msgblank(fmt)) {
			fputs(fmt, _logfile);
			return;
		}
		va_list args;
		if(*fmt == '\n') {
			fputs("\n", _logfile);
			++fmt; /* omit the new line from the message */
		}

		/* Message header */
		if(_outputattrs & LOG_OUTPUT_COLORED)
			BEGIN_COLOR(lvl);
		if(_outputattrs & LOG_OUTPUT_TIME)
			_printtime();
		if(_outputattrs & LOG_OUTPUT_FILE) {
			fprintf(_logfile, "%s:%u", file, line);
			if(_outputattrs & LOG_OUTPUT_FUNC)
				fputc(',', _logfile);
			fputc(' ', _logfile);
		}
		if(_outputattrs & LOG_OUTPUT_FUNC)
			fprintf(_logfile, "%s() ", func);
		fprintf(_logfile, "%s -- ", _levelheaders[lvl]);
		if(_outputattrs & LOG_OUTPUT_COLORED)
			END_COLORS();

		/* The mesage itself */
		va_copy(args, a);
		vfprintf(_logfile, fmt, args);
		va_end(args);
		fprintf(_logfile, "\n");
	}
}
