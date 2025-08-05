#include "directory.h"
#include "directory_search.h"

#include <stdio.h>


ALL* new_ALL_from_strings(int choice, char** strings, int nr_strings) {
    ALL* new = (ALL*)malloc(sizeof(ALL));
    new->choice = choice;
    new->depth = nr_strings;
    new->names = malloc(sizeof(char*) * nr_strings);
    for (int i = 0; i < nr_strings; i++) {
        if (strings[i] == NULL) {
            new->names[i] = NULL;
            continue;
        }
        new->names[i] = malloc(sizeof(char) * (strlen(strings[i]) + 1));
        strcpy(new->names[i], strings[i]);

    }
    return new;
}
void free_ALL(ALL* new) {
    if (new != NULL) {
        for (int i = 0; i < new->depth; i++) {
            free(new->names[i]);
        }
        free(new->names);
    }
    free(new);
}
ALL* new_ALL_from_string(char c, char* b, char* a, char* s) {
    int choice = 0;
    if (c == 'b') choice = 0;
    if (c == 'a') choice = 1;
    if (c == 's') choice = 2;
    char* names[3] = {b, a, s};
    return new_ALL_from_strings(choice, names, 3);
}
int find_if_correct_check_below(DIRECTORY* potential_dir, ALL* all, int start) {
    int type = DIR_TYPE;
    if (start == all->depth - 1) {
        type = FILE_TYPE;
    }
    if (!take_file_based_on_type(potential_dir, type) || !take_file_based_on_name(potential_dir, all->names[start])) {
        return 0;
    }
    if (start == all->depth - 1) {
        return 1;
    }
    for (int i = 0; i < potential_dir->nr_directories; i++) {
        if (find_if_correct_check_below(potential_dir->sub_directories[i], all, start + 1)) {
            return 1;
        }
    }
    return 0;
}

int get_all_correct_dirs(DIRECTORY* music, ALL* all, DIRECTORY_LIST* directories_list) {
    DIRECTORY_LIST* list = directory_list_new();
    get_correct_files_check_above(music, all, list);
    int nr = 0;
    for (int i = 0; i < list->nr_directories; i++) {
        if (find_if_correct_check_below(list->directories_list[i], all, all->choice)) {

            directory_list_add_dir(directories_list, list->directories_list[i]);
            nr++;
        }
    }
    free_directories_list(list);
    return nr;
}
int get_correct_files_check_above(DIRECTORY* music, ALL* all, DIRECTORY_LIST* directories_list) {
    int nr_dir_list = all->choice;
    DIRECTORY_LIST* dl_old = directory_list_new();
    directory_list_add_dir(dl_old, music);
    for (int i = 0; i <= nr_dir_list; i++) {
        int type = DIR_TYPE;
        if (i == all->depth - 1) {
            type = FILE_TYPE;
        }
        DIRECTORY_LIST* dl_new = directory_list_new();
        add_named_files_from_dir_list_to_list(dl_old, all->names[i], type, dl_new);
        free_directories_list(dl_old);
        dl_old = dl_new;
    }
    copy_from_list_to_list(dl_old, directories_list);
    return nr_dir_list;
}
DIRECTORY* directory_make_music_dir(char* path) {
    DIRECTORY* directories = directory_new(path, NULL);
    //printf("directories %p\n", directories);
    directory_make_from_path_recursive(directories, path, 3);
    //printf("directories %p\n", directories);
    return directories;
}
int print_directory(DIRECTORY* data, int what_to_show, int depth, int number, int nr_digits) {
    if (what_to_show & S_NUMBER && number == -1) {
        return 0;
    }
    if (!take_file_based_on_type(data, what_to_show)) {
        return 0;
    }
    if (what_to_show & S_NUMBER) {
        char* string;
        string = get_file_index_number_string(number, nr_digits);
        printf("%s ", string);
        free(string);
    }
    for (int i = 0; i < depth; i++) {
        printf("        ");
    }
    if (what_to_show & S_PATH) {
        printf("PATH=%s ", data->path);
    }
    if (what_to_show & S_TYPE) {
        if (data->main_directory.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            printf("DIR : ");
        }
        else {
            printf("FILE: ");
        }
    }
    if (what_to_show & S_NAME) {
        printf("%s", data->main_directory.cFileName);
    }
    printf("\n");
    return 1;
}
void recursive_show_directories(DIRECTORY* directories, int recursion_number, int depth, int what_to_show) {
    if (is_dir(&directories->main_directory) && (what_to_show & DIR_TYPE)) {
        print_directory(directories, S_TYPE | S_NAME, depth, 0, 0);
    }
    else if (!is_dir(&directories->main_directory) && (what_to_show & FILE_TYPE)) {
        print_directory(directories, S_TYPE | S_NAME, depth, 0, 0);
    }
    if (recursion_number == 0) {
        return;
    }
    for (int i = 0; i < directories->nr_directories; i++) {
        if (directories->sub_directories[i] != NULL)
            recursive_show_directories(directories->sub_directories[i], recursion_number - 1, depth + 1, what_to_show);
    }
}

// int main() {
//     //char* dir_path = get_dirpath_from_filepath();
//     DIRECTORIES *directories = make_music_directory("D:/Music");
//     ALL* all = new_ALL_from_string('s', "Slipknot", "iowa", NULL);
//     DIRECTORIES_LIST* directories_list = new_directories_list();
//     get_all_correct_dirs(directories, all, directories_list);
//     for (int i = 0; i < directories_list->nr_directories; i++) {
//         PRINT_DIRECTORY(directories_list->directories_list[i], S_NAME | S_NUMBER, i, get_nr_digits(directories_list->nr_directories));
//     }
//     free_ALL(all);
//     free_directories_list(directories_list);
//     free_directories(directories);
// }
