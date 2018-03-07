/**
 * \file log.h
 * \author joH1
 *
 * This file contains the definition of a simple logging system in C.
 *
 * Messages will be ouput on \a stderr by default, however the log file can be
 * overwritten using \a log_setlogfile.
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
 * will be output without header (level name). This feature allows clearer
 * output and a hierarchy in the display.
 *
 * Likewise, a message whose first character is a line feed (\c '\\n') will be
 * output (level name included) \e after a new blank line.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h> /* for va_* */
#include <stdio.h> /* for FILE, fprintf, fputs */

#include "log_attrs.h" /* for function attributes */


/**
 * \brief Defines the level of priority of a logging message, or an alias for a
 * log filter.
 *
 * The message is to be filtered out from logs if its priority level is
 * inferior to the one set in log_setfilter().
 * It will also be preceded in logs by a header, consisting of the name of the
 * level and two separating dashes, e.g.:
 * \verbatim INFO -- info message content \endverbatim
 */
typedef enum {
	LOG_DEBUG = 0,
	/**< A message used while developing, to help the programmer */
	LOG_VERBOSE,
	/**< A detailled information message */
	LOG_INFO,
	/**< An information message */
	LOG_NOTICE,
	/**< An important information */
	LOG_WARNING,
	/**< Indicates an unexpected state of the system */
	LOG_ERROR,
	/**< Denotes a severe unexpected behavior which can lead to system
	     failure */
	LOG_FATAL,
	/**< Marks a non-recoverable error happened, and the system will exit
	     immediately */

	/* Those should not be used with log() as they are only aliases for the
	minimum and maximum filtering levels. */
	LOGF_ALL = LOG_DEBUG,
	/**< The maximal allowance level in filter (alias to \a LOG_DEBUG) */
	LOGF_NONE = LOG_FATAL
	/**< The most quiet level (only crash messages are output), alias to
	     \a LOG_FATAL */
} LogLevel;


/**
 * \brief Defines attributes to modify the aspect of the logging output.
 */
typedef enum {
	/**
	 * \brief The message is output in plain text, with only the priority level.
	 */
	LOG_OUTPUT_MINIMAL = 0x0,
	/**
	 * \brief The message header displays the logging time.
	 */
	LOG_OUTPUT_TIME = 0x1,
	/**
	 * \brief The message logs file name and line number info.
	 */
	LOG_OUTPUT_FILE = 0x2,
	/**
	 * \brief Logs the name of the function the message is logged within.
	 */
	LOG_OUTPUT_FUNC = 0x4,
	/**
	 * \brief The message is output colored (using ANSI escape sequences).
	 *
	 * \note Only the header (level name and optionally verbose output info) is
	 *       colored, the message itself is not.
	 */
	LOG_OUTPUT_COLORED = 0x10,
	/**
	*  \brief The message is output with time, line, function and file info.
	*/
	LOG_OUTPUT_VERBOSE = LOG_OUTPUT_TIME | LOG_OUTPUT_FILE | LOG_OUTPUT_FUNC,
} OutputAttribute;


/**
 * \brief Specifies the file to log messages in.
 *
 * \param[in] logfile The file where to log messages, or \c NULL for \a stderr
 *
 * \note If none is set (this function never gets called), messages will be
 * printed to \a stderr.
 */
void log_setlogfile(FILE *logfile) NOTNULL(1);

/**
 * \brief Retrieve the log file.
 *
 * \return The file where messages are logged.
 */
FILE *log_getlogfile(void) PURE;

/**
 * \brief Sets a priority level to filter log messages.
 *
 * Only output messages with \code level >= lvl \endcode.
 *
 * \param[in] filterlevel The logging level to filter messages
 */
void log_setfilter(LogLevel filterlevel);

/**
 * \brief Retrieves the filter level.
 *
 * \return The priority level of output filter.
 *
 * \sa log_getfiltername
 */
LogLevel log_getfilter(void) PURE;

/**
 * \brief Gets the name of the filter level.
 *
 * \return The name of the filtering level, in uppercase, e.g. \c "DEBUG".
 *
 * \sa log_getfilter
 */
const char *log_getfiltername(void) PURE;

/**
 * \brief Sets the output attributes.
 *
 * \param[in] attrs The OutputAttribute, or several \c OR -ed together
 */
void log_setoutputattrs(OutputAttribute attrs);

/**
 * \brief Retrieves the output attributes.
 *
 * \return The \c OR sum of the log output attributes.
 */
OutputAttribute log_getoutputattrs(void) PURE;


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
            LogLevel level, const char *fmt, ...) PRINTF(5, 6);

/**
 * \brief Logs a debugging message.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define debug(...) logmsg(__FILE__, __LINE__, __func__, LOG_DEBUG, __VA_ARGS__)

/**
 * \brief Logs a detailled information message.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define verbose(...) logmsg(__FILE__, __LINE__, __func__, LOG_VERBOSE, __VA_ARGS__)

/**
 * \brief Logs a basic information message.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define info(...) logmsg(__FILE__, __LINE__, __func__, LOG_INFO, __VA_ARGS__)

/**
 * \brief Logs an information message that requires attention.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define notice(...) logmsg(__FILE__, __LINE__, __func__, LOG_NOTICE, __VA_ARGS__)

/**
 * \brief Logs a warning message.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define warning(...) logmsg(__FILE__, __LINE__, __func__, LOG_WARNING, __VA_ARGS__)

/**
 * \brief Logs an error message.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define error(...) logmsg(__FILE__, __LINE__, __func__, LOG_ERROR, __VA_ARGS__)

/**
 * \brief Logs a fatal error message.
 *
 * \note Use before a call to \a exit(), or a return from \a main.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define fatal(...) logmsg(__FILE__, __LINE__, __func__, LOG_FATAL, __VA_ARGS__)


/**
 * \brief Logs a message of given \a level, with one list of variadic arguments.
 *
 * \param[in] level The level of the message
 * \param[in] fmt   The string format for the message
 * \param[in] args   The arguments to format, as a \a va_list
 *
 * \sa logmsg
 */
void vlogmsg(const char *file, unsigned int line, const char *func,
             LogLevel level, const char *fmt, va_list args) PRINTF(5, 0);


#endif /* LOG_H */
