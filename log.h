/**
 * \file log.h
 * \author joH1
 *
 * This file contains the definition of a simple logging system.
 *
 * Each call to a logging function will output its message -- given as a
 * format string and parameters.
 *
 * A message is provided with a priority level, and can be filtered out, by
 * specifiying a filter level.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>


/**
 * \brief Defines the level of priority of a logging message.
 *
 * The message is to be filtered out from logs if its priority level is
 * inferior to the one set in log_setfilter().
 * It will also be preceded in logs by a header, consisting of the name of the
 * level and two separating dashes, e.g.:
 * \verbatim INFO -- info message content \endverbatim
 */
typedef enum {
	LOG_DEBUG = 0, /**< A message used while developing, to help the programmer */
	LOG_VERBOSE,   /**< A detailled information message */
	LOG_INFO,      /**< An information message */
	LOG_WARNING,   /**< Denotes a malfunction in the system */
	LOG_ERROR,     /**< Denotes a severe unexpected behavior which can lead to system failure */

	/* Those should not be used with log() as they only refer to minimum and
	maximum filtering levels. */
	LOGF_ALL = LOG_DEBUG,  /**< The maximal allowance level in filter */
	LOGF_NONE = LOG_ERROR  /**< The most quiet level (only error messages are output) */
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
 * \brief Sets a priority level to filter log messages.
 *
 * Only output messages with \code level >= lvl \endcode
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
 * \brief Sets the date and time format used in the logging header -- if any.
 *
 * \param[in] fmt The format for date-time output after the header
 */
void log_settimefmt(const char *fmt);

/**
 * \brief Retrieves the date and time format.
 *
 * \return The date and time format used in the header
 */
const char *log_gettimefmt(void);


/**
 * \brief Logs a message of given \a level, with one list of variadic arguments.
 *
 * \param[in] level The level of the message
 * \param[in] fmt   The string format for the message 
 * \param[in] args   The arguments to format, as a \a va_list
 *
 * \sa mlog
 */
void vmlog(LogLevel level, const char *fmt, va_list args);

/**
 * \brief Logs a message in the file (on stderr if none was specified).
 *
 * Uses same format than printf() et al.
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
 * \sa warning
 * \sa error
 */
inline void mlog(LogLevel level, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vmlog(level, fmt, args);
	va_end(args);
}



/**
 * \brief Logs a debugging message.
 *
 * \param[in] fmt The string format for the message
 * \param[in] ... The arguments to format
 *
 * \sa mlog
 * \sa vmlog
 */
inline void debug(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vmlog(LOG_DEBUG, fmt, args);
	va_end(args);
}

/**
 * \brief Logs a detailled information message.
 *
 * \param[in] fmt The string format for the message
 * \param[in] ... The arguments to format
 *
 * \sa mlog
 * \sa vmlog
 */
inline void verbose(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vmlog(LOG_VERBOSE, fmt, args);
	va_end(args);
}

/**
 * \brief Logs a basic information message.
 *
 * \param[in] fmt The string format for the message
 * \param[in] ... The arguments to format
 *
 * \sa mlog
 * \sa vmlog
 */
inline void info(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vmlog(LOG_INFO, fmt, args);
	va_end(args);
}

/**
 * \brief Logs a warning message.
 *
 * \param[in] fmt The string format for the message
 * \param[in] ... The arguments to format
 *
 * \sa mlog
 * \sa vmlog
 */
inline void warning(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vmlog(LOG_WARNING, fmt, args);
	va_end(args);
}

/**
 * \brief Logs an error message.
 *
 * \note Use mainly before a call to exit()
 *
 * \param[in] fmt The string format for the message
 * \param[in] ... The arguments to format
 *
 * \sa mlog
 * \sa vmlog
 */
inline void error(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vmlog(LOG_ERROR, fmt, args);
	va_end(args);
}


#endif /* LOG_H */
