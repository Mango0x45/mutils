#define _GNU_SOURCE
#include <sys/types.h>

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef APPEND
#	define PROGNAME "ap"
#else
#	define PROGNAME "pp"
#endif

static void
usage(void)
{
	fputs(PROGNAME ": missing oprand\n", stderr);
	exit(EXIT_FAILURE);
}

static void
pp(const char *text)
{
#ifdef APPEND
	char end;
#endif
	char *line = NULL;
	size_t n = 0;
	ssize_t len;

	while ((len = getline(&line, &n, stdin)) != -1) {
#ifdef APPEND
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
		perror(PROGNAME);
		exit(EXIT_FAILURE);
	}
}

int
main(int argc, char **argv)
{
	int i;
	if (argc == 1)
		usage();

	setlocale(LC_CTYPE, "");

	/* Concatinate all of argv into one string */
	for (i = 2; i < argc; i++)
		*(argv[i] - 1) = ' ';

	pp(argv[1]);
	return EXIT_SUCCESS;
}
