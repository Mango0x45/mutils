%{
#define _XOPEN_SOURCE
#include <stdio.h>
#include <string.h>
#include <time.h>

char *name;
struct tm *today;

/* The compiler complains about this missing */
char *strptime(const char *, const char *, struct tm *);
void print_bday(struct tm);
%}

%option noyywrap
%option nodefault

spc	[[:space:]]*

%%

	/* Trailing context ensures that the regex matches */
name{spc}={spc}[^ ]*/.*		name = strdup(yytext + 5);

birthday{spc}={spc}[0-9]{4}.*	{ struct tm date; strptime(yytext + 9, "%Y-%m-%d", &date);
					print_bday(date); }
birthday{spc}={spc}--.*		{ struct tm date; strptime(yytext + 11, "%m-%d", &date);
					date.tm_year = 0; print_bday(date); }

^\n			free(name);
.*|\n	;

%%

void
print_bday(struct tm date)
{
	if (date.tm_mon == today->tm_mon && date.tm_mday == today->tm_mday) {
		char *s = name;
		while (*++s) {} /* Get to the end of name */
		char *name_suf = (s[-1] == 's') ? "'" : "'s";

		if (date.tm_year) {
			int age = today->tm_year - date.tm_year;
			char *age_suf;

			switch (age % 10) {
			case 1:
				age_suf = "st";
				break;
			case 2:
				age_suf = "nd";
				break;
			case 3:
				age_suf = "rd";
				break;
			default:
				age_suf = "th";
				break;
			}

			printf("It's %s%s %d%s birthday today!\n", name, name_suf, age, age_suf);
		}
		else
			printf("It's %s%s birthday today!\n", name, name_suf);
	}
}

int
main(int argc, char **argv)
{
	time_t t = time(NULL);
	today = localtime(&t);

	if (argc == 1)
		yylex();
	else while (*++argv) {
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
	}

	return EXIT_SUCCESS;
}