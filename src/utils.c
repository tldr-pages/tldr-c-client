#include "tldr.h"
#include "utils.h"
#include <errno.h>
#include <fcntl.h>
#include <fts.h>
#include <math.h>
#include <pwd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <zip.h>

#if defined(_WIN32)
    #define BINARY O_BINARY
#else
    #define BINARY 0
#endif

char const *
gethome(void)
{
    char const *homedir = NULL;
    if ((homedir = getenv("HOME")) == NULL) {
        struct passwd *uid;
        if ((uid = getpwuid(getuid())) != NULL)
        { homedir = uid->pw_dir; }
    }

    return homedir;
}

int
sstrncat(char *dest, size_t *pos, size_t max, char const *src, size_t len)
{
    if ((*pos + len) > max)
    { return 1; }
    memcpy(dest + *pos, src, len);
    *(dest + *pos + len) = '\0';

    *pos += len;
    return 0;
}

double
rround(double arg)
{
    double integer;
    double fractional;

    fractional = modf(arg, &integer);
    if (fractional < 0.5)
    { return floor(arg); }
    else
    { return ceil(arg); }
}

int
rm(char const *path, int options)
{
    FTS *tree;
    FTSENT *cur = NULL;
    char *paths[2];
    paths[0] = (char *)path;
    paths[1] = NULL;

    tree = fts_open(paths, FTS_COMFOLLOW | FTS_NOCHDIR, NULL);
    if (tree != NULL) {
        while ((cur = fts_read(tree)) != NULL) {
            switch (cur->fts_info) {
            case FTS_NS:
            case FTS_DNR:
            case FTS_ERR:
                if ((options & RMOPT_IGNORE_NOFILE)
                        && cur->fts_errno == ENOENT) {
                    return 0;
                }

                fprintf(stderr, "Error: %s: error: %s\n", cur->fts_accpath,
                        strerror(cur->fts_errno));
                return 1;

            case FTS_DC:
            case FTS_DOT:
            case FTS_NSOK:
                /* Not reached unless FTS_LOGICAL, FTS_SEEDOT, or FTS_NOSTAT
                 * were */
                /* passed to fts_open() */
                break;

            case FTS_D:
                /* Do nothing. Need depth-first search, so directories are
                 * deleted */
                /* in FTS_DP */
                break;

            case FTS_DP:
            case FTS_F:
            case FTS_SL:
            case FTS_SLNONE:
            case FTS_DEFAULT:
                if (remove(cur->fts_accpath) < 0) {
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
unzip(char const *path, char const *outpath)
{
    int err;
    long i, len;
    size_t filelen;
    struct zip *archive;
    struct zip_file *file;
    struct zip_stat stat;
    int fd;
    size_t sum;
    size_t slen;
    char buf[4096];
    size_t outlen;
    char tmp[STRBUFSIZ];

    archive = zip_open(path, 0, &err);
    if (!archive) {
        /* It looks like, linux ships an old version of libzip, where this
         * isn't supported, yet. To not break the builds, it's commented out,
         * and can be enabled for debugging.
         */
        /* zip_error_t error; */
        /* zip_error_init_with_code(&error, err); */
        /* fprintf(stderr, "Error: Opening Zip: %s (%s)\n", path, */
        /*         zip_error_strerror(&error)); */
        /* zip_error_fini(&error); */
        fprintf(stderr, "Error: Opening Zip: %s\n", path);
        return 1;
    }

    outlen = strlen(outpath);
    len = (long)zip_get_num_entries(archive, 0);
    for (i = 0; i < len; i++) {
        if (zip_stat_index(archive, (zip_uint64_t)i, 0, &stat))
        { goto error; }

        filelen = strlen(stat.name);

        slen = 0;
        if (sstrncat(tmp, &slen, STRBUFSIZ, outpath, outlen))
        { goto error; }
        if (sstrncat(tmp, &slen, STRBUFSIZ, "/", 1))
        { goto error; }
        if (sstrncat(tmp, &slen, STRBUFSIZ, stat.name, filelen))
        { goto error; }

        if (tmp[outlen + filelen + 1 - 1] == '/') {
            if (mkdir(tmp, 0755)) {
                fprintf(stderr, "Error: Creating directory: %s\n", tmp);
                goto error;
            }
        } else {
            file = zip_fopen_index(archive, (zip_uint64_t)i, 0);
            if (!file) {
                fprintf(stderr, "Error: Opening zip content: %s", tmp);
                goto error;
            }

            fd = open(tmp, O_RDWR | O_TRUNC | O_CREAT | BINARY, 0644);
            if (fd < 0) {
                fprintf(stderr, "Error: Opening file: %s\n", tmp);
                zip_fclose(file);
                goto error;
            }

            sum = 0;
            while (sum != stat.size) {
                filelen = (size_t)zip_fread(file, buf, 4096);
                if (len < 0) {
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
