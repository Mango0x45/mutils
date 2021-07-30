#define _GNU_SOURCE
#include <sys/types.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *argv0;

static void
usage(void)
{
	fprintf(stderr, "%s: Missing oprand\n", argv0);
	exit(EXIT_FAILURE);
}

static void
pp(const char *text)
{
	char *line = NULL;
	size_t n = 0;
	ssize_t len;

	while ((len = getline(&line, &n, stdin)) != -1) {
#ifdef APPEND
		char end;
		if (line[--len] == '\n') {
			line[len] = '\0';
			end = '\n';
		}
		else
			end = '\0';
		printf("%s%s%c", line, text, end);
#else
		printf("%s%s", text, line);
#endif
	}

	if (ferror(stdin)) {
		fprintf(stderr, "%s: getline: %s\n", argv0, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

int
main(int argc, char **argv)
{
	argv0 = argv[0];
	if (argc == 1)
		usage();

	/* Concatinate all of argv into one string */
	for (int i = 2; i < argc; i++)
		*(argv[i] - 1) = ' ';

	pp(argv[1]);
	return EXIT_SUCCESS;
}
