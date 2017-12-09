#include <assert.h>
#include <stdio.h>
#include <string.h>


#include "log.h"

#define testlog(fmt, args...) fprintf(stderr, (fmt"\n"), ##args)

int main(void) {

	const char *const fname = "test.log";

	FILE *test_log_file = fopen(fname, "w");
	if(test_log_file == NULL) { // just in case
		fprintf(stderr, "Unable to create file \"%s\"\n", fname);
		fprintf(stderr, "Check your permissions in the current directory\n");
		return -1;
	}

	const LogLevel lvl = LOG_DEBUG;
	const char *const lname = "DEBUG";

	const char *const hdr = "Test header log";

	const char *const tfm = "%d/%m/%Y";

	const char *const msg = "Test message log";

	testlog("test log_setlogfile(FILE<\"%s\">)", fname);
	log_setlogfile(test_log_file);
	testlog("OK\n");

	testlog("test log_setfilter(%d)", lvl);
	log_setfilter(lvl);
	testlog("OK\n");

	testlog("test log_getilter() == %d", lvl);
	assert(log_getfilter() == lvl);
	testlog("OK\n");

	testlog("test log_getfiltername() == \"%s\"", lname);
	assert(strcmp(log_getfiltername(), lname) == 0);
	testlog("OK\n");

	testlog("test log_setheader(NULL)");
	log_setheader(NULL);
	testlog("OK\n");

	testlog("test log_setheader(\"%s\")", hdr);
	log_setheader(hdr);
	testlog("OK\n");

	testlog("test log_settimefmt(\"%s\")", tfm);
	log_settimefmt(tfm);
	testlog("OK\n");

	testlog("test log_gettimefmt() == \"%s\"", tfm);
	assert(strcmp(log_gettimefmt(), tfm) == 0);
	testlog("OK\n");

	testlog("test debug(\"%s\")", msg);
	debug(msg);
	// TODO

	testlog("end tests");

	fclose(test_log_file);
	return 0;
}
