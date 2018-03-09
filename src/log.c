#include "log.h"

#include <ctype.h> /* for isspace */
#include <time.h> /* for time_t, localtime, strftime */
#include <stdbool.h>



static FILE *_logfile = NULL; /* cannot initialize to stderr :'( error is
                                "initializer element is not constant" */
static LogLevel _filterlevel = LOG_FILTER_ALL;
static const char *const _levelnames[] = {
		[LOG_DEBUG] = "DEBUG",
		[LOG_VERBOSE] = "VERBOSE",
		[LOG_INFO] = "INFO",
		[LOG_NOTICE] = "NOTICE",
		[LOG_WARNING] = "WARNING",
		[LOG_ERROR] = "ERROR",
		[LOG_FATAL] = "FATAL"
};

static OutputAttribute _outputattrs = LOG_ATTR_MINIMAL;

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

#define DO_LOCK true
#define DO_UNLOCK false
static void (*_lockfuncs[2])(void*) = {
	[DO_LOCK] = NULL,
	[DO_UNLOCK] = NULL
};
static void *_lockuserdata = NULL;


static INLINE PURE bool _msgblank(const char *msg) {
	while(isspace(*msg)) ++msg;
	return !*msg;
}

static INLINE void _printtime(void) {
	time_t t;
	char s[12];
	time(&t);
	strftime(s, 12, "[%H:%M:%S] ", localtime(&t));
	fputs(s, _logfile);
}

static INLINE void _lock(bool i) {
	if(_lockfuncs[i])
		_lockfuncs[i](_lockuserdata);
}


void log_setlogfile(FILE *const f) {
	_logfile = f;
}

FILE *log_getlogfile(void) {
	return _logfile;
}

void log_setfilterlevel(const LogLevel lvl) {
	_filterlevel = lvl;
}

LogLevel log_getfilterlevel(void) {
	return _filterlevel;
}

const char *log_getfiltername(void) {
	return _levelnames[_filterlevel];
}

void log_setoutputattrs(const OutputAttribute a) {
	_outputattrs = a;
}

OutputAttribute log_getoutputattrs(void) {
	return _outputattrs;
}

void log_setlock(void (*const f)(void*)) {
	_lockfuncs[DO_LOCK] = f;
}

void log_setunlock(void (*const f)(void*)) {
	_lockfuncs[DO_UNLOCK] = f;
}

void log_setlockuserdata(void *const u) {
	_lockuserdata = u;
}

void *log_getlockuserdata(void) {
	return _lockuserdata;
}

void logmsg(const char *const file, const unsigned int line, const char *const func,
            const LogLevel level, const char *const fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vlogmsg(file, line, func, level, fmt, args);
	va_end(args);
}

void vlogmsg(const char *const file, const unsigned int line, const char *const func,
             const LogLevel lvl, const char *fmt, va_list args) {
	/* acquire thread lock */
	_lock(true);

	if(_logfile == NULL) {
		/* _logfile has not yet been initialized, we do it now */
		_logfile = stderr;
	}
	if(_filterlevel <= lvl) {
		if(_msgblank(fmt)) {
			fputs(fmt, _logfile);
			_lock(false);
			return;
		}
		if(*fmt == '\n') {
			fputs("\n", _logfile);
			++fmt; /* omit the new line from the message */
		}

		/* Message header */
		if(_outputattrs & LOG_ATTR_COLORED)
			BEGIN_COLOR(lvl);
		if(_outputattrs & LOG_ATTR_TIME)
			_printtime();
		if(_outputattrs & LOG_ATTR_FILE) {
			fprintf(_logfile, "%s:%u", file, line);
			if(_outputattrs & LOG_ATTR_FUNC)
				fputc(',', _logfile);
			fputc(' ', _logfile);
		}
		if(_outputattrs & LOG_ATTR_FUNC)
			fprintf(_logfile, "%s() ", func);
		fprintf(_logfile, "%-7s -- ", _levelnames[lvl]);
		if(_outputattrs & LOG_ATTR_COLORED)
			END_COLORS();

		/* The mesage itself */
		vfprintf(_logfile, fmt, args);
		fprintf(_logfile, "\n");

		/* release thread lock */
		_lock(false);
	}
}
