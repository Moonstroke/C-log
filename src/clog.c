#include "clog.h"

#include <time.h> /* for time_t, localtime, strftime */



#define INLINE __attribute__((__always_inline__)) inline


static InitMode _initmode = CLOG_INIT_TRUNCATE;
static const char *_modes[] = {
	[CLOG_INIT_TRUNCATE] = "w",
	[CLOG_INIT_APPEND] = "a"
};

static FILE *_logfile = NULL; /* cannot initialize to stderr :'( error is
                                "initializer element is not constant" */

static OutputFormat _outputfmt = CLOG_FORMAT_TEXT;

static OutputAttribute _outputattrs = CLOG_ATTR_MINIMAL;

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

static void _vlogmsg_text(const char*, unsigned int, const char*, LogLevel,
                          const char*, va_list);
static void _vlogmsg_xml(const char*, unsigned int, const char*, LogLevel, const
                         char*, va_list);
static void _vlogmsg_csv(const char*, unsigned int, const char*, LogLevel, const
                         char*, va_list);
static void _vlogmsg_json(const char*, unsigned int, const char*, LogLevel,
                          const char*, va_list);
static void (*_outputfuncs[])(const char*, unsigned int, const char*, LogLevel,
                              const char*, va_list) = {
	[CLOG_FORMAT_TEXT] = _vlogmsg_text,
	[CLOG_FORMAT_XML] = _vlogmsg_xml,
	[CLOG_FORMAT_CSV] = _vlogmsg_csv,
	[CLOG_FORMAT_JSON] = _vlogmsg_json,
};

static bool _json1stmsg; /* Necessary for the delimiter comma */

#define DO_LOCK 1
#define DO_UNLOCK 0
static void (*_lockfuncs[])(void*) = {
	[DO_LOCK] = NULL,
	[DO_UNLOCK] = NULL
};
static void *_lockuserdata = NULL;


static INLINE bool _init(FILE *const f, const InitMode m, const OutputFormat
                         fmt, const OutputAttribute a) {
	if(m == CLOG_INIT_APPEND && (fmt == CLOG_FORMAT_XML || fmt == CLOG_FORMAT_JSON))
		return false;
	_logfile = f;
	_initmode = m;
	switch(_outputfmt = fmt) {
		case CLOG_FORMAT_XML:
			fputs("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n",
				  _logfile);
			fputs("<!DOCTYPE log SYSTEM \"clog.dtd\">", _logfile);
			fputs("<log>\n", _logfile);
			break;
		case CLOG_FORMAT_CSV:
			if(a && CLOG_ATTR_TIME)
				fputs("Time (hh:mm:ss)\t", _logfile);
			if(a && CLOG_ATTR_FILE)
				fputs("File name\tLine number\t", _logfile);
			if(a && CLOG_ATTR_FUNC)
				fputs("Function name\t", _logfile);
			fputs("Level name\tMessage content\n", _logfile);
			break;
		case CLOG_FORMAT_JSON:
			/* TODO */
			_json1stmsg = true;
			fputs("{\n\t\"log\": [", _logfile);
			break;
		default: break;
	}
	_outputattrs = a;
	return true;
}

static INLINE PURE int _is_space(const char c) {
	return ('\t' <= c && c <= '\r') || c == ' ';
}

static INLINE PURE NOTNULL(1) int _msgblank(const char *msg) {
	while(_is_space(*msg)) ++msg;
	return !*msg;
}

static INLINE void _printtime(char s[9]) {
	time_t t;
	time(&t);
	strftime(s, 9, "%H:%M:%S", localtime(&t));
}

static INLINE void _lock(int i) {
	if(_lockfuncs[i])
		_lockfuncs[i](_lockuserdata);
}


bool clog_init_file(const char *const s, const InitMode m, const OutputFormat
                    fmt, const OutputAttribute a) {
	FILE *const f = fopen(s, _modes[m]);
	return (f != NULL) && _init(f, m, fmt, a);
}

bool clog_init(const OutputFormat fmt, const OutputAttribute a) {
	return _init(stderr, CLOG_INIT_TRUNCATE, fmt, a);
}

void clog_term(void) {
	switch(_outputfmt) {
		case CLOG_FORMAT_XML:
			fputs("</log>\n", _logfile);
			break;
		case CLOG_FORMAT_JSON:
			fputs("\n\t]\n}\n", _logfile);
			break;
		default: break;
	}
	fclose(_logfile);
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

		_outputfuncs[_outputfmt](file, line, func, lvl, fmt, args);

		/* release thread lock */
		_lock(DO_UNLOCK);
	}
}


static void _vlogmsg_text(const char *const file, const unsigned int line, const
                          char *const func, const LogLevel lvl, const char
                          *const fmt, va_list args) {
	/* Message header */
	if(_outputattrs & CLOG_ATTR_COLORED)
		BEGIN_COLOR(lvl);
	if(_outputattrs & CLOG_ATTR_TIME) {
		char tstr[9];
		_printtime(tstr);
		fprintf(_logfile, "[%s] ", tstr);
	}
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
}

static void _vlogmsg_xml(const char *const file, const unsigned int line, const
                         char *const func, const LogLevel lvl, const char
                         *const fmt, va_list args) {
	/*
		<message time="time" file-name="file" file-line="line" func-name="func" level="lvl">
			msg
		</message>
	*/
	fputs("\t<message ", _logfile);
	if(_outputattrs & CLOG_ATTR_TIME) {
		char tstr[9];
		_printtime(tstr);
		fprintf(_logfile, "time=\"%s\" ", tstr);
	}
	if(_outputattrs & CLOG_ATTR_FILE)
		fprintf(_logfile, "file=\"%s\" line=\"%u\" ", file, line);
	if(_outputattrs & CLOG_ATTR_FUNC)
		fprintf(_logfile, "func=\"%s\" ", func);
	fprintf(_logfile, "level=\"%s\">", _levelnames[lvl]);

	/* The mesage itself */
	vfprintf(_logfile, fmt, args);
	fprintf(_logfile, "</message>\n");
}

static void _vlogmsg_csv(const char *const file, const unsigned int line, const
                         char *const func, const LogLevel lvl, const char
                         *const fmt, va_list args) {
	/*
	time	file	line	func	level	msg
	*/
	if(_outputattrs & CLOG_ATTR_TIME) {
		char tstr[9];
		_printtime(tstr);
		fprintf(_logfile, "%s\t", tstr);
	}
	if(_outputattrs & CLOG_ATTR_FILE)
		fprintf(_logfile, "%s\t%u\t", file, line);
	if(_outputattrs & CLOG_ATTR_FUNC)
		fprintf(_logfile, "%s\t", func);
	fprintf(_logfile, "%s\t", _levelnames[lvl]);

	/* The mesage itself */
	vfprintf(_logfile, fmt, args);
	fprintf(_logfile, "\n");
}

static void _vlogmsg_json(const char *const file, const unsigned int line, const
                          char *const func, const LogLevel lvl, const char
                          *const fmt, va_list args) {
	/*
{
	"log": [
		{
			"time": "15:36:23",
			"file": "myfile.c",
			"line": 42,
			"func": "main",
			"level": WARNING",
			"msg": "There is a bug!"
		}
	]
}
	*/
	if(_json1stmsg) {
		_json1stmsg = false;
	} else
		fputc(',', _logfile);
	fputs("\n\t\t{\n", _logfile);
	if(_outputattrs & CLOG_ATTR_TIME) {
		char tstr[9];
		_printtime(tstr);
		fprintf(_logfile, "\t\t\t\"time\": \"%s\",\n", tstr);
	}
	if(_outputattrs & CLOG_ATTR_FILE)
		fprintf(_logfile, "\t\t\t\"file\": \"%s\",\n\t\t\t\"line\": %u,\n",
		        file, line);
	if(_outputattrs & CLOG_ATTR_FUNC)
		fprintf(_logfile, "\t\t\t\"func\": \"%s\",\n", func);
	fprintf(_logfile, "\t\t\t\"level\": \"%s\",\n", _levelnames[lvl]);

	/* The mesage itself */
	fputs("\t\t\t\"msg: \"", _logfile);
	vfprintf(_logfile, fmt, args);
	fprintf(_logfile, "\"\n\t\t}");
}
