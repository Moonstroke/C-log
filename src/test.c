#include <assert.h>
#include <stdio.h>
#include <string.h>


#include "clog.h"

#define testlog(...) fprintf(stderr, __VA_ARGS__)

int main(void) {

	const char *const fname = "test.log";

	FILE *const test_log_file = fopen(fname, "w");

	const LogLevel lvl = CLOG_DEBUG;
	const char *const lname = "DEBUG";

	const char *const msg = "Test message log";
	const char *const msg_blank = "\t\n\v\f\r ";

	if(test_log_file == NULL) { /* just in case */
		fprintf(stderr, "Unable to create file \"%s\"\n", fname);
		fprintf(stderr, "Check your permissions in the current directory\n");
		return -1;
	}

	testlog("test clog_setlogfile(FILE<\"%s\">)\n", fname);
	clog_setlogfile(test_log_file);
	testlog("OK\n\n");

	testlog("test clog_setfilter(%d)\n", lvl);
	clog_setfilterlevel(lvl);
	testlog("OK\n\n");

	testlog("test clog_getilter() == %d\n", lvl);
	assert(clog_getfilterlevel() == lvl);
	testlog("OK\n\n");

	testlog("test log_getfiltername() == \"%s\"\n", lname);
	assert(strcmp(clog_getfiltername(), lname) == 0);
	testlog("OK\n\n");


	testlog("test debug(\"%s\")\n", msg);
	debug(msg);
	// TODO check file content

	testlog("test blank message\n");
	debug(msg_blank);
	// TODO check file content

	testlog("end tests\n");

	fclose(test_log_file);
	return 0;
}
