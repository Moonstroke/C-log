# C / log
## A simple logging system in C

This repo contains a simple message logging and sorting in C.

Each call to a logging function will output its message -- given as a format
string and parameters.

Messages will be ouput on *stderr* by default, however the log file can be
easily overwritten.

A message is provided with a priority level, and can be filtered out, by
specifiying a filter level.
