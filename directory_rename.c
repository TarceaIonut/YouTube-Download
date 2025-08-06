
#include "directory_rename.h"
#include "string_helper_functions.h"
#include "directory.h"
#include <stdio.h>

#include "directory_search.h"

char* get_name_with_number(DIRECTORY* dir, char* name, int number, int nr_digits) {
    char* string = get_file_index_number_string(number, nr_digits);
    int len_string = strlen(string);
    int len_name = strlen(name);
    char* new_file_name = malloc(len_string + 2 + len_name);
    sprintf(new_file_name, "%s-%s", string, name);
    new_file_name[len_string + 1 + len_name] = '\0';
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
    if (dir == NULL) return 0;
    char* full_path = get_name_with_number(dir, name, number, nr_digits);
    rename_directory(dir, full_path);
    free(full_path);
    return 1;
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
    strcpy(dir->main_directory.cFileName, new_name);
    helper_change_path_with_new_name(&dir->path, new_name);

    free(new_path);
    return 1;
}
int order_directory_list(DIRECTORY_LIST* dir_list, int numbers[], int nr_numbers) {
    if (dir_list == NULL || dir_list->directories_list == NULL) {
        return 0;
    }
    int ordered_list_size = min(dir_list->nr_directories, nr_numbers);
    DIRECTORY_LIST* ordered_list = directory_list_new_with_size(ordered_list_size);
    int nr_digits = get_nr_digits(ordered_list_size);
    for (int i = 0; i < ordered_list_size; i++) {
        ordered_list->directories_list[i] = dir_list->directories_list[numbers[i]];
        //PRINT_DIRECTORY(ordered_list->directories_list[i], FILE_TYPE | S_NAME | S_TYPE);
        rename_directory_with_number(ordered_list->directories_list[i], ordered_list->directories_list[i]->main_directory.cFileName, i, nr_digits);
    }
    free(dir_list->directories_list);
    dir_list->directories_list = ordered_list->directories_list;
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