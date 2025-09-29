#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "ugit.h"

static FILE *abrir_staging(void) {
    return fopen(".ugit/staging.txt", "a");
}

static FILE *leer_staging(void) {
    return fopen(".ugit/staging.txt", "r");
}

static void limpiar_staging(void) {
    FILE *f = fopen(".ugit/staging.txt", "w");
    if (f) fclose(f);
}

static void generar_id(char *dest, size_t len) {
    const char *chars = "abcdef0123456789";
    srand(time(NULL) ^ rand());
    for (size_t i = 0; i < len - 1; i++)
        dest[i] = chars[rand() % 16];
    dest[len - 1] = '\0';
}

void add_file(const char *archivo) {
    FILE *f = abrir_staging();
    if (!f) {
        printf("No se pudo abrir el área de preparación.\n");
        return;
    }
    fprintf(f, "%s\n", archivo);
    fclose(f);
    printf("Se añadió '%s' al área de preparación.\n", archivo);
}

void make_commit(const char *mensaje) {
    if (version_count >= MAX_VERSIONS) {
        printf("Máximo de commits alcanzado.\n");
        return;
    }
    FILE *staging = leer_staging();
    if (!staging) {
        printf("Ejecuta primero ugit init.\n");
        return;
    }
    fseek(staging, 0, SEEK_END);
    if (ftell(staging) == 0) {
        printf("No hay archivos en el área de preparación.\n");
        fclose(staging);
        return;
    }
    rewind(staging);

    Version v;
    generar_id(v.id, MAX_ID_LEN);
    time_t now = time(NULL);
    strftime(v.fecha, sizeof(v.fecha), "%Y-%m-%d %H:%M:%S", localtime(&now));
    strncpy(v.mensaje, mensaje, sizeof(v.mensaje));
    v.total_archivos = 0;
    while (fgets(v.archivos[v.total_archivos], sizeof(v.archivos[0]), staging)) {
        v.archivos[v.total_archivos][strcspn(v.archivos[v.total_archivos], "\n")] = '\0';
        v.total_archivos++;
    }
    fclose(staging);

    versions[version_count++] = v;

    FILE *f = fopen(".ugit/commits.txt", "a");
    if (f) {
        fprintf(f, "commit %s\n", v.id);
        fprintf(f, "Date: %s\n", v.fecha);
        fprintf(f, "Message: %s\n", v.mensaje);
        fprintf(f, "Files:\n");
        for (int i = 0; i < v.total_archivos; i++)
            fprintf(f, "%s\n", v.archivos[i]);
        fprintf(f, "---\n");
        fclose(f);
    }

    limpiar_staging();
    printf("[master] commit %s - %s\n", v.id, mensaje);
}

void load_versions(void) {
    FILE *f = fopen(".ugit/commits.txt", "r");
    if (!f) return;
    char linea[256];
    Version tmp = {0};
    while (fgets(linea, sizeof(linea), f)) {
        if (strncmp(linea, "commit", 6) == 0) {
            if (tmp.total_archivos > 0 || tmp.id[0] != '\0') {
                versions[version_count++] = tmp;
                memset(&tmp, 0, sizeof(tmp));
            }
            sscanf(linea, "commit %8s", tmp.id);
        } else if (strncmp(linea, "Date:", 5) == 0) {
            sscanf(linea, "Date: %[^\n]", tmp.fecha);
        } else if (strncmp(linea, "Message:", 8) == 0) {
            sscanf(linea, "Message: %[^\n]", tmp.mensaje);
        } else if (strncmp(linea, "---", 3) == 0) {
            versions[version_count++] = tmp;
            memset(&tmp, 0, sizeof(tmp));
        } else if (strlen(linea) > 1) {
            linea[strcspn(linea, "\n")] = '\0';
            strncpy(tmp.archivos[tmp.total_archivos], linea, 255);
            tmp.total_archivos++;
        }
    }
    if (tmp.total_archivos > 0 || tmp.id[0] != '\0') {
        versions[version_count++] = tmp;
    }
    fclose(f);
}

void show_log(void) {
    if (version_count == 0) {
        printf("No hay commits.\n");
        return;
    }
    for (int i = version_count - 1; i >= 0; i--) {
        printf("commit %s%s\n", versions[i].id,
               (i == version_count - 1) ? " (HEAD -> master)" : "");
        printf("Author: Tu Nombre\n");
        printf("Date: %s\n", versions[i].fecha);
        printf("%s\n\n", versions[i].mensaje);
    }
}

void checkout_version(const char *id) {
    FILE *f = fopen(".ugit/commits.txt", "r");
    if (!f) {
        printf("Error al abrir commits.txt.\n");
        return;
    }
    char linea[256];
    bool mostrando = false, ok = false;
    while (fgets(linea, sizeof(linea), f)) {
        if (strncmp(linea, "commit", 6) == 0) {
            char cid[MAX_ID_LEN];
            if (sscanf(linea, "commit %8s", cid) == 1) {
                if (strcmp(cid, id) == 0) {
                    mostrando = true; ok = true;
                    printf("%s", linea);
                    continue;
                } else mostrando = false;
            }
        }
        if (mostrando) {
            if (strncmp(linea, "---", 3) == 0) break;
            printf("%s", linea);
        }
    }
    if (!ok) printf("No se encontró commit %s\n", id);
    fclose(f);
}
