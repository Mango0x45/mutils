%{
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

#include <gmp.h>

#define CLEANUP                                                                                    \
	do {                                                                                       \
		print_bignum(0);                                                                   \
		yyterminate();                                                                     \
	} while (0)

#define EATSPACE                                                                                   \
	while (isspace(*yytext))                                                                   \
		putchar(*yytext++);

#define EATPUNCT                                                                                   \
	while (ispunct(*yytext) || isspace(*yytext))                                               \
		putchar(*yytext++);

int to_base = 10;
const char *argv0;

void print_bignum(int base);
%}

%option noyywrap
%option nodefault
%option case-insensitive

%x PFLAG
%x WFLAG

spc	[ \t]+
pnt	[ \t[:punct:]]+
end	{spc}|\n
pnd	{pnt}|\n

hex	0x[0-9a-f]+
dec	  [0-9]+
oct	0o[0-7]+
bin	0b[01]+

%%

	/* Raw numbers */
{hex}	print_bignum(16);
{dec}	print_bignum(10);
{oct}	print_bignum(8);
{bin}	print_bignum(2);

	/* Anchored numbers with -p */
<PFLAG>^{hex}/{pnd}	print_bignum(16);
<PFLAG>^{dec}/{pnd}	print_bignum(10);
<PFLAG>^{oct}/{pnd}	print_bignum(8);
<PFLAG>^{bin}/{pnd}	print_bignum(2);

	/* Anchored numbers with -w */
<WFLAG>^{hex}/{end}	print_bignum(16);
<WFLAG>^{dec}/{end}	print_bignum(10);
<WFLAG>^{oct}/{end}	print_bignum(8);
<WFLAG>^{bin}/{end}	print_bignum(2);

	/* Independent words with -p */
<PFLAG>{pnt}{hex}/{pnd}	{ EATPUNCT; print_bignum(16); }
<PFLAG>{pnt}{dec}/{pnd}	{ EATPUNCT; print_bignum(10); }
<PFLAG>{pnt}{oct}/{pnd}	{ EATPUNCT; print_bignum(8); }
<PFLAG>{pnt}{bin}/{pnd}	{ EATPUNCT; print_bignum(2); }

	/* Independent words with -w */
<WFLAG>{spc}{hex}/{end}	{ EATSPACE; print_bignum(16); }
<WFLAG>{spc}{dec}/{end}	{ EATSPACE; print_bignum(10); }
<WFLAG>{spc}{oct}/{end}	{ EATSPACE; print_bignum(8); }
<WFLAG>{spc}{bin}/{end}	{ EATSPACE; print_bignum(2); }

	/* Attempt to match as much non-converted text as possible for performance reasons */
<*>(?x:
0x[^0-9a-f\n]+	|
0o[^0-7\n]+	|
0b[^01\n]+	|
.|\n		|
((0[^xob\n]|[^0\n])[^0\n]*)*[^0 \t\n[:punct:]]
)	ECHO;

<*><<EOF>>	CLEANUP;

%%

static noreturn void
usage(void)
{
	fprintf(stderr, "Usage: %s [-b base] [-pw] [file...]\n", argv0);
	exit(EXIT_FAILURE);
}

void
print_bignum(int base)
{
	static mpz_t i;
	if (!base)
		return;

	/* Skip over the 0[xob] prefix */
	if (base != 10)
		yytext += 2;
	mpz_set_str(i, yytext, base);
	mpz_out_str(stdout, to_base, i);
}

int
main(int argc, char **argv)
{
	argv0 = argv[0];

	int opt;
	while ((opt = getopt(argc, argv, ":b:pw")) != -1) {
		switch (opt) {
		case 'b':
			to_base = atoi(optarg);
			break;
		case 'p':
			BEGIN(PFLAG);
			break;
		case 'w':
			BEGIN(WFLAG);
			break;
		default:
			usage();
		}
	}

	argc -= optind;
	argv += optind;

	if (!argc)
		yylex();
	else do {
		if (strcmp(*argv, "-") == 0) {
			yyrestart(stdin);
			yylex();
		}
		else {
			FILE *fp = fopen(*argv, "r");
			if (!fp) {
				perror(*argv);
				exit(EXIT_FAILURE);
			}

			yyrestart(fp);
			yylex();
			(void) fclose(fp);
		}
	} while (*++argv);

	return EXIT_SUCCESS;
}
