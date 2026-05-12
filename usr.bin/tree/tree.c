#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

long TotalSize = 0;
long TotalSubdirectories = 0;
long TotalFiles = 0;

int tree(const char *path, const char *identation);

int tree(const char *path, const char *identation)
{
    struct dirent *entry;
    struct stat info;
    DIR *dp;

    dp = opendir(path);
    if (dp == NULL)
    {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dp)))
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        // Usar lstat() para NO seguir enlaces
        if (lstat(fullpath, &info) == -1)
        {
            perror("lstat");
            continue;
        }

        // Verificar si es enlace simbólico
        if (S_ISLNK(info.st_mode))
        {
            // Es enlace simbólico, mostrarlo pero NO entrar
            printf("%s%s [symlink]\n", identation, entry->d_name);
            TotalFiles++;  // Contar como archivo (o podrías tener contador separado)
            // IMPORTANTE: NO llamar a tree() aquí
            continue;
        }

        // Si es directorio regular (NO enlace)
        if (S_ISDIR(info.st_mode))
        {
            TotalSubdirectories++;
            char NewIdentation[1024];
            snprintf(NewIdentation, sizeof(NewIdentation), "%s  ", identation);
            printf("%s/%s\n", identation, entry->d_name);
            tree(fullpath, NewIdentation);  // Seguro: es directorio real
        }
        else
        {
            TotalFiles++;
            TotalSize = TotalSize + info.st_size;
            printf("%s%s ---- %lld b\n", identation, entry->d_name, (long long)info.st_size);
        }
    }

    closedir(dp);
    return 0;
}

int main(int argc, char *argv[])
{
    const char *path;

    if (argc < 2)
        path = ".";
    else
        path = argv[1];

    printf("Explorando ... [%s]\n", path);
    tree(path, "  ");
    printf("[Total Size: %lld B]\n[Subdirectories: %ld]\n[Files: %ld]\n", 
           (long long)TotalSize, TotalSubdirectories, TotalFiles);
    return 0;
}
