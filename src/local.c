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
    char tldr_home[STRBUFSIZ];         /* $HOME/TLDR_HOME                      */
    char tldr_home_db[STRBUFSIZ];      /* $HOME/TLDR_HOME/TLDR_DIR             */
    char temp_dir[STRBUFSIZ];          /* $HOME/TLDR_HOME/TMP_DIR              */
    char update_zip_file[STRBUFSIZ];   /* $HOME/TLDR_HOME/TMP_DIR/TMP_FILE     */
    char update_contents[STRBUFSIZ];   /* $HOME/TLDR_HOME/TMP_DIR/TLDR_ZIP_DIR */
    char const *homedir;
    size_t outlen;

    homedir = gethome();
    if (homedir == NULL) {
        fprintf(stderr, "Error: HOME not existent\n");
        return 1;
    }

    /* Create TLDR_HOME if non-existent */
    outlen = 0;
    if (sstrncat(tldr_home, &outlen, STRBUFSIZ, homedir, strlen(homedir))) {
        return 1;
    }
    if (sstrncat(tldr_home, &outlen, STRBUFSIZ, TLDR_HOME, TLDR_HOME_LEN)) {
        return 1;
    }
    if (mkdir(tldr_home, 0755) > 0 && errno != EEXIST) {
        fprintf(stderr, "Error: Could not create directory: %s\n", tldr_home);
        return 1;
    }

    /* Set up the temp directory */
    outlen = 0;
    if (sstrncat(temp_dir, &outlen, STRBUFSIZ, tldr_home, strlen(tldr_home))) {
        return 1;
    }
    if (sstrncat(temp_dir, &outlen, STRBUFSIZ, TMP_DIR, TMP_DIR_LEN)) {
        return 1;
    }
    if (mkdir(temp_dir, 0755) > 0 && errno != EEXIST) {
        fprintf(stderr, "Error: Could not create directory: %s\n", temp_dir);
        return 1;
    }

    /* The update zip file will be downloaded to $HOME/TLDR_HOME/TMP_DIR/TMP_FILE */
    outlen = 0;
    if (sstrncat(update_zip_file, &outlen, STRBUFSIZ, temp_dir, strlen(temp_dir))) {
        return 1;
    }
    if (sstrncat(update_zip_file, &outlen, STRBUFSIZ, TMP_FILE, TMP_FILE_LEN)) {
        return 1;
    }

    /* Download and unzip the file */
    if (download_file(ZIP_URL, update_zip_file, verbose)) {
        fprintf(stderr, "Error: Downloading file: %s\n", ZIP_URL);
        return 1;
    }

    if (unzip(update_zip_file, temp_dir)) {
        rm(temp_dir, 0);
        return 1;
    }

    outlen = 0;
    if (sstrncat(update_contents, &outlen, STRBUFSIZ, temp_dir, strlen(temp_dir))) {
        return 1;
    }
    if (sstrncat(update_contents, &outlen, STRBUFSIZ, TLDR_ZIP_DIR, TLDR_ZIP_DIR_LEN)) {
        return 1;
    }

    /* tldr_home_db is where we want to move the update contents */
    outlen = 0;
    if (sstrncat(tldr_home_db, &outlen, STRBUFSIZ, tldr_home, strlen(tldr_home))) {
        return 1;
    }
    if (sstrncat(tldr_home_db, &outlen, STRBUFSIZ, TLDR_DIR, TLDR_HOME_LEN)) {
        return 1;
    }
    if (sstrncat(tldr_home_db, &outlen, STRBUFSIZ, "/", 1)) {
        return 1;
    }

    /* Remove the old database */
    if ((stat(tldr_home_db, &s) == 0) && (S_ISDIR(s.st_mode))) {
        if (rm(tldr_home_db, 0)) {
            fprintf(stderr, "Error: Could not remove the old database: %s\n", tldr_home_db);
            return 1;
        }
    }

    if (rename(update_contents, tldr_home_db)) {
        fprintf(stderr, "Error: Could not rename: %s to %s\n", update_contents, tldr_home_db);
        rm(temp_dir, 0);
        return 1;
    }

    if (rm(temp_dir, 0)) {
        fprintf(stderr, "Error: Could not remove: %s\n", temp_dir);
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

