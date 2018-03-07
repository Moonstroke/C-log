# C / log

[![Build Status](https://travis-ci.org/Moonstroke/C-log.svg?branch=master)](
https://travis-ci.org/Moonstroke/C-log)


## A simple logging system in C

This project defines a simple message logging and sorting system in C.

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
names as in the previous table, prepended with `LOG_`. For example, the value
corresponding to the *info* level is named exactly `LOG_INFO`.

Two supplementary values are defined, `LOG_FILTER_ALL` and `LOG_FILTER_NONE`,
that describe the minimum and maximum filtering levels (see after).


#### 3. Filtering the logs

The function `log_setfilterlevel(LogLevel)` allows to specify a priority level
below which the messages will not be logged. (This is where the *LogLevel*
values `LOG_FILTER_*` take all their meaning.)

The initial filter level is set to `LOG_FILTER_ALL`, which means that all
messages are logged.

The value of the log filter can be retrieved with the function
`log_getfilterlevel()`.



### IV. Output attributes

The enumeration *OutputAttribute* defines constants which represent an attribute
to modify the display of a logged message. They are named `LOG_OUTPUT_<name>`,
where `<name>` describes the effect ot the attribute:

 `<name>`|Attribute effect on the message header
:-------:|:-------------------------------------
`MINIMAL`|Only consists of the level name
  `TIME` |Contains the time (in format `hh:mm:ss`) of the logging
  `FILE` |Contains the name of the file and the line number of the function call
  `FUNC` |Contains the name of the function the call was made from
`COLORED`|The header is colored with the color assocatied with the level (*cf.* Summary table)

Several attributes can be given simultaneously, by combining them with a bitwise
`OR`. In fact, *OutputAttribute* defines a value, `LOG_OUTPUT_VERBOSE`, which
combines the attributes `LOG_OUTPUT_TIME`, `FILE` and `FUNC`.
