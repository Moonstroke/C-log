/**
 * \file clog.h
 * \author joH1
 * \version 0.1
 *
 * This file contains the definition of a simple logging system in C.
 *
 * Messages will be ouput on \a stderr by default, however the log file can be
 * overwritten using \a clog_setlogfile.
 *
 * Each call to a logging function will output its message -- given as a
 * <i>printf</i>-like format string and parameters.
 *
 * A log message is provided with a priority level, and can be filtered out, by
 * specifiying a global filter level higher than the priority of the message.
 *
 * A message containing only <b>blank characters</b>, \e i.e.:
 * - \c '\\t', <i>Horizontal Tabulation</i> (Tab),
 * - \c '\\n', <i>Line Feed</i> (\c Enter key on *nix systems)
 * - \c '\\v', <i>Vertical tabulation</i> (not much encountered these days)
 * - \c '\\f', <i>Form Feed</i> (not much used either)
 * - \c '\\r', <i>Carriage Return</i> (the cursor returns to the beginning of
 *   the line), and
 * - \c ' ' <i>Space</i>
 * will be output without header (level name, time, file info, function name).
 * This feature allows clearer output and a hierarchy in the display.
 *
 * Likewise, a message whose first character is a line feed (\c '\\n') will be
 * output (level name included) \e after a new blank line.
 *
 *
 */

#ifndef CLOG_H
#define CLOG_H

#define HAVE_IOATTRS /* to enable PRINTF */
#include <PUCA/funcattrs.h> /* for NOTNULL, PURE, PRINTF */
#include <stdarg.h> /* for va_* */
#include <stdbool.h>
#include <stdio.h> /* for FILE, fopen, fprintf, fputs */



/**
 * \name Enumerations
 * \{
 */

/**
 * \brief Defines the level of priority of a logging message, or an alias for a
 * log filter.
 *
 * The message is to be filtered out from logs if its priority level is
 * inferior to the one set in clog_setfilter().
 * It will also be preceded in logs by a header, consisting of the name of the
 * level and two separating dashes, e.g.:
 * \verbatim INFO -- info message content \endverbatim
 */
typedef enum {
	/** \brief Designed to be used as a marker for the control flow. */
	CLOG_TRACE,
	/** \brief A message used while developing, to help the programmer. */
	CLOG_DEBUG,

	/** \brief A detailled information message. */
	CLOG_VERBOSE,

	/** An information message. */
	CLOG_INFO,

	/** \brief An important information. */
	CLOG_NOTICE,

	/** \brief Indicates an unexpected state of the system. */
	CLOG_WARNING,

	/** \brief Denotes a severe unexpected behavior which can lead to system
	 *         failure. */
	CLOG_ERROR,

	/** \brief Marks a non-recoverable error happened, and the system will exit
	 *         immediately. */
	CLOG_FATAL,

	/* Those should not be used with log() as they are only aliases for the
	   minimum and maximum filtering levels. */
	/** \brief The maximal allowance filter level (alias of \a CLOG_DEBUG). */
	CLOG_FILTER_ALL = CLOG_DEBUG,

	/** \brief The most quiet level, only crash messages are output; alias of
	 *         \a CLOG_FATAL. */
	CLOG_FILTER_NONE = CLOG_FATAL
} LogLevel;

/**
 * \brief Defines attributes to modify the aspect of the logging output.
 */
typedef enum {
	/**
	 * \brief The message is output in plain text, with only the priority level.
	 */
	CLOG_ATTR_MINIMAL = 0x0,

	/**
	 * \brief The message header displays the logging time.
	 */
	CLOG_ATTR_TIME = 0x1,

	/**
	 * \brief The message logs file name and line number info.
	 */
	CLOG_ATTR_FILE = 0x2,

	/**
	 * \brief Logs the name of the function the message is logged within.
	 */
	CLOG_ATTR_FUNC = 0x4,

	/**
	 * \brief The message is output colored (using ANSI escape sequences).
	 *
	 * \attention Only the header (ie. the level name and optionally verbose
	 *            output info) is colored, the message itself is not.
	 */
	CLOG_ATTR_COLORED = 0x10,

	/**
	*  \brief The message is output with time, line, function and file info.
	*/
	CLOG_ATTR_VERBOSE = CLOG_ATTR_TIME | CLOG_ATTR_FILE | CLOG_ATTR_FUNC
} OutputAttribute;

/**
 * \brief Represents the format in which the messages should be logged.
 */
typedef enum {
	/**
	 * \brief Simple text format.
	 */
	CLOG_FORMAT_TEXT,

	/**
	 * \brief Log messages in XML.
	 *
	 * \note See the file \c clog.dtd for the document type declaration.
	 *
	 * \attention This format cannot be used in conjunction with the
	 *            \a CLOG_INIT_APPEND init mode.
	 */
	CLOG_FORMAT_XML,

	/**
	 * \brief Log messages in comma-separated values (table).
	 */
	CLOG_FORMAT_CSV,

	/**
	 * \brief Log the messages in JSON format.
	 *
	 * \attention This format cannot be used in conjunction with the
	 *            \a CLOG_INIT_APPEND init mode.
	 */
	 CLOG_FORMAT_JSON,
} OutputFormat;


/**
 * \}
 * \name Initialization and termination functions
 * \{
 */

/**
 * \brief Initializes the log system to a file.
 *
 * \param[in] filename The path to the log file
 * \param[in] format   The output format
 * \param[in] attrs The OutputAttribute, or several \c OR -ed together
 *
 * \return \c true iff no error occured.
 */
bool clog_init_file(const char *filename, OutputFormat format,
                    OutputAttribute attrs);

/**
 * \brief Initializes the log system to \c stderr.
 *
 * \param[in] format   The output format
 * \param[in] attrs The OutputAttribute, or several \c OR -ed together
 *
 * \return \c true iff no error occured.
 */
bool clog_init(OutputFormat format, OutputAttribute attrs);

/**
 * \brief Shuts down the log system.
 */
void clog_term(void);


/**
 * \}
 * \name Log settings functions
 * \{
 */

/**
 * \brief Retrieve the log file.
 *
 * \return The file where messages are logged.
 */
FILE *clog_getlogfile(void) PURE;


/**
 * \brief Sets a priority level to filter log messages.
 *
 * Only output messages with \code level >= lvl \endcode.
 *
 * \param[in] filterlevel The logging level to filter messages
 */
void clog_setfilterlevel(LogLevel filterlevel);

/**
 * \brief Retrieves the filter level.
 *
 * \return The priority level of output filter.
 *
 * \sa clog_getfiltername
 */
LogLevel clog_getfilterlevel(void) PURE;

/**
 * \brief Gets the name of the filter level.
 *
 * \return The name of the filtering level, in uppercase, e.g. \c "DEBUG".
 *
 * \sa clog_getfilter
 */
const char *clog_getfiltername(void) PURE;


/**
 * \brief Retrieves the output attributes.
 *
 * \return The \c OR sum of the log output attributes.
 */
OutputAttribute clog_getoutputattrs(void) PURE;


/**
 * \brief Retrieves the format used to log messages.
 *
 * \return The log output format.
 */
OutputFormat clog_getoutputformat(void) PURE;


/**
 * \brief Specifies a lock function to acquire thread lock.
 *
 * \note In a multi-threaded environment where the log file can be accessed
 *       concurrently, using this function ensure thread-safety. If the function
 *       is not called, no thread lock will be performed.
 *
 * \param[in] lock The function to lock the thread
 */
void clog_setlock(void (*lock)(void*)) NOTNULL(1);

/**
 * \brief Specifies the function to release the thread lock.
 *
 * \param[in] unlock The thread unlock function
 */
void clog_setunlock(void (*unlock)(void*)) NOTNULL(1);

/**
 * \brief Gives the parameter to pass to the thread lock and unlock functions.
 *
 * \param[in] userdata The user data
 */
void clog_setlockuserdata(void *userdata);


/**
 * \brief Retrieves the user pointer passed to the thread lock/unlock functions.
 *
 * \return The user data passed to lock funcs.
 */
void *clog_getlockuserdata(void) PURE;


/**
 * \}
 * \name Log functions and macros
 * \{
 */

/**
 * \brief Logs a message in the log file (on stderr if none was specified).
 *
 * The format string uses \a printf() 's formatting flags.
 *
 * \note If fmt starts with a \c '\\n' character, a new line will be output just
 *       before the message.
 *
 * \param[in] file  The absolute path of the source file
 * \param[in] line  The line in the at which the logging is called
 * \param[in] func  The function within which the call is made
 * \param[in] level The level of the message
 * \param[in] fmt   The string format for the message
 * \param[in] ...   The arguments to format
 *
 * \sa debug
 * \sa verbose
 * \sa info
 * \sa notice
 * \sa warning
 * \sa error
 */
void logmsg(const char *file, unsigned int line, const char *func,
            LogLevel level, const char *fmt, ...) PRINTF(5, 6) NOTNULL(1, 3, 5);

/**
 * \brief Logs a trace message.
 *
 * \param[in] ... The format string and optional arguments
 *
 * \sa logmsg
 */
#define trace(...) logmsg(__FILE__, __LINE__, __func__, CLOG_TRACE, __VA_ARGS__)

/**
 * \brief Logs a debugging message.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define debug(...) logmsg(__FILE__, __LINE__, __func__, CLOG_DEBUG, __VA_ARGS__)

/**
 * \brief Logs a detailled information message.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define verbose(...) logmsg(__FILE__, __LINE__, __func__, CLOG_VERBOSE,\
                            __VA_ARGS__)

/**
 * \brief Logs a basic information message.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define info(...) logmsg(__FILE__, __LINE__, __func__, CLOG_INFO, __VA_ARGS__)

/**
 * \brief Logs an information message that requires attention.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define notice(...) logmsg(__FILE__, __LINE__, __func__, CLOG_NOTICE,\
                           __VA_ARGS__)

/**
 * \brief Logs a warning message.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define warning(...) logmsg(__FILE__, __LINE__, __func__, CLOG_WARNING,\
                            __VA_ARGS__)

/**
 * \brief Logs an error message.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define error(...) logmsg(__FILE__, __LINE__, __func__, CLOG_ERROR, __VA_ARGS__)

/**
 * \brief Logs a fatal error message.
 *
 * \note Use before a call to \a exit(), or a return from \a main.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define fatal(...) logmsg(__FILE__, __LINE__, __func__, CLOG_FATAL, __VA_ARGS__)


/**
 * \brief Logs a message of given \a level, with one list of variadic arguments.
 *
 * \note \c va_end is not called on \a args inside the function, however since
 *       the args list is operated on directly, its value is undefined after the
 *       function call.
 *
 * \param[in] file  The absolute path of the source file
 * \param[in] line  The line in the at which the logging is called
 * \param[in] func  The function within which the call is made
 * \param[in] level The level of the message
 * \param[in] fmt   The string format for the message
 * \param[in] args  The arguments to format, as a \a va_list
 *
 * \sa logmsg
 */
void vlogmsg(const char *file, unsigned int line, const char *func,
             LogLevel level, const char *fmt, va_list args)
PRINTF(5, 0) NOTNULL(1, 3, 5);


/** \} */


#include <PUCA/end.h>


#endif /* CLOG_H */
