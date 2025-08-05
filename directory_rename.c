
#include "directory_rename.h"
#include "directory.h"
#include <stdio.h>

char* get_name_with_number(DIRECTORY* dir, char* name, int number, int nr_digits) {
    char* string = get_file_index_number_string(number, nr_digits);
    int len_string = strlen(string);
    int len_name = strlen(name);
    char* new_file_name = malloc(len_string + 1 + len_name);
    sscanf(new_file_name, "%s-%s", string, new_file_name);
    return new_file_name;
}
char* create_new_path_from_full_path(char* full_path, char* name) {
    int len_full_path = strlen(full_path);
    int len_name = strlen(name);
    int last_poz = len_full_path - 1;
    for (int i = len_full_path - 1; i >= 0; i--) {
        if (full_path[i] == '/') {
            last_poz = i;
            break;
        }
    }
    char* new_path = malloc(last_poz + 2 + len_name);
    memcpy(new_path, full_path, last_poz + 1);
    memcpy(new_path + last_poz + 1, name, len_name);
    new_path[last_poz + 1 + len_name] = 0;
    return new_path;
}
int rename_directory_with_number(DIRECTORY* dir, char* name, int number, int nr_digits) {
    char* full_path = get_name_with_number(dir, name, number, nr_digits);
    rename_directory(dir, full_path);
    free(full_path);
}
int rename_directory(DIRECTORY* dir, char* new_name) {
    if (dir == NULL) {
        return 0;
    }
    char* new_path = create_new_path_from_full_path(dir->path, new_name);
    if (rename(dir->path, new_path) == -1) {
        free(new_path);
        return 0;
    }
    strcpy(dir->main_directory.cFileName, new_path);
    free(new_path);
    return 1;
}




// int main() {
//     DIRECTORIES* dir = new_directory("D:/zzzz", NULL);
//     if (dir->main_directory.dwFileAttributes == -1) {
//         printf("File attributes not present\n");
//         return 0;
//     }
//     int rez = rename_directory(dir, "zzzz");
//     printf("%d\n", rez);
// }