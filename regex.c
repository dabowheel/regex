#include <stdio.h>
#include <regex.h>
#include <getopt.h>
#include <stdlib.h>
#define IMPORT_FROM_AQUA
#include <aqua.h>

#define USAGE "Usage: regex [options] <regex> [<nmatch>]\n"\
    " Test a libc regular expression agaist patterns.\n\n"\
    " nmatch - This is the number of matches to display counting the primary match\n"\
    "  and any sub-expression matches. If the primary pattern does not match then no\n"\
    "  matches are displayed. The default of nmatch is 1.\n\n"\
    " Compile Options:\n"\
    "  -e REG_EXTENDED - Treat the pattern as an extended regular expression, rather\n"\
    "      than as a basic regular expression.\n"\
    "  -i REG_ICASE - Ignore case when matching letters.\n"\
    "  -o REG_NOSUB - Don’t bother storing the contents of the matchptr array.\n"\
    "  -n REG_NEWLINE - Treat a newline in string as dividing string into multiple\n"\
    "      lines, so that ‘$’ can match before the newline and ‘^’ can match after.\n"\
    "      Also, don’t permit ‘.’ to match a newline, and don’t permit ‘[^...]’ to\n"\
    "      match a newline. Otherwise, newline acts like any other ordinary\n"\
    "      character.\n\n"\
    " Exec Options:\n"\
    "  -b REG_NOTBOL - Do not regard the beginning of the specified string as the\n"\
    "      beginning of a line; more generally, don’t make any assumptions about what\n"\
    "      text might precede it.\n"\
    "  -l REG_NOTEOL - Do not regard the end of the specified string as the end of a\n"\
    "      line; more generally, don’t make any assumptions about what text might\n"\
    "      follow it.\n"

void run(char *str, int cflags, int eflags, int nmatch);

int main(int argc, char *argv[])
{
    int c;
    int eflag = 0;
    char *str;
    int nmatch = 1;
    int cflags = 0;
    int eflags = 0;

    while ((c = getopt(argc, argv, "eionbl")) != -1) {
        switch (c) {
            case 'e':
                cflags |= REG_EXTENDED;
                break;
            case 'i':
                cflags |= REG_ICASE;
                break;
            case 'o':
                cflags |= REG_NOSUB;
                break;
            case 'n':
                cflags |= REG_NEWLINE;
                break;
            case 'b':
                eflags |= REG_NOTBOL;
                break;
            case 'l':
                eflags |= REG_NOTEOL;
                break;
            case '?':
                fprintf(stderr, "Unknown option: %c\n", c);
                exit(1);
                break;
            default:
                fprintf(stderr, "Unexpected error processing options.\n");
                exit(1);
                break;
        }
    }

    if (optind < argc - 2 || optind > argc - 1) {
        fprintf(stderr, USAGE);
        return 1;
    }

    str = argv[optind++];
    if (optind == argc - 1) {
        nmatch = strtol(argv[optind], NULL, 10);
        if (nmatch < 0) {
            fprintf(stderr, "nmatch cannot be negative.\n");
            return 1;
        }
    }
    if (eflag)
        cflags |= REG_EXTENDED;

    run(str, cflags, eflags, nmatch);
}

void run(char *str, int cflags, int eflags, int nmatch)
{
    regex_t compiled;
    int errcode;
    string s;
    int hasterm;
    int match = 0;
    regmatch_t *matchptr = NULL;

    errcode = regcomp(&compiled, str, cflags);
    if (errcode) {
        size_t length;
        char *error;
        length = regerror(errcode, &compiled, NULL, 0);
        error = malloc(length);
        regerror(errcode, &compiled, error, length);
        fprintf(stderr, "Compile error: %s\n", error);
        free(error);
        exit(1);
    }

    while ((s = getline(stdin, &hasterm))) {
        if (nmatch > 0) {
            matchptr = malloc(sizeof(regmatch_t) * nmatch);
        }
        errcode = regexec(&compiled, s->data, nmatch, matchptr, eflags);
        match = (errcode == 0);
        if (!match && errcode != REG_NOMATCH) {
            size_t length;
            char *error;
            length = regerror(errcode, &compiled, NULL, 0);
            error = malloc(length);
            regerror(errcode, &compiled, error, length);
            fprintf(stderr, "Exec error: %s\n", error);
            sdestroy(s);
            if (matchptr)
                free(matchptr);
            break;
        }

        printf("match = %d\n", match);
        if (match) {
            char *name;
            for (int i = 0; i < nmatch; i++) {
                if (i == 0)
                    name = "match";
                else
                    name = "submatch";
                printf("%s position = [%d, %d)\n", name, matchptr[i].rm_so, matchptr[i].rm_eo);
            }
        }
        if (nmatch > 0)
            free(matchptr);
    }
}
