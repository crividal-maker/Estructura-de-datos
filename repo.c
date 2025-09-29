#include <stdio.h>
#include <stdlib.h>
#include "ugit.h"

int repo_existe(void) {
    FILE *f = fopen(".ugit/commits.txt", "r");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

static int crear_carpeta(void) {
#ifdef _WIN32
    return system("mkdir .ugit");
#else
    return system("mkdir -p .ugit");
#endif
}

static void crear_archivos(void) {
    FILE *f;
    f = fopen(".ugit/commits.txt", "w"); if (f) fclose(f);
    f = fopen(".ugit/staging.txt", "w"); if (f) fclose(f);
}

void init_repo(void) {
    if (repo_existe()) {
        printf("Ya existe un repositorio µGit en este directorio.\n");
        return;
    }
    if (crear_carpeta() != 0) {
        printf("Error al crear la carpeta del repositorio.\n");
        return;
    }
    crear_archivos();
    printf("Repositorio µGit creado correctamente.\n");
}
