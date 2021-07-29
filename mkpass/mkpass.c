#include <sys/param.h>

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <unistd.h>

#define CTOI(x) ((x) ^ 48)

#define ANCHARS 62
#define PCHARS  94

#define ALPHANUM "0123456789abcdefghijkmnlopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define SYMBOLS  "`~!@#$%^&*()-_=+[{]}\\|;:'\",<.>/?"

const char alphanum[] = ALPHANUM;
const char printable[] = ALPHANUM SYMBOLS;

const char *charset;
unsigned int chars;

const char *argv0;
bool aflag, nflag;

static noreturn void
usage(void)
{
	fprintf(stderr, "Usage: %s [-m] [-c count] [-an] [length]\n", argv0);
	exit(EXIT_FAILURE);
}

static noreturn void
die(const char *s)
{
	fprintf(stderr, "%s: %s: %s\n", argv0, s, strerror(errno));
	exit(EXIT_FAILURE);
}

static unsigned long long
atoull(const char *s)
{
	unsigned long long ret = 0;
	for (; *s; s++) {
		if (!isdigit(*s))
			usage();
		ret = ret * 10 + CTOI(*s);
	}
	return ret;
}

static void
mkpass(unsigned long long len)
{
#ifdef BSD
	for (unsigned long long i = 0; i < len; i++)
		putchar(charset[arc4random_uniform(chars)]);
#else
	FILE *fp = fopen("/dev/urandom", "r");
	if (!fp)
		die("fopen");

	for (unsigned long long i = 0; i < len; i++) {
		int c = fgetc(fp);
		putchar(charset[c % chars]);
	}

	fclose(fp);
#endif
}

int
main(int argc, char **argv)
{
	argv0 = argv[0];
	unsigned long long count = 1;

	int opt;
	while ((opt = getopt(argc, argv, ":ac:mn")) != -1) {
		switch (opt) {
		case 'a':
			aflag = true;
			break;
		case 'c':
			count = atoull(optarg);
			break;
		case 'm':
			printf("%llu\n", ULLONG_MAX);
			return EXIT_SUCCESS;
		case 'n':
			nflag = true;
			break;
		default:
			usage();
		}
	}

	argc -= optind;
	argv += optind;

	unsigned long long len;
	switch (argc) {
	case 0:
		len = 50;
		break;
	case 1:
		len = atoull(*argv);
		break;
	default:
		usage();
	}

	if (aflag) {
		charset = alphanum;
		chars = ANCHARS;
	}
	else {
		charset = printable;
		chars = PCHARS;
	}

	for (unsigned long long i = 0; i < count; i++) {
		mkpass(len);
		if (!nflag || i != count - 1)
			putchar('\n');
	}
	return EXIT_SUCCESS;
}
