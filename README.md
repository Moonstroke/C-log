# Clog

[![Build Status](https://travis-ci.org/Moonstroke/Clog.svg?branch=master)](
https://travis-ci.org/Moonstroke/Clog)


## A simple logging system in C

*Clog* is, beyond traditional dutch footwear, a simple logging system written
in and for C.

The project is designed to be lightweight, efficient and easy to use.
The features are fully documented here and in the `log.h` file (in the `inc`
directory) in [Doxygen](doxygen.org) format; in fact, the only files you really
need are `log.h`, and `log.c`, in the `src` directory!

In the same vein as [Log4j](https://logging.apache.org/log4j/2.x/),
[Syslog](https://en.wikipedia.org/wiki/Syslog), etc., the system hierarchizes
the logs in priority levels, here at the number of seven: three in an *error*
category, three in an *info* category, and a debugging level.
These levels, and the associated functions, allow to order and filter the output
easily.

A number of output attributes allow to specify context information for the
message (file name, line number, function name) or to enhance the output
(colors!).

A couple of functions allow to specify user-defined functions to acquire and
release a thread lock; the system is thus safe to use in a multithreaded
environment (where the log destination could be accessed concurrently).

**Sumary table**

|Priority level|  Color |Priority description
|:------------:|:------:|:-------------------
|    `DEBUG`   |  Blue  |Development info for the programmer.
|   `VERBOSE`  |  Cyan  |Details, extraneous information.
|    `INFO`    |  Green |Common information.
|   `NOTICE`   | Yellow |Important information that requires attention.
|   `WARNING`  | Magenta|Indicates unexpected behaviour or potential problem.
|    `ERROR`   |   Red  |The system is in a failure state and will not work properly.
|    `FATAL`   |Bold red|The system can not continue and will exit immediately.



### I. Logging functions

The system defines a set of macros to log a message, each named after their
associated priority level, all lowercase.

The message is presented as a `printf`-styled format string and parameters.



### II. Log file

Messages will be ouput to *stderr* by default, however the log file can be
overwritten to an accessible file on the disk (i.e. opened for writing).

Inside the log file, a message is presented under the form of a header (at least
the log level name, optionally expanded according to output attributes, see
section IV) terminated with two dashes, followed by the formatted message
content.

It is possible to write blank lines in a log message by including newline
characters in the message; besides, a message consisting only of *blank*
characters will be output without a message header.

In the same spirit a message starting with a newline character will be output
*after* a blank line (excluded from the message).



### III. Messages hierarchy

#### 1. Priority levels

A message is provided with a priority level, which is one of the following
table:

See further for an explanation about the colors.


#### 2. *LogLevel* enumeration

*LogLevel* is an `enum` declared with values named after the priority level
names as in the previous table, prepended with `CLOG_`. For example, the value
corresponding to the *info* level is named exactly `CLOG_INFO`.

Two supplementary values are defined, `CLOG_FILTER_ALL` and `CLOG_FILTER_NONE`,
that describe the minimum and maximum filtering levels (see after).


#### 3. Filtering the logs

The function `clog_setfilterlevel(LogLevel)` allows to specify a priority level
below which the messages will not be logged. (This is where the *LogLevel*
values `CLOG_FILTER_*` take all their meaning.)

The initial filter level is set to `CLOG_FILTER_ALL`, which means that all
messages are logged.

The value of the log filter can be retrieved with the function
`clog_getfilterlevel()`.



### IV. Output attributes

The enumeration *OutputAttribute* defines constants which represent an attribute
to modify the display of a logged message. They are named `CLOG_OUTPUT_<name>`,
where `<name>` describes the effect ot the attribute:

 `<name>`|Attribute effect on the message header
:-------:|:-------------------------------------
`MINIMAL`|Only consists of the level name
  `TIME` |Contains the time (in format `hh:mm:ss`) of the logging
  `FILE` |Contains the name of the file and the line number of the function call
  `FUNC` |Contains the name of the function the call was made from
`COLORED`|The header is colored with the color associated with the level (*cf.* Summary table)

**Warning:** Clog uses
[ANSI escape codes](https://en.wikipedia.org/wiki/ANSI_escape_code) to color the
output. This feature involves unprintable characters; although widely supported
in terminal emulators, they may be misrepresented in text editors.

Several attributes can be given simultaneously, by combining them with a bitwise
`OR`. In fact, *OutputAttribute* defines a value, `CLOG_ATTR_VERBOSE`, which
combines the attributes `CLOG_ATTR_TIME`, `FILE` and `FUNC`.
