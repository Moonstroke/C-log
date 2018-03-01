/**
 * \file log.h
 * \author joH1
 *
 * This file contains the definition of a simple logging system in C.
 *
 * Messages will be ouput on \a stderr by default, however the log file can be
 * overwritten with \a log_setlogfile.
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
 * will be output without header line or level. This feature allows clearer
 * output and a hierarchy in the display.
 *
 * Likewise, a message whose first character is a line feed (\c '\\n') will be
 * output (header line and level included) \e after a new blank line.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h> /* for va_* */
#include <stdio.h> /* for FILE, fprintf, fputs */


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
 * \brief Specifies the file to log messages in.
 *
 * \param[in] logfile The file where to log messages, or \c NULL for \a stderr
 *
 * \note If none is set (this function never gets called), messages will be
 * printed to \a stderr.
 */
void log_setlogfile(FILE *logfile);

/**
 * \brief Retrieve the log file.
 *
 * \return The file where messages are logged.
 */
FILE *log_getlogfile(void);

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
 * \return The level of message filter
 *
 * \sa log_getfiltername
 */
LogLevel log_getfilter(void);

/**
 * \brief Gets the name of the filter level.
 *
 * \return The name of the filtering level, in uppercase, e.g. \c "DEBUG"
 *
 * \sa log_getfilter
 */
const char *log_getfiltername(void);


/**
 * \brief Sets the heading line to start each log message with.
 *
 * \param[in] header The header line, or \c NULL for none
 */
void log_setheader(const char *header);

/**
 * \brief Retrieves the heading line.
 *
 * \return header The header line, or \c NULL if none is set.
 */
const char *log_getheader(void);

/**
 * \brief Sets the date and time format used in the logging header -- if any.
 *
 * \param[in] fmt The format for date-time output after the header, in
 *            \a strftime format.
 */
void log_settimefmt(const char *fmt);

/**
 * \brief Retrieves the date and time format.
 *
 * \return The date and time format used in the header
 */
const char *log_gettimefmt(void);


/**
 * \brief Logs a message in the log file (on stderr if none was specified).
 *
 * The format string uses \a printf() 's formatting flags.
 *
 * \note If fmt starts with a \c '\\n' character, a new line will be output just
 *       before the message.
 *
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
void logmsg(LogLevel level, const char *fmt, ...);

/**
 * \brief Logs a debugging message.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define debug(...) logmsg(LOG_DEBUG, __VA_ARGS__)

/**
 * \brief Logs a detailled information message.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define verbose(...) logmsg(LOG_VERBOSE, __VA_ARGS__)

/**
 * \brief Logs a basic information message.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define info(...) logmsg(LOG_INFO, __VA_ARGS__)

/**
 * \brief Logs an information message that requires attention.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define notice(...) logmsg(LOG_NOTICE, __VA_ARGS__)

/**
 * \brief Logs a warning message.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define warning(...) logmsg(LOG_WARNING, __VA_ARGS__)

/**
 * \brief Logs an error message.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define error(...) logmsg(LOG_ERROR, __VA_ARGS__)

/**
 * \brief Logs a fatal error message.
 *
 * \note Use before a call to \a exit(), or a return from \a main.
 *
 * \param[in] ... The format string and the optional arguments
 *
 * \sa logmsg
 */
#define fatal(...) logmsg(LOG_FATAL, __VA_ARGS__)


/**
 * \brief Logs a message of given \a level, with one list of variadic arguments.
 *
 * \param[in] level The level of the message
 * \param[in] fmt   The string format for the message
 * \param[in] args   The arguments to format, as a \a va_list
 *
 * \sa logmsg
 */
void vlogmsg(LogLevel level, const char *fmt, va_list args);


#endif /* LOG_H */
