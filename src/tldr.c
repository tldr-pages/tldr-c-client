/*
 * tldr.c - main routines
 *
 * Copyright (C) 2016 Arvid Gerstmann
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include "tldr.h"
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define VERSION_TAG "v1.5.0"
#ifndef VERSION
    #define VERSION_PRETTY ""
#else
    #define VERSION_PRETTY VERSION
#endif

/* Help and usage */
void        print_version           (char const *arg);
void        print_usage             (char const *arg);

/* getopt */
static int help_flag;
static int version_flag;
static int verbose_flag;
static int update_flag;
static int clear_flag;
static int platform_flag;
static int list_flag;
static int render_flag;
static char pbuf[STRBUFSIZ];
static struct option long_options[] = {
    { "help", no_argument, &help_flag, 1 },
    { "version", no_argument, &version_flag, 1 },
    { "verbose", no_argument, &verbose_flag, 1 },

    { "update", no_argument, &update_flag, 1 },
    { "clear-cache", no_argument, &clear_flag, 1 },
    { "platform", required_argument, 0, 'p' },
    { "linux",  no_argument, 0, 'p' },
    { "osx",  no_argument, 0, 'p' },
    { "sunos",  no_argument, 0, 'p' },
    { "list", no_argument, &list_flag, 'l'},
    { "render", required_argument, 0, 'r'}, {0, 0, 0, 0 }
};

int
main(int argc, char **argv)
{
    int c;
    int missing_arg;
    int option_index;

    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    while (1) {
        option_index = 0;
        c = getopt_long_only(argc, argv, "v", long_options, &option_index);

        /* reached the end, bail out */
        if (c == -1) {
            break;
        }

        switch (c) {
        case 0:
            break;

        case 'v':
            verbose_flag = 1;
            break;

        case '?':
            /* do not set help flag, only show getopt error */
            /* help_flag = 1; */
            break;

        case 'p': {
            const char* platform_name = long_options[option_index].name;
            if (strcmp(platform_name, "platform") == 0) {
                size_t len = strlen(optarg);
                if (len > STRBUFSIZ)
                    exit(EXIT_FAILURE);

                memcpy(pbuf, optarg, len);
                pbuf[len] = '\0';
            } else {
                memcpy(pbuf, platform_name, strlen(platform_name));
            }
            platform_flag = 1;
        } break;

        case 'r': {
            size_t len = strlen(optarg);
            if (len > STRBUFSIZ)
                exit(EXIT_FAILURE);

            memcpy(pbuf, optarg, len);
            pbuf[len] = '\0';
            render_flag = 1;
        } break;

        default:
            abort();
        }
    }

    if (!update_flag) {
        check_localdate();
    }

    /* show help, if platform was supplied, but no further argument */
    missing_arg = (platform_flag && !list_flag && (optind == argc));
    if (help_flag || missing_arg) {
        print_usage(argv[0]);
        return EXIT_SUCCESS;
    }
    if (version_flag) {
        print_version(argv[0]);
        return EXIT_SUCCESS;
    }
    if (update_flag) {
        if (update_localdb(verbose_flag))
            return EXIT_FAILURE;
        return EXIT_SUCCESS;
    }
    if (clear_flag) {
        if (clear_localdb(verbose_flag))
            return EXIT_FAILURE;
        return EXIT_SUCCESS;
    }
    if (verbose_flag && optind >= argc) {
        print_version(argv[0]);
        return EXIT_SUCCESS;
    }
    if (list_flag) {
        if (!has_localdb())
            update_localdb(verbose_flag);

        if (print_tldrlist(pbuf[0] != 0 ? pbuf : NULL))
            return EXIT_FAILURE;
        return EXIT_SUCCESS;
    }
    if (render_flag) {
        if (print_localpage(pbuf))
            return EXIT_FAILURE;
        return EXIT_SUCCESS;
    }

    if (optind < argc) {
        size_t len, sum;
        char buf[4096];

        sum = 0;
        while (optind < argc) {
            len = strlen(argv[optind]);
            if (sum+len >= 4096)
                exit(EXIT_FAILURE);
            memcpy(buf + sum, argv[optind], len);
            memcpy(buf + sum + len, "-", 1);
            sum += len + 1;
            optind++;
        }

        buf[sum - 1] = '\0';

        if (!has_localdb())
            update_localdb(verbose_flag);
        if (print_tldrpage(buf, pbuf[0] != 0 ? pbuf : NULL)) {
            fprintf(stdout, "This page doesn't exist yet!\n");
            fprintf(stdout, "Submit new pages here: https://github.com/tldr-pages/tldr\n");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

void
print_version(char const *arg)
{
    /* *INDENT-OFF* */
    if (strcmp("", VERSION_PRETTY) == 0)
       fprintf(stdout, "%s %s\n", arg, VERSION_TAG);
    else
       fprintf(stdout, "%s %s (%s)\n", arg, VERSION_TAG, VERSION_PRETTY);;
    fprintf(stdout, "Copyright (C) 2016 Arvid Gerstmann\n");
    fprintf(stdout, "Source available at https://github.com/tldr-pages/tldr-c-client\n");
    /* *INDENT-ON* */
}

void
print_usage(char const *arg)
{
    char const *out = "usage: %s [-v] [OPTION]... SEARCH\n\n";

    /* *INDENT-OFF* */
    fprintf(stdout, out, arg);
    fprintf(stdout, "available commands:\n");
    fprintf(stdout, "    %-20s %-30s\n", "-v", "print verbose output");
    fprintf(stdout, "    %-20s %-30s\n", "--version", "print version and exit");
    fprintf(stdout, "    %-20s %-30s\n", "-h, --help", "print this help and exit");
    fprintf(stdout, "    %-20s %-30s\n", "-u, --update", "update local database");
    fprintf(stdout, "    %-20s %-30s\n", "-c, --clear-cache", "clear local database");
    fprintf(stdout, "    %-20s %-30s\n", "-l, --list", "list all entries in the local database");
    fprintf(stdout, "    %-20s %-30s\n", "-p, --platform=PLATFORM",
            "select platform, supported are linux / osx / sunos / windows / common");
    fprintf(stdout, "    %-20s %-30s\n", "--linux", "show command page for Linux");
    fprintf(stdout, "    %-20s %-30s\n", "--osx", "show command page for OSX");
    fprintf(stdout, "    %-20s %-30s\n", "--sunos", "show command page for SunOS");
    fprintf(stdout, "    %-20s %-30s\n", "-r, --render=PATH",
            "render a local page for testing purposes");
    /* *INDENT-ON* */
}

