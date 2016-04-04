#define _BSD_SOURCE

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <libgen.h>
#include <fts.h>
#include <errno.h>
#include <zip.h>
#include <fcntl.h>
#include <pwd.h>
#include <time.h>

#define VERSION_MAJOR 1
#define VERSION_MINOR 1
#define VERSION_PATCH 0
#define VERSION_PRETTY "v1.1.0"

#if defined(_WIN32)
    #define BINARY O_BINARY
#else
    #define BINARY 0
#endif

#if defined(_WIN32)

#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_MAC

    #endif
#elif __linux__

#elif __unix__

#elif defined(_POSIX_VERSION)

#else
    #error "Unknown Compiler"
#endif

#define BASE_URL "https://raw.github.com/tldr-pages/tldr/master/pages"
#define BASE_URL_LEN (sizeof(BASE_URL) - 1)

#define ZIP_URL "https://github.com/tldr-pages/tldr/archive/master.zip"
#define ZIP_URL_LEN (sizeof(ZIP_URL_LEN) - 1)

#define TMP_DIR "/tmp/tldrXXXXXX"
#define TMP_DIR_LEN (sizeof(TMP_DIR) - 1)

#define TMP_FILE "/master.zip"
#define TMP_FILE_LEN (sizeof(TMP_FILE) - 1)

#define TLDR_DIR "/tldr-master"
#define TLDR_DIR_LEN (sizeof(TLDR_DIR) - 1)

#define TLDR_HOME "/.tldr"
#define TLDR_HOME_LEN (sizeof(TLDR_HOME) - 1)

#define TLDR_EXT "/.tldr/tldr-master/pages/"
#define TLDR_EXT_LEN (sizeof(TLDR_EXT) - 1)


static const char* const ANSI_COLOR_RESET_FG            = "\x1b[39m";
static const char* const ANSI_COLOR_TITLE_FG            = "\x1b[39m";
static const char* const ANSI_COLOR_EXPLANATION_FG      = "\x1b[39m";
static const char* const ANSI_COLOR_COMMENT_FG          = "\x1b[32m";
static const char* const ANSI_COLOR_CODE_FG             = "\x1b[31m";
static const char* const ANSI_COLOR_CODE_PLACEHOLDER_FG = "\x1b[34m";
static const char* const ANSI_BOLD_ON                   = "\x1b[1m";
static const char* const ANSI_BOLD_OFF                  = "\x1b[22m";

/* utility functions */
double _round(double arg);
int _rm(char const* path);
int _unzip(char const* path, char const* outpath);
char const* _gethome(void);

/* Output */
int construct_url(char* buf, size_t buflen, char const* input,
                  char const* platform);
int construct_path(char* buf, size_t buflen, char const* home, char const* input,
                   char const* platform);
int parse_tldrpage(char const* input);
int print_tldrpage(char const* input);

/* Help and usage */
void print_version(char const* arg);
void print_usage(char const* arg);

/* functionality */
int check_localdate(void);
int update_localdate(void);
int update_localdb(int verbose);
int clear_localdb(int verbose);
int get_file_content(char const* path, char** out, int verbose);

/* cURL */
int progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow,
                      curl_off_t ultotal, curl_off_t ulnow);
int download_file(char const* url, char const* outfile, int verbose);
int download_content(char const* url, char** out, int verbose);

/* getopt */
static int help_flag;
static int version_flag;
static int verbose_flag;
static int update_flag;
static int clear_flag;
static struct option long_options[] =
{
    { "help", no_argument, &help_flag, 1 },
    { "version", no_argument, &version_flag, 1 },
    { "verbose", no_argument, &verbose_flag, 1 },

    { "update", no_argument, &update_flag, 1 },
    { "clear-cache", no_argument, &clear_flag, 1 },
    { 0, 0, 0, 0 }
};

int
main(int argc, char** argv)
{
    int c;
    int option_index;

    check_localdate();
    if (argc < 2)
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    while (1)
    {
        option_index = 0;
        c = getopt_long_only(argc, argv, "v", long_options, &option_index);

        /* reached the end, bail out */
        if (c == -1)
        { break; }

        switch (c)
        {
            case 0:
                break;

            case 'v':
                verbose_flag = 1;
                break;

            case '?':
                help_flag = 1;
                break;

            default:
                abort();
        }
    }

    if (help_flag)
    {
        print_usage(argv[0]);
        return EXIT_SUCCESS;
    }
    if (version_flag)
    {
        print_version(argv[0]);
        return EXIT_SUCCESS;
    }
    if (update_flag)
    {
        if (update_localdb(verbose_flag))
        { return EXIT_FAILURE; }
        return EXIT_SUCCESS;
    }
    if (clear_flag)
    {
        if (clear_localdb(verbose_flag))
        { return EXIT_FAILURE; }
        return EXIT_SUCCESS;
    }

    if (optind < argc)
    {
        int len, sum;
        char buf[4096];

        sum = 0;
        while (optind < argc)
        {
            if (sum >= 4096) { exit(EXIT_FAILURE); }

            len = strlen(argv[optind]);
            memcpy(buf + sum, argv[optind], len);
            memcpy(buf + sum + len, "-", 1);
            sum += len + 1;
            optind++;
        }

        buf[sum - 1] = '\0';
        if (print_tldrpage(buf))
        {
            fprintf(stdout, "This page doesn't exist yet!\n");
            fprintf(stdout, "Submit new pages here: https://github.com/tldr-pages/tldr\n");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int
construct_url(char* buf, size_t buflen, char const* input, char const* platform)
{
    size_t baselen;
    size_t delimlen;
    size_t inputlen;

    baselen = strlen(BASE_URL);
    delimlen = strlen(platform);
    inputlen = strlen(input);
    if ((baselen + delimlen + inputlen + 5) > buflen) { return 1; }

    memcpy(buf, BASE_URL, baselen);
    memcpy(buf + baselen, "/", 1);
    memcpy(buf + baselen + 1, platform, delimlen);
    memcpy(buf + baselen + 1 + delimlen, "/", 1);
    memcpy(buf + baselen + 2 + delimlen, input, inputlen);
    memcpy(buf + baselen + 2 + delimlen + inputlen, ".md", 3);
    buf[baselen + 2 + delimlen + inputlen + 3] = '\0';

    return 0;
}

int
construct_path(char* buf, size_t buflen, char const* home, char const* input,
               char const* platform)
{
    size_t homelen;
    size_t inputlen;
    size_t platformlen;

    homelen = strlen(home);
    inputlen = strlen(input);
    platformlen = strlen(platform);
    if ((homelen + inputlen + platformlen + TLDR_EXT_LEN + 4) > buflen)
    { return 1; }

    memcpy(buf, home, homelen);
    memcpy(buf + homelen, TLDR_EXT, TLDR_EXT_LEN);
    memcpy(buf + homelen + TLDR_EXT_LEN, platform, platformlen);
    memcpy(buf + homelen + TLDR_EXT_LEN + platformlen, "/", 1);
    memcpy(buf + homelen + TLDR_EXT_LEN + platformlen + 1, input, inputlen);
    memcpy(buf + homelen + TLDR_EXT_LEN + platformlen + 1 + inputlen, ".md", 3);
    buf[homelen + TLDR_EXT_LEN + platformlen + inputlen + 4] = '\0';

    return 0;
}

int
parse_tldrpage(char const* input)
{
    char c;
    int i, len;
    int start = -1;

    len = strlen(input);

    fprintf(stdout, "\n");
    for (i = 0; i < len; ++i)
    {
        c = input[i];
        if (start == -1)
        {
            switch (c)
            {
                case '>':
                    start = i;
                    fprintf(stdout, "%s", ANSI_COLOR_EXPLANATION_FG);
                    continue;

                case '-':
                    start = i;
                    fprintf(stdout, "%s", ANSI_COLOR_COMMENT_FG);
                    continue;

                case '`':
                    start = i;
                    fprintf(stdout, "%s", ANSI_COLOR_CODE_FG);
                    fprintf(stdout, "    ");
                    continue;

                case '#':
                    start = i;
                    fprintf(stdout, "%s", ANSI_BOLD_ON);
                    fprintf(stdout, "%s", ANSI_COLOR_TITLE_FG);
                    continue;
            }
        }
        else if (start > -1)
        {
            if (input[i] == '{' && input[i + 1] == '{')
            {
                fprintf(stdout, "%.*s", i - (start + 1), input + (start + 1));
                fprintf(stdout, "%s", ANSI_BOLD_OFF);
                fprintf(stdout, "%s", ANSI_COLOR_RESET_FG);
                fprintf(stdout, "%s", ANSI_COLOR_CODE_PLACEHOLDER_FG);

                start = i;
                for (i = i + 1; i < len; i++)
                {
                    if (input[i] == '}' && input[i + 1] == '}')
                    {
                        fprintf(stdout, "%.*s", i - (start + 2), input + (start + 2));
                        fprintf(stdout, "%s", ANSI_COLOR_RESET_FG);
                        fprintf(stdout, "%s", ANSI_COLOR_CODE_FG);
                        start = i + 1;
                        break;
                    }
                }

                continue;
            }
        }

        if (c == '\n' && start > -1)
        {
            if (input[i - 1] == '`')
            { fprintf(stdout, "%.*s", i - (start + 2), input + (start + 1)); }
            else if (input[start] == '-')
            { fprintf(stdout, "\n%.*s", i - (start), input + start); }
            else if (input[start] == '>')
            { fprintf(stdout, "%.*s", i - (start + 2), input + (start + 2)); }
            else
            { fprintf(stdout, "%.*s\n", i - (start + 2), input + (start + 2)); }

            fprintf(stdout, "%s", ANSI_BOLD_OFF);
            fprintf(stdout, "%s", ANSI_COLOR_RESET_FG);
            fprintf(stdout, "\n");
            start = -1;
        }
    }

    return 0;
}

int
print_tldrpage(char const* input)
{
    int islinux;
    int isdarwin;
    char* output;
    char* platform;
    char url[1024];
    struct utsname sys;

    size_t homelen;
    char const* homedir;
    char directory[1024];
    struct stat sb;

    uname(&sys);
    islinux = strcmp(sys.sysname, "Linux") == 0;
    isdarwin = strcmp(sys.sysname, "Darwin") == 0;

    if (islinux) { platform = "linux"; }
    else if (isdarwin) { platform = "osx"; }
    else { platform = "common"; }

    homedir = _gethome();
    if (homedir == NULL) { return 1; }

    homelen = strlen(homedir);
    memcpy(directory, homedir, homelen);
    memcpy(directory + homelen, TLDR_EXT, TLDR_EXT_LEN);
    directory[homelen + TLDR_EXT_LEN] = '\0';

    if (stat(directory, &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        construct_path(url, 1024, homedir, input, platform);
        if (stat(url, &sb) == 0 && S_ISREG(sb.st_mode))
        {
            if (!get_file_content(url, &output, 0))
            {
                parse_tldrpage(output);
                free(output);
                return 0;
            }
        }
        else
        {
            construct_path(url, 1024, homedir, input, "common");
            if (stat(url, &sb) == 0 && S_ISREG(sb.st_mode))
            {
                if (!get_file_content(url, &output, 0))
                {
                    parse_tldrpage(output);
                    free(output);
                    return 0;
                }
            }
        }
    }

    construct_url(url, 1024, input, platform);
    download_content(url, &output, 0);
    if (output == NULL)
    {
        construct_url(url, 1024, input, "common");
        download_content(url, &output, 0);
        if (output == NULL) { return 1; }
    }

    parse_tldrpage(output);

    free(output);
    return 0;
}

void
print_version(char const* arg)
{
    fprintf(stdout, "%s %s\n", arg, VERSION_PRETTY);
    fprintf(stdout, "Copyright (C) 2016 Arvid Gerstmann\n");
    fprintf(stdout, "Source available at https://github.com/tldr-pages/tldr-cpp-client\n");
}

void
print_usage(char const* arg)
{
    char const* out = "usage: %s [-v] [<command>] <search>\n\n";
    fprintf(stdout, out, arg);
    fprintf(stdout, "available commands:\n");
    fprintf(stdout, "    %-15s %-30s\n", "--version", "print version and exit");
    fprintf(stdout, "    %-15s %-30s\n", "--help", "print this help and exit");
    fprintf(stdout, "    %-15s %-30s\n", "--update", "update local database");
    fprintf(stdout, "    %-15s %-30s\n", "--clear-cache", "clear local database");
}

int
check_localdate(void)
{
    FILE* fp;
    size_t homelen;
    char const* homedir;
    size_t extlen;
    char* extpath = "/.tldr/date";
    char outdir[1024];
    char buffer[1024];
    size_t read, len;
    time_t oldtime;
    time_t curtime;
    time_t difftime;

    homedir = _gethome();
    if (homedir == NULL) { return 1; }

    homelen = strlen(homedir);
    extlen = strlen(extpath);
    memcpy(outdir, homedir, homelen);
    memcpy(outdir + homelen, extpath, extlen);
    outdir[homelen + extlen] = '\0';

    fp = fopen(outdir, "rb");
    if (!fp) { return -1; }

    if (fseek(fp, 0, SEEK_END)) { goto error; }
    len = ftell(fp);
    if (fseek(fp, 0, SEEK_SET)) { goto error; }

    read = fread(buffer, 1, len, fp);
    if (read != len) { goto error; }

    curtime = time(NULL);
    oldtime = strtol(buffer, NULL, 10);
    difftime = curtime - oldtime;
    if (difftime > (60 * 60 * 24 * 7 * 2))
    {
        fprintf(stdout, "%s", ANSI_BOLD_ON);
        fprintf(stdout, "%s", ANSI_COLOR_CODE_FG);
        fprintf(stdout,
                "Local data is older than two weeks, use --update to update it.\n\n");
        fprintf(stdout, "%s", ANSI_COLOR_RESET_FG);
        fprintf(stdout, "%s", ANSI_BOLD_OFF);
    }

    return difftime;

error:
    fclose(fp);
    return -1;
}

int
update_localdate(void)
{
    FILE* fp;
    size_t homelen;
    char const* homedir;
    size_t extlen;
    char* extpath = "/.tldr/date";
    char outdir[1024];
    size_t written;
    char timestr[32];
    time_t curtime;

    homedir = _gethome();
    if (homedir == NULL) { return 1; }

    homelen = strlen(homedir);
    extlen = strlen(extpath);
    memcpy(outdir, homedir, homelen);
    memcpy(outdir + homelen, extpath, extlen);
    outdir[homelen + extlen] = '\0';

    fp = fopen(outdir, "w");
    if (!fp) { return 1; }

    curtime = time(NULL);
    sprintf(timestr, "%ld", curtime);
    written = fwrite(timestr, sizeof(char), strlen(timestr), fp);
    if (written < strlen(timestr)) { goto error; }

    fclose(fp);
    return 0;

error:
    fclose(fp);
    return 1;
}

int
update_localdb(int verbose)
{
    struct stat s;
    char tmp[255];
    char outpath[255];
    char outfile[255];
    char outhome[255];
    char const* homedir;
    size_t homedirlen;

    memcpy(outfile, TMP_DIR, TMP_DIR_LEN);
    if (!mkdtemp(outfile)) { return 1; }

    memcpy(outpath, outfile, TMP_DIR_LEN);
    memcpy(outfile + TMP_DIR_LEN, TMP_FILE, TMP_FILE_LEN);
    outfile[TMP_DIR_LEN + TMP_FILE_LEN] = '\0';

    if (download_file(ZIP_URL, outfile, verbose))
    { return 1; }

    if (_unzip(outfile, outpath))
    {
        _rm(outpath);
        return 1;
    }

    memcpy(tmp, outpath, TMP_DIR_LEN);
    memcpy(tmp + TMP_DIR_LEN, TLDR_DIR, TLDR_DIR_LEN);
    tmp[TMP_DIR_LEN + TLDR_DIR_LEN] = '\0';

    homedir = _gethome();
    if (homedir == NULL) { return 1; }

    homedirlen = strlen(homedir);
    memcpy(outhome, homedir, homedirlen);
    memcpy(outhome + homedirlen, TLDR_HOME, TLDR_HOME_LEN);
    outhome[homedirlen + TLDR_HOME_LEN] = '\0';
    if (mkdir(outhome, 0755) > 0)
    {
        if (errno != EEXIST)
        {
            _rm(outpath);
            return 1;
        }
    }

    memcpy(outhome + homedirlen + TLDR_HOME_LEN, TLDR_DIR, TLDR_DIR_LEN);
    outhome[homedirlen + TLDR_HOME_LEN + TLDR_DIR_LEN] = '/';
    outhome[homedirlen + TLDR_HOME_LEN + TLDR_DIR_LEN + 1] = '\0';
    if (stat(outhome, &s) == 0 && S_ISDIR(s.st_mode))
    {
        if (_rm(outhome)) { return 1; }
    }

    if (rename(tmp, outhome))
    {
        _rm(outpath);
        return 1;
    }

    if (_rm(outpath))
    { return 1; }

    update_localdate();
    return 0;
}

int
clear_localdb(int verbose)
{
    char tmp[255];
    char const* homedir;
    size_t homedirlen;

    ((void)verbose);
    homedir = _gethome();
    if (homedir == NULL) { return 1; }

    homedirlen = strlen(homedir);
    memcpy(tmp, homedir, homedirlen);
    memcpy(tmp + homedirlen, TLDR_HOME, TLDR_HOME_LEN);
    tmp[homedirlen + TLDR_HOME_LEN] = '\0';
    if (_rm(tmp)) { return 1; }

    fprintf(stdout, "Successfully removed %s\n", tmp);
    return 0;
}

int
get_file_content(char const* path, char** out, int verbose)
{
    size_t len, read;
    FILE* fp;

    ((void)verbose);
    *out = NULL;
    fp = fopen(path, "rb");
    if (!fp) { return 1; }

    if (fseek(fp, 0, SEEK_END)) { goto error; }
    len = ftell(fp);
    if (fseek(fp, 0, SEEK_SET)) { goto error; }

    *out = malloc(len);
    if (*out == NULL) { goto error; }

    read = fread(*out, 1, len, fp);
    if (read != len) { goto error; }

    fclose(fp);
    return 0;

error:
    fclose(fp);
    free(*out);
    *out = NULL;
    return 1;
}

double
_round(double arg)
{
    double integer;
    double fractional;

    fractional = modf(arg, &integer);
    if (fractional < 0.5)
    {
        return floor(arg);
    }
    else
    {
        return ceil(arg);
    }
}

int
_rm(char const* path)
{
    FTS* tree;
    FTSENT* cur = NULL;
    char* paths[2];
    paths[0] = (char*)path;
    paths[1] = NULL;

    tree = fts_open(paths, FTS_COMFOLLOW | FTS_NOCHDIR, NULL);
    if (tree != NULL)
    {
        while ((cur = fts_read(tree)) != NULL)
        {
            switch (cur->fts_info)
            {
                case FTS_NS:
                case FTS_DNR:
                case FTS_ERR:
                    fprintf(stderr, "Error: %s: error: %s\n",
                            cur->fts_accpath, strerror(cur->fts_errno));
                    return 1;

                case FTS_DC:
                case FTS_DOT:
                case FTS_NSOK:
                    /* Not reached unless FTS_LOGICAL, FTS_SEEDOT, or FTS_NOSTAT were */
                    /* passed to fts_open() */
                    break;

                case FTS_D:
                    /* Do nothing. Need depth-first search, so directories are deleted */
                    /* in FTS_DP */
                    break;

                case FTS_DP:
                case FTS_F:
                case FTS_SL:
                case FTS_SLNONE:
                case FTS_DEFAULT:
                    if (remove(cur->fts_accpath) < 0)
                    {
                        fprintf(stderr, "Error: %s: Failed to remove: %s\n",
                                strerror(errno), cur->fts_path);
                        return 1;
                    }
                    break;
            }
        }

        fts_close(tree);
        return 0;
    }

    return 1;
}

int
_unzip(char const* path, char const* outpath)
{
    int err;
    int i, len;
    size_t filelen;
    struct zip* archive;
    struct zip_file* file;
    struct zip_stat stat;
    int fd;
    size_t sum;
    char buf[4096];
    size_t outlen;
    char tmp[FILENAME_MAX];

    archive = zip_open(path, 0, &err);
    if (!archive) { return 1; }

    outlen = strlen(outpath);
    len = zip_get_num_entries(archive, 0);
    for (i = 0; i < len; i++)
    {
        if (zip_stat_index(archive, i, 0, &stat))
        {
            goto error;
        }

        filelen = strlen(stat.name);
        memcpy(tmp, outpath, outlen);
        memcpy(tmp + outlen, "/", 1);
        memcpy(tmp + outlen + 1, stat.name, filelen);
        tmp[outlen + filelen + 1] = '\0';

        if (tmp[outlen + filelen + 1 - 1] == '/')
        {
            if (mkdir(tmp, 0755))
            {
                fprintf(stderr, "Error: Creating directory: %s\n", tmp);
                goto error;
            }
        }
        else
        {
            file = zip_fopen_index(archive, i, 0);
            if (!file)
            {
                fprintf(stderr, "Error: Opening zip content: %s", tmp);
                goto error;
            }

            fd = open(tmp, O_RDWR | O_TRUNC | O_CREAT | BINARY, 0644);
            if (fd < 0)
            {
                fprintf(stderr, "Error: Opening file: %s\n", tmp);
                zip_fclose(file);
                goto error;
            }

            sum = 0;
            while (sum != stat.size)
            {
                filelen = zip_fread(file, buf, 4096);
                if (len < 0)
                {
                    fprintf(stderr, "Error: Reading file: %s\n", tmp);
                    close(fd);
                    zip_fclose(file);
                    goto error;
                }

                write(fd, buf, filelen);
                sum += filelen;
            }

            close(fd);
            zip_fclose(file);
        }
    }

    zip_close(archive);
    return 0;

error:
    zip_close(archive);
    return 1;
}

char const*
_gethome(void)
{
    char const* homedir = NULL;
    if ((homedir = getenv("HOME")) == NULL)
    {
        struct passwd* uid;
        if ((uid = getpwuid(getuid())) != NULL)
        {
            homedir = uid->pw_dir;
        }
    }

    return homedir;
}

int
progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow,
                  curl_off_t ultotal, curl_off_t ulnow)
{
    int i, total;
    double progress;
    ((void)ultotal), ((void)ulnow);
    if (dltotal <= 0) { return 0; }

    progress = dlnow / dltotal;
    total = _round(progress * 40.0);

    printf("%s [", (char*)clientp);
    for (i = 0; i < total; i++)
    {
        printf("=");
    }
    for (; i < 40; i++)
    {
        printf(" ");
    }
    printf("] %.0f%%\r", progress * 100.0f);
    fflush(stdout);

    return 0;
}

int
old_progress_callback(void* p, double dltotal, double dlnow,
                          double ultotal, double ulnow)
{
    return progress_callback(p, (curl_off_t)dltotal, (curl_off_t)dlnow,
                             (curl_off_t)ultotal, (curl_off_t)ulnow);
}

int
download_file(char const* url, char const* outfile, int verbose)
{
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        int len, ret = 1;
        FILE* file;
        char* base;
        char filename[FILENAME_MAX];

        file = fopen(outfile, "wb");
        if (!file) { return 1; }

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        if (verbose)
        {
            len = strlen(outfile);
            memcpy(filename, outfile, len);
            filename[len] = '\0';
            base = basename(filename);

#if LIBCURL_VERSION_NUM >= 0x072000
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, &progress_callback);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, base);
#else
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &old_progress_callback);
            curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, base);
#endif
        }

        res = curl_easy_perform(curl);
        if (verbose) { printf("\n"); }
        if (res == CURLE_OK)
        {
            long http_code = 0;

            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200)
            { ret = 0; }
            else { ret = 1; }
        }

        curl_easy_cleanup(curl);
        fclose(file);
        return ret;
    }

    return 1;
}

struct curl_string
{
    char* str;
    size_t len;
};

void
curl_string_init(struct curl_string* str)
{
    str->len = 0;
    str->str = malloc(1024);
    if (str->str == NULL) { exit(EXIT_FAILURE); }

    str->str[1024] = '\0';
}

size_t
write_function(void* ptr, size_t size, size_t nmemb, void* stream)
{
    struct curl_string* str = (struct curl_string*)stream;
    size_t newlen = str->len + size * nmemb;

    str->str = realloc(str->str, newlen + 1);
    if (str->str == NULL) { exit(EXIT_FAILURE); }

    memcpy(str->str + str->len, ptr, size * nmemb);
    str->str[newlen] = '\0';
    str->len = newlen;

    return size * nmemb;
}

int
download_content(char const* url, char** out, int verbose)
{
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        struct curl_string str;
        int ret = 1;

        curl_string_init(&str);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

        if (verbose)
        {
        }

        res = curl_easy_perform(curl);
        if (res == CURLE_OK)
        {
            long http_code = 0;

            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200)
            {
                *out = str.str;
                ret = 0;
            }
            else
            {
                free(str.str);
                *out = NULL;
                ret = 1;
            }
        }

        curl_easy_cleanup(curl);
        return ret;
    }

    *out = NULL;
    return 1;
}

