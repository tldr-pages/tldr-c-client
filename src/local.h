#ifndef LOCAL_H
#define LOCAL_H

long        check_localdate         (void);
int         update_localdate        (void);
int         has_localdb             (void);
int         update_localdb          (int verbose);
int         clear_localdb           (int verbose);
int         get_file_content        (char const *path, char **out, int verbose);

#endif /* LOCAL_H */
