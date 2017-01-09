/*
 * local.c - local database maintenance
 *
 * Copyright (C) 2016 Arvid Gerstmann
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include "tldr.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <errno.h>
#include <unistd.h>

long
check_localdate(void)
{
    FILE *fp;
    char const *homedir;
    char outdir[STRBUFSIZ], buffer[STRBUFSIZ];
    size_t curlen, read, len;
    time_t oldtime, curtime, difftime;

    homedir = gethome();
    if (homedir == NULL)
        return -1;

    curlen = 0;
    if (sstrncat(outdir, &curlen, STRBUFSIZ, homedir, strlen(homedir)))
        return -1;
    if (sstrncat(outdir, &curlen, STRBUFSIZ, TLDR_DATE, TLDR_DATE_LEN))
        return -1;

    fp = fopen(outdir, "rb");
    if (!fp)
        return -1;

    if (fseek(fp, 0, SEEK_END))
        goto error;
    if ((len = (size_t)ftell(fp)) == (size_t)EOF)
        goto error;
    if (fseek(fp, 0, SEEK_SET))
        goto error;

    read = fread(buffer, 1, len, fp);
    if (read != len)
        goto error;

    curtime = time(NULL);
    oldtime = strtol(buffer, NULL, 10);
    difftime = curtime - oldtime;
    if (difftime > (60 * 60 * 24 * 7 * 2)) {
        /* *INDENT-OFF* */
        fprintf(stdout, "%s", ANSI_BOLD_ON);
        fprintf(stdout, "%s", ANSI_COLOR_CODE_FG);
        fprintf(stdout, "Local data is older than two weeks, use --update to update it.\n\n");
        fprintf(stdout, "%s", ANSI_COLOR_RESET_FG);
        fprintf(stdout, "%s", ANSI_BOLD_OFF);
        /* *INDENT-ON* */
    }

    fclose(fp);
    return difftime;

error:
    fclose(fp);
    return -1;
}

int
update_localdate(void)
{
    FILE *fp;
    size_t len;
    char const *homedir;
    char outdir[STRBUFSIZ];
    size_t written;
    char timestr[32];
    time_t curtime;

    homedir = gethome();
    if (homedir == NULL)
        return 1;

    len = 0;
    if (sstrncat(outdir, &len, STRBUFSIZ, homedir, strlen(homedir)))
        return 1;
    if (sstrncat(outdir, &len, STRBUFSIZ, TLDR_DATE, TLDR_DATE_LEN))
        return 1;

    fp = fopen(outdir, "w");
    if (!fp)
        return 1;

    curtime = time(NULL);
    sprintf(timestr, "%ld", curtime);
    written = fwrite(timestr, sizeof(char), strlen(timestr), fp);
    if (written < strlen(timestr))
        goto error;

    fclose(fp);
    return 0;

error:
    fclose(fp);
    return 1;
}

int
has_localdb(void)
{
    struct stat s;
    size_t len;
    char const *homedir;
    char outhome[STRBUFSIZ];

    homedir = gethome();
    if (homedir == NULL)
        return 0;

    len = 0;
    if (sstrncat(outhome, &len, STRBUFSIZ, homedir, strlen(homedir)))
        return 0;
    if (sstrncat(outhome, &len, STRBUFSIZ, TLDR_HOME, TLDR_HOME_LEN))
        return 0;

    if ((stat(outhome, &s) == 0) && (S_ISDIR(s.st_mode)))
        return 1;

    return 0;
}

int
update_localdb(int verbose)
{
    struct stat s;
    char tmp[STRBUFSIZ];
    char outpath[STRBUFSIZ];
    char outfile[STRBUFSIZ];
    char outhome[STRBUFSIZ];
    char const *homedir;
    size_t outlen;
    size_t homelen;
    size_t tmplen;

    /* get path to ".tldrc" (outhome) */
    homedir = gethome();
    if (homedir == NULL) {
        fprintf(stderr, "Error: HOME not existant\n");
        return 1;
    }

    outlen = 0;
    if (sstrncat(outhome, &outlen, STRBUFSIZ, homedir, strlen(homedir)))
        return 1;
    if (sstrncat(outhome, &outlen, STRBUFSIZ, TLDR_HOME, TLDR_HOME_LEN))
        return 1;
    homelen = outlen;

    /* create it if it doesn't exist */
    if (mkdir(outhome, 0755) > 0 && errno != EEXIST) {
        fprintf(stderr, "Error: Could Not Create Directory: %s\n", outhome);
        return 1;
    }

    /* create tmp directory (outpath) in outhome */
    outlen = 0;
    if (sstrncat(outpath, &outlen, STRBUFSIZ, outhome, homelen))
        return 1;
    if (sstrncat(outpath, &outlen, STRBUFSIZ, TMP_DIR, TMP_DIR_LEN))
        return 1;
    tmplen = homelen + TMP_DIR_LEN;
    if (mkdtemp(outpath) == NULL) {
        fprintf(stderr, "Error: Creating Directory: %s\n", outpath);
        return 1;
    }

    /* downlaod content to outfile in tmp directory (outpath) */
    outlen = 0;
    if (sstrncat(outfile, &outlen, STRBUFSIZ, outpath, tmplen))
        return 1;
    if (sstrncat(outfile, &outlen, STRBUFSIZ, TMP_FILE, TMP_FILE_LEN))
        return 1;

    if (download_file(ZIP_URL, outfile, verbose)) {
        fprintf(stderr, "Error: Downloading File: %s\n", ZIP_URL);
        if (rm(outpath, 0))
            fprintf(stderr, "Error: Could Not Remove: %s\n", outpath);

        return 1;
    }

    /* unzip content (tmp) and move it to outhome */
    if (unzip(outfile, outpath)) {
        if (rm(outpath, 0))
            fprintf(stderr, "Error: Could Not Remove: %s\n", outpath);

        return 1;
    }

    if (sstrncat(outhome, &homelen, STRBUFSIZ, TLDR_DIR, TLDR_DIR_LEN))
        return 1;
    if (sstrncat(outhome, &homelen, STRBUFSIZ, "/", 1))
        return 1;

    if ((stat(outhome, &s) == 0) && (S_ISDIR(s.st_mode))) {
        if (rm(outhome, 0)) {
            fprintf(stderr, "Error: Could Not Remove: %s\n", outhome);
            return 1;
        }
    }

    outlen = 0;
    if (sstrncat(tmp, &outlen, STRBUFSIZ, outpath, tmplen))
        return 1;
    if (sstrncat(tmp, &outlen, STRBUFSIZ, TLDR_DIR, TLDR_DIR_LEN))
        return 1;

    if (rename(tmp, outhome)) {
        fprintf(stderr, "Error: Could Not Rename: %s to %s\n", tmp, outhome);
        if (rm(outpath, 0))
            fprintf(stderr, "Error: Could Not Remove: %s\n", outpath);

        return 1;
    }

    /* clean up */
    if (rm(outpath, 0)) {
        fprintf(stderr, "Error: Could Not Remove: %s\n", outpath);
        return 1;
    }

    if (update_localdate()) {
        fprintf(stderr, "Error: Could not update last updated date\n");
        return 1;
    }

    fprintf(stdout, "Successfully updated local database\n");
    return 0;
}

int
clear_localdb(int verbose)
{
    size_t len;
    char tmp[STRBUFSIZ];
    char const *homedir;

    homedir = gethome();
    if (homedir == NULL)
        return 1;

    len = 0;
    if (sstrncat(tmp, &len, STRBUFSIZ, homedir, strlen(homedir)))
        return 1;
    if (sstrncat(tmp, &len, STRBUFSIZ, TLDR_HOME, TLDR_HOME_LEN))
        return 1;

    if (rm(tmp, RMOPT_IGNORE_NOFILE))
        return 1;

    if (verbose) {
        fprintf(stdout, "Successfully removed %s\n", tmp);
    } else {
        fprintf(stdout, "Successfully removed local database\n");
    }

    return 0;
}

int
get_file_content(char const *path, char **out, int verbose)
{
    size_t len, read;
    FILE *fp;

    ((void)verbose);
    *out = NULL;
    fp = fopen(path, "rb");
    if (!fp)
        return 1;

    if (fseek(fp, 0, SEEK_END))
        goto error;
    if ((len = (size_t)ftell(fp)) == (size_t)EOF)
        goto error;
    if (fseek(fp, 0, SEEK_SET))
        goto error;

    *out = (char *)malloc(len);
    if (*out == NULL)
        goto error;

    read = fread(*out, 1, len, fp);
    if (read != len)
        goto error;

    fclose(fp);
    return 0;

error:
    fclose(fp);
    free(*out);
    *out = NULL;
    return 1;
}

