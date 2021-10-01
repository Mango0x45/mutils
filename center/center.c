#define _GNU_SOURCE
#include <sys/ioctl.h>
#include <sys/types.h>

#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <unistd.h>

#define ESCSEQ_REGEX "\033\\[[;0123456789]*[A-Z]"

const char *argv0;

static noreturn void
die(const char *s)
{
	fprintf(stderr, "%s: %s: %s\n", argv0, s, strerror(errno));
	exit(EXIT_FAILURE);
}

static int
get_cols(void)
{
	int cols;
	if (isatty(STDOUT_FILENO)) {
		struct winsize w;
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
			die("ioctl");
		cols = w.ws_col;
	}
	else {
		FILE *fp = popen("tput cols", "r");
		if (!fp)
			die("popen");

		fscanf(fp, "%d", &cols);
		(void) pclose(fp);
	}
	return cols;
}

static size_t
strlen_no_escapes(char *line, regex_t *re)
{
	int ret, eflags = 0;
	size_t len = 0, offset = 0;
	regmatch_t whole_match;

	while ((ret = regexec(re, line + offset, 1, &whole_match, eflags)) == 0) {
		offset += whole_match.rm_eo;
		eflags = REG_NOTBOL;

		len += whole_match.rm_eo - whole_match.rm_so;
	}

	return len;
}

static void
center(FILE *fp, const int col, regex_t *re)
{
	char *line = NULL;
	size_t n = 0;
	ssize_t olen;
	while ((olen = getline(&line, &n, fp)) > 0) {
		/* 1 tab = 8 spaces */
		int tabs = 0;
		char *match = line;
		while (strchr(match, '\t')) {
			match++;
			tabs++;
		}

		int extra = tabs * 8 - tabs;
		olen += extra;
		size_t len = olen - strlen_no_escapes(line, re) + extra;
		int pad = (len + col + (olen - len) * 2) / 2 + 1;

		printf("%*s", pad, line);
	}

	if (ferror(fp))
		die("getline");
}

int
main(int argc, char **argv)
{
	argv0 = argv[0];

	regex_t re;
	if (regcomp(&re, ESCSEQ_REGEX, REG_EXTENDED | REG_ICASE) != 0)
		die("regcomp");

	int cols = get_cols();
	if (argc == 1)
		center(stdin, cols, &re);
	else while (*++argv) {
		if (!strcmp(*argv, "-"))
			center(stdin, cols, &re);
		else {
			FILE *fp = fopen(*argv, "r");
			if (!fp)
				die("fopen");
			center(fp, cols, &re);
			(void) fclose(fp);
		}
	}

	return EXIT_SUCCESS;
}
