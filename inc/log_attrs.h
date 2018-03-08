#ifndef LOG_ATTRS_H
#define LOG_ATTRS_H

#ifndef LOG_H
# error "This file must not be included directly."
#endif /* ! LOG_H */

#ifndef __GNUC__
# define __attribute__(x) /**< Empty definition for ISO C. */
#endif

/**
 * \brief The arguments at given indices (starting from \c 1) must not be
 *        \c NULL.
 *
 * \param[in] ... The arguments to check for non-\c NULL value.
 */
#define NOTNULL(...) __attribute__((__nonnull__(__VA_ARGS__)))

/**
 * \brief The function does not affect global data.
 */
#define PURE __attribute__((__pure__, __warn_unused_result__))

/**
 * \brief The function call is replaced by its code when encountered.
 */
#define INLINE __attribute__((__always_inline__)) inline

/**
 * \brief The function uses a \a printf -like formatting style.
 *
 * \param[in] fmt  The index of the format string arguments
 * \param[in] arg1 The index of the first argument
 */
#define PRINTF(fmt, arg1) __attribute__((__format__(printf, fmt, arg1)))


#endif /* LOG_ATTRS_H */
