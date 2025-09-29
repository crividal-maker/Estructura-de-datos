#ifndef UGIT_H
#define UGIT_H

#define MAX_VERSIONS 100
#define MAX_FILES    20
#define MAX_ID_LEN    9

typedef struct {
    char id[MAX_ID_LEN];
    char fecha[64];
    char mensaje[256];
    char archivos[MAX_FILES][256];
    int total_archivos;
} Version;

/* variables globales */
extern Version versions[MAX_VERSIONS];
extern int version_count;

/* prototipos */
int repo_existe(void);
void init_repo(void);
void add_file(const char *archivo);
void make_commit(const char *mensaje);
void load_versions(void);
void show_log(void);
void checkout_version(const char *id);

#endif
