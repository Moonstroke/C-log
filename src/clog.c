#include "clog.h"

#include <time.h> /* for time_t, localtime, strftime */



static FILE *_logfile = NULL; /* cannot initialize to stderr :'( error is
                                "initializer element is not constant" */

static LogLevel _filterlevel = CLOG_FILTER_ALL;
static const char *const _levelnames[] = {
		[CLOG_DEBUG] = "DEBUG",
		[CLOG_VERBOSE] = "VERBOSE",
		[CLOG_INFO] = "INFO",
		[CLOG_NOTICE] = "NOTICE",
		[CLOG_WARNING] = "WARNING",
		[CLOG_ERROR] = "ERROR",
		[CLOG_FATAL] = "FATAL"
};

static OutputAttribute _outputattrs = CLOG_ATTR_MINIMAL;

static const char *const _colorcodes[] = {
	[CLOG_DEBUG] = "34", /* blue */
	[CLOG_VERBOSE] = "36", /* cyan */
	[CLOG_INFO] = "32", /* green */
	[CLOG_NOTICE] = "33", /* yellow */
	[CLOG_WARNING] = "35", /* magenta */
	[CLOG_ERROR] = "31", /* red */
	[CLOG_FATAL] = "1;31", /* bold red */
};
#define BEGIN_COLOR(lvl) fprintf(_logfile, "\x1b[%sm", _colorcodes[lvl])
#define END_COLORS() fputs("\x1b[0m", _logfile)

#define DO_LOCK 1
#define DO_UNLOCK 0
static void (*_lockfuncs[2])(void*) = {
	[DO_LOCK] = NULL,
	[DO_UNLOCK] = NULL
};
static void *_lockuserdata = NULL;


static INLINE PURE int _is_space(const char c) {
	return ('\t' <= c && c <= '\r') || c == ' ';
}

static INLINE PURE NOTNULL(1) int _msgblank(const char *msg) {
	while(_is_space(*msg)) ++msg;
	return !*msg;
}

static INLINE void _printtime(void) {
	time_t t;
	char s[12];
	time(&t);
	strftime(s, 12, "[%H:%M:%S] ", localtime(&t));
	fputs(s, _logfile);
}

static INLINE void _lock(int i) {
	if(_lockfuncs[i])
		_lockfuncs[i](_lockuserdata);
}


void clog_setlogfile(FILE *const f) {
	_logfile = f;
}

FILE *clog_getlogfile(void) {
	return _logfile;
}

void clog_setfilterlevel(const LogLevel lvl) {
	_filterlevel = lvl;
}

LogLevel clog_getfilterlevel(void) {
	return _filterlevel;
}

const char *clog_getfiltername(void) {
	return _levelnames[_filterlevel];
}

void clog_setoutputattrs(const OutputAttribute a) {
	_outputattrs = a;
}

OutputAttribute clog_getoutputattrs(void) {
	return _outputattrs;
}

void clog_setlock(void (*const f)(void*)) {
	_lockfuncs[DO_LOCK] = f;
}

void clog_setunlock(void (*const f)(void*)) {
	_lockfuncs[DO_UNLOCK] = f;
}

void clog_setlockuserdata(void *const u) {
	_lockuserdata = u;
}

void *clog_getlockuserdata(void) {
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
	_lock(DO_LOCK);

	if(_logfile == NULL) {
		/* _logfile has not yet been initialized, we do it now */
		_logfile = stderr;
	}

	if(_filterlevel <= lvl) {
		if(_msgblank(fmt)) {
			fputs(fmt, _logfile);
			_lock(DO_UNLOCK);
			return;
		}
		if(*fmt == '\n') {
			fputs("\n", _logfile);
			++fmt; /* omit the new line from the message */
		}

		/* Message header */
		if(_outputattrs & CLOG_ATTR_COLORED)
			BEGIN_COLOR(lvl);
		if(_outputattrs & CLOG_ATTR_TIME)
			_printtime();
		if(_outputattrs & CLOG_ATTR_FILE) {
			fprintf(_logfile, "%s:%u", file, line);
			if(_outputattrs & CLOG_ATTR_FUNC)
				fputc(',', _logfile);
			fputc(' ', _logfile);
		}
		if(_outputattrs & CLOG_ATTR_FUNC)
			fprintf(_logfile, "%s() ", func);
		fprintf(_logfile, "%-7s -- ", _levelnames[lvl]);
		if(_outputattrs & CLOG_ATTR_COLORED)
			END_COLORS();

		/* The mesage itself */
		vfprintf(_logfile, fmt, args);
		fprintf(_logfile, "\n");

		/* release thread lock */
		_lock(DO_UNLOCK);
	}
}
