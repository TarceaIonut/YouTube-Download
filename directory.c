#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "directory.h"


int directory_list_add_dir_list(DIRECTORY_LIST* final_list, DIRECTORY_LIST* list) {
    if (final_list == NULL || list == NULL) {
        return 0;
    }
    int final_nr_of_dirs = final_list->nr_directories + list->nr_directories;
    if (final_nr_of_dirs > final_list->max_allowed_dirs) {
        directory_list_expand_by_value(final_list, final_nr_of_dirs - final_list->max_allowed_dirs);
    }
    //memcpy(final_list->directories_list + final_list->nr_directories, list->directories_list, list->nr_directories * sizeof(DIRECTORY_LIST*));
    for (int i = 0; i < list->nr_directories; i++) {
        final_list->directories_list[i + final_list->nr_directories] = list->directories_list[i];
    }
    final_list->nr_directories = final_nr_of_dirs;
    return 1;
}
int directory_list_free(DIRECTORY_LIST** dir) {
    if (*dir != NULL) {
        free((*dir)->directories_list);
        free(*dir);
        *dir = nullptr;
        return 1;
    }
    return 0;
}
int directory_list_free_dirs(DIRECTORY_LIST** dir) {
    if ((*dir) == NULL) return 0;
    if ((*dir)->directories_list != NULL) {
        for (int i = 0; i < (*dir)->nr_directories; i++) {
            directory_free(&(*dir)->directories_list[i]);
            (*dir)->directories_list[i] = nullptr;
        }
        free((*dir)->directories_list);
        free(*dir);
        *dir = nullptr;
    }
    return 1;
}
void directory_free(DIRECTORY** directories) {
    if (*directories == NULL) return;
    directory_list_free_dirs(&(*directories)->directory_list);
    free(*directories);
    *directories = nullptr;
}
DIRECTORY_LIST* directory_list_new() {
    DIRECTORY_LIST* new_directories_list = malloc(sizeof(DIRECTORY_LIST));
    new_directories_list->nr_directories = 0;
    new_directories_list->max_allowed_dirs = INITIAL_DIR_SIZE;
    new_directories_list->directories_list = malloc(sizeof(DIRECTORY_LIST*) * INITIAL_DIR_SIZE);
    return new_directories_list;
}
void expand_directories_list(DIRECTORY_LIST* directories_list) {
    directory_list_expand_by_value(directories_list, INITIAL_DIR_SIZE);
}
void directory_list_expand_by_value(DIRECTORY_LIST* directories_list, int value) {
    directories_list->max_allowed_dirs += value;
    DIRECTORY** old = directories_list->directories_list;
    DIRECTORY** new = malloc(sizeof(DIRECTORY*) * directories_list->max_allowed_dirs);
    memcpy(new, old, sizeof(DIRECTORY*) * directories_list->nr_directories);
    directories_list->directories_list = new;
    free(old);
}
void directory_list_add_dir(DIRECTORY_LIST* directories_list, DIRECTORY* directories) {
    if (directories_list->max_allowed_dirs <= directories_list->nr_directories) {
        expand_directories_list(directories_list);
    }
    directories_list->directories_list[directories_list->nr_directories++] = directories;
}
boolean is_dir(const WIN32_FIND_DATAA* data) {
    return data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}
DIRECTORY* directory_new(char* path, DIRECTORY* parrent){
    DIRECTORY* directory = malloc(sizeof(DIRECTORY));
    int rez = get_data_for_current_directory(path, &directory->main_directory);
    if (rez == 0) return nullptr;
    directory->parent_directory = parrent;
    directory->path = path;
    directory->directory_list = directory_list_new();
    return directory;
}

void directory_expand_sub_dir_size(DIRECTORY* directories) {
    directory_list_expand_by_value(directories->directory_list, INITIAL_DIR_SIZE);
}
void directory_add_dir(DIRECTORY* directory, DIRECTORY *dir) {
    if (dir == NULL) return;
    directory_list_add_dir(directory->directory_list, dir);
}
char* get_dir_name_from_path(char* path) {
    int len = strlen(path) - 1;
    int poz = 0;
    for (int i = len; i >= 0; i--) {
        if (path[i] == '/') {
            poz = i + 1;
            break;
        }
    }
    return path + poz;
}
int get_data_for_current_directory(char* path, WIN32_FIND_DATAA* data) {
    WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
    int x = GetFileAttributesExA(path, GetFileExInfoStandard, &fileAttributes);
    if (x < 0) {
        return 0;
    }
    data->dwFileAttributes = fileAttributes.dwFileAttributes;
    data->ftCreationTime = fileAttributes.ftCreationTime;
    data->ftLastAccessTime = fileAttributes.ftLastAccessTime;
    data->ftLastWriteTime = fileAttributes.ftLastWriteTime;
    data->nFileSizeHigh = fileAttributes.nFileSizeHigh;
    data->nFileSizeLow = fileAttributes.nFileSizeLow;
    strcpy(data->cFileName, get_dir_name_from_path(path));

    return 1;
}
char* get_new_path_from_dirpath(char* path, char* name) {
    char* new_path = malloc(strlen(path) + strlen(name) + 2);
    sprintf(new_path, "%s/%s", path, name);
    return new_path;
}
char* get_dirpath_from_filepath(char* filepath){
    char* dirpath = malloc(strlen(filepath) + 3);
    sprintf(dirpath, "%s/*", filepath);
    return dirpath;
}
boolean directory_make_from_path_recursive(DIRECTORY* directory, char* path, int recursion) {
    WIN32_FIND_DATAA data;
    directory->path = path;
    if (recursion == 0) {
        return 1;
    }
    char* dir_path = get_dirpath_from_filepath(path);
    HANDLE hFind = FindFirstFileA(dir_path, &data);
    free(dir_path);
    if (!is_dir(&data)) {
        return 1;
    }
    if (hFind != INVALID_HANDLE_VALUE) {
        char* name;
        do {
            name = data.cFileName;
            if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0){
                continue;
            }

            char* new_path = get_new_path_from_dirpath(path, name);
            DIRECTORY* new_d = directory_new(new_path, directory);
            if (new_d == NULL) {
                continue;
            }
            directory_add_dir(directory, new_d);
            DIRECTORY_LIST* list = directory->directory_list;
            directory_make_from_path_recursive(list->directories_list[list->nr_directories - 1], new_path, recursion - 1);
        } while (FindNextFileA(hFind, &data));
        FindClose(hFind);
    }
    else{
        return 0;
    }
    return 1;
}


DIRECTORY* directory_list_find_by_name(DIRECTORY_LIST* list, char* name) {
    for (int i = 0; i < list->nr_directories; i++) {
        if (list->directories_list[i] != NULL)
            if (strcasecmp(list->directories_list[i]->main_directory.cFileName, name) == 0) {
                return list->directories_list[i];
            }
    }
    return nullptr;
}


void direct_transfer_directories_to_list(DIRECTORY* directories, DIRECTORY_LIST** directories_list) {
    *directories_list = directories->directory_list;
}
void add_all_sub_dirs_to_list(DIRECTORY* directories, DIRECTORY_LIST* directories_list) {
    directory_list_add_dir_list(directories_list, directories->directory_list);
}
boolean get_named_file_from_dir(DIRECTORY* directories, char* band_name, DIRECTORY_LIST* directories_list) {
    if (band_name == NULL) {
        direct_transfer_directories_to_list(directories, &directories_list);
        return 1;
    }
    else {
        DIRECTORY* dir = directory_list_find_by_name(directories->directory_list, band_name);
        if (dir == NULL) {
            return 0;
        }
        directory_list_add_dir(directories_list, dir);
    }
    return 1;
}
boolean take_file_based_on_type(DIRECTORY* dir, int type) {
    //return !((type & DIR_TYPE != 0) ^ (is_dir(&dir->main_directory) != 0));
    return ((type & DIR_TYPE != 0) && is_dir(&dir->main_directory)) || ((type & FILE_TYPE) != 0) && !is_dir(&dir->main_directory);
}
boolean str_starts_with(char* str1, char* str2) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if (len1 < len2) return 0;
    for (int i = 0; i < len2; i++) {
        if (str1[i] != str2[i]) return 0;
    }
    return 1;
}
boolean str_ends_with(char* main_str, char* str) {
    int point_poz = -1;
    int main_str_len = strlen(main_str);
    int str_len = strlen(str);
    for (int i = 0; i < main_str_len; i++) {
        if (main_str[i] == '.') point_poz = i;
    }
    if (point_poz == -1) point_poz = main_str_len;
    if (str_len > point_poz) return 0;
    for (int i = 0; i < str_len; i++) {
        if (str[i] != main_str[i + point_poz - str_len]) return 0;
    }
    return 1;
}
boolean take_file_based_on_name(DIRECTORY* dir, char* name) {
    if (name == NULL) return 1;
    if (name[0] == 0) return 1;
    if (strcasecmp(dir->main_directory.cFileName, name) == 0) return 1;

    if (str_ends_with(dir->main_directory.cFileName, name)) return 1;
    return str_starts_with(dir->main_directory.cFileName, name);

}
int add_named_files_from_dir_to_list(DIRECTORY* directories, char* name, int type, DIRECTORY_LIST* directories_list) {
    int nr_added = 0;
    for (int i = 0; i < directories->directory_list->nr_directories; i++) {
        if (directories->directory_list->directories_list[i] != NULL) {
            if (take_file_based_on_name(directories->directory_list->directories_list[i], name)) {
                printf("");
            }
            if (take_file_based_on_name(directories->directory_list->directories_list[i], name) && take_file_based_on_type(directories->directory_list->directories_list[i], type)) {
                directory_list_add_dir(directories_list, directories->directory_list->directories_list[i]);
                nr_added++;
            }
        }
    }
    return nr_added;
}
int add_named_files_from_dir_list_to_list(DIRECTORY_LIST* directories, char* name, int type, DIRECTORY_LIST* directories_list) {
    int nr_added = 0;
    for (int i = 0; i < directories->nr_directories; i++) {
        nr_added += add_named_files_from_dir_to_list(directories->directories_list[i], name, type, directories_list);
    }
    return nr_added;
}
void copy_from_list_to_list(DIRECTORY_LIST* source, DIRECTORY_LIST* destination) {
    destination->directories_list = source->directories_list;
    destination->nr_directories = source->nr_directories;
    destination->max_allowed_dirs = source->max_allowed_dirs;
}
int get_nr_digits(int number) {
    if (number == 0) return  1;
    int rez = 0;
    while(number) {
        number /= 10;
        rez++;
    }
    return rez;
}
char* get_file_index_number_string(int number, int nr_digits) {
    char* string = malloc((nr_digits + 1) * sizeof(char));
    string[nr_digits] = '\0';
    int number_nr_digits = get_nr_digits(number);
    for (int i = 0; i < nr_digits - number_nr_digits; i++) {
        string[i] = '0';
    }
    sprintf(string + nr_digits - number_nr_digits, "%d", number);
    return string;
}