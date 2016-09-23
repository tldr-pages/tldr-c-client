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
rm(char const *path)
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
    len = zip_get_num_entries(archive, 0);
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

int
copyfile(const char *src, const char *dest, mode_t perms)
{
    ssize_t n;
    int src_fd, dest_fd;
    int dest_flags = O_CREAT | O_WRONLY | O_TRUNC;
    int buffer[BUFSIZ];
    int error_occured = 0;

    /* Open source file */
    if ((src_fd = open(src, O_RDONLY)) == -1) {
        fprintf(stderr,
            "Error: Couldn't open source file \"%s\" for copying: %s\n",
            src,
            strerror(errno)
        );
        return 1;
    }

    /* Open destination file with the source's permissions. */
    if ((dest_fd = open(dest, dest_flags, perms)) == -1) {
        fprintf(stderr,
            "Error: Couldn't open destination file \"%s\" for copying: %s\n",
            dest,
            strerror(errno)
        );
        error_occured = 1;
    } else { /* Open destination was successful */
        /* Write to dest while we can read from src */
        while ((n = read(src_fd, buffer, BUFSIZ)) > 0) {
            /* Handle read error */
            if (n == -1) {
                fprintf(stderr,
                    "Read error during copy from \"%s\": %s\n",
                    src,
                    strerror(errno)
                );
                error_occured = 1;
                break;
            }

            /* Write or handle write error */
            if (write(dest_fd, buffer, (size_t) n) == -1) {
                fprintf(stderr,
                    "Write error during copy to \"%s\": %s\n",
                    dest,
                    strerror(errno)
                );
                error_occured = 1;
                break;
            }
        }

        /* Close source file */
        if (close(src_fd) == -1) {
            fprintf(stderr,
                "Error: Couldn't close source file \"%s\" after copying: %s\n",
                src,
                strerror(errno)
            );
            error_occured = 1;
        }
    } /* Else just try to clean up dest_fd, bc opening source file failed */

    /* Close destination file */
    if (close(dest_fd) == -1) {
        fprintf(stderr,
            "Error: Couldn't close destination file \"%s\" after copying: %s\n",
            dest,
            strerror(errno)
        );
        error_occured = 1;
    }

    return error_occured;
}

int
copytree(const char *src, const char *dest)
{

    /* Variables for getting path relative to destination
     */
    size_t src_len = strlen(src);
    size_t dest_len = strlen(dest) - 1; /* Ignore '/' at end */
    char destbuf[STRBUFSIZ];

    int cur_is_dir = 0; /* Is the current object a directory? */

    /* Tree Transversal */
    FTS *tree;
    FTSENT *cur = NULL;
    char *paths[2];
    paths[0] = (char *)src;
    paths[1] = NULL;

    /* Put destnation path in buffer, paths will be appended to it. */
    strncpy(destbuf, dest, dest_len);

    tree = fts_open(paths, FTS_COMFOLLOW | FTS_NOCHDIR, NULL);
    if (tree != NULL) {
        while ((cur = fts_read(tree)) != NULL) {
            switch (cur->fts_info) {
            case FTS_NS:
            case FTS_DNR:
            case FTS_ERR:
                fprintf(stderr, "Error: %s: error: %s\n", cur->fts_accpath,
                        strerror(cur->fts_errno));
                fts_close(tree);
                return 1;

            case FTS_DC:
            case FTS_DOT:
            case FTS_NSOK:
                /* Not reached unless FTS_LOGICAL, FTS_SEEDOT, or FTS_NOSTAT
                 * were */
                /* passed to fts_open() */
                break;

            case FTS_DP:
                /* We're going to create directories before looking at their
                 * files. So we're ignoring this.
                 */
                break;

            case FTS_D:
                cur_is_dir = 1; /* Its a directory */
            case FTS_F:
            case FTS_SL:
            case FTS_SLNONE:
            case FTS_DEFAULT:
                /* Append relative path to destbuf, overwriting anything else
                 * that might be there after the destination path.
                 */
                if ((cur->fts_pathlen - src_len + dest_len) > STRBUFSIZ) {
                    fprintf(stderr, "Error: Path too long\n");
                    fts_close(tree);
                    return 1;
                }
                strncpy(destbuf + dest_len, cur->fts_path + src_len,
                        cur->fts_pathlen - src_len); 
                destbuf[dest_len + cur->fts_pathlen - src_len] = '\0';

                if (cur_is_dir) { /* Create Directory with perms of src one */
                    if (mkdir(destbuf, cur->fts_statp->st_mode)) {
                        if (errno != EEXIST) { /* Ignore if already exists */
                            fprintf(stderr,
                                    "Couldn't create directory \"%s\": %s\n",
                                    destbuf,
                                    strerror(errno)
                            );
                            fts_close(tree);
                            return 1;
                        }
                    }
                    cur_is_dir = 0;
                } else { /* Copy File */
                    if (copyfile(cur->fts_path, destbuf,
                                 cur->fts_statp->st_mode)) {
                        fts_close(tree);
                        return 1;
                    }
                }
                break;
            }
        }

        fts_close(tree);
        return 0;
    }

    return 1;
}
