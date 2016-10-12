#include <stdio.h>
#include <regex.h>
#include <getopt.h>
#include <stdlib.h>
#define IMPORT_FROM_AQUA
#include <aqua.h>

int main(int argc, char *argv[])
{
    int c;
    int eflag = 0;
    char *str;
    regex_t compiled;
    int cflags = 0;
    int errcode;
    string s;
    int hasterm;
    int eflags = 0;
    int match = 0;
    regmatch_t *matchptr = NULL;
    int nmatch = 0;

    while ((c = getopt(argc, argv, "e")) != -1) {
        switch (c) {
            case 'e':
                eflag = 1;
                break;
            case '?':
                fprintf(stderr, "Unknown option: %c\n", c);
                break;
            default:
                fprintf(stderr, "Unexpected error processing options.\n");
                exit(1);
                break;
        }
    }

    if (optind < argc - 2 || optind > argc - 1) {
        fprintf(stderr, "Usage: regex [-e] <regex> [<nmatch>]\n");
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
 
    errcode = regcomp(&compiled, str, cflags);
    if (errcode) {
        size_t length;
        char *error;
        length = regerror(errcode, &compiled, NULL, 0);
        error = malloc(length);
        regerror(errcode, &compiled, error, length);
        fprintf(stderr, "Compile error: %s\n", error);
        free(error);
        return 1;
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
            fprintf(stderr, "Error: %s\n", error);
            sdestroy(s);
            if (matchptr)
                free(matchptr);
            break;
        }
        printf("match = %d\n", match);
        if (match) {
            for (int i = 0; i < nmatch; i++) {
                printf("so = %d\n", matchptr[i].rm_so);
                printf("eo = %d\n", matchptr[i].rm_eo);
            }
        }
        if (nmatch > 0)
            free(matchptr);
    }
    return 0;
}