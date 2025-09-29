#include <stdio.h>
#include <string.h>
#include "ugit.h"

Version versions[MAX_VERSIONS];
int version_count = 0;

int main(int argc, char *argv[]) {
    if (repo_existe()) {
        load_versions();
    }

    if (argc < 2) {
        printf("Uso: ugit <comando>\n");
        return 1;
    }

    if (strcmp(argv[1], "init") == 0) {
        init_repo();
    } else if (strcmp(argv[1], "add") == 0) {
        if (argc < 3)
            printf("Uso: ugit add <archivo>\n");
        else
            add_file(argv[2]);
    } else if (strcmp(argv[1], "commit") == 0) {
        if (argc < 4 || strcmp(argv[2], "-m") != 0)
            printf("Uso: ugit commit -m \"mensaje\"\n");
        else
            make_commit(argv[3]);
    } else if (strcmp(argv[1], "log") == 0) {
        show_log();
    } else if (strcmp(argv[1], "checkout") == 0) {
        if (argc < 3)
            printf("Uso: ugit checkout <id>\n");
        else
            checkout_version(argv[2]);
    } else {
        printf("Comando no reconocido: %s\n", argv[1]);
    }

    return 0;
}
