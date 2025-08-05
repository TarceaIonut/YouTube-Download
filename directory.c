#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "directory.h"






int free_directories_list(DIRECTORIES_LIST* dir) {
    if (dir != NULL) {
        free(dir->directories_list);
        free(dir);
        return 1;
    }
    return 0;
}
DIRECTORIES_LIST* new_directories_list() {
    DIRECTORIES_LIST* new_directories_list = malloc(sizeof(DIRECTORIES_LIST));
    new_directories_list->nr_directories = 0;
    new_directories_list->max_allowed_dirs = INITIAL_DIR_SIZE;
    new_directories_list->directories_list = malloc(sizeof(DIRECTORIES_LIST*) * INITIAL_DIR_SIZE);
    return new_directories_list;
}
void expand_directories_list(DIRECTORIES_LIST* directories_list) {
    directories_list->max_allowed_dirs += INITIAL_DIR_SIZE;
    DIRECTORIES** old = directories_list->directories_list;
    DIRECTORIES** new = malloc(sizeof(DIRECTORIES*) * directories_list->max_allowed_dirs);
    memcpy(new, old, sizeof(DIRECTORIES*) * directories_list->nr_directories);
    directories_list->directories_list = new;
    free(old);
}
void add_to_directories_list(DIRECTORIES_LIST* directories_list, DIRECTORIES* directories) {
    if (directories_list->max_allowed_dirs >= directories_list->nr_directories) {
        expand_directories_list(directories_list);
    }
    directories_list->directories_list[directories_list->nr_directories++] = directories;
}
boolean is_dir(const WIN32_FIND_DATAA* data) {
    return data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}
DIRECTORIES* new_directory(char* path, DIRECTORIES* parrent){
    DIRECTORIES* directory = malloc(sizeof(DIRECTORIES));
    directory->main_directory = get_data_for_current_directoty(path);
    directory->parrent_directory = parrent;
    directory->path = path;
    directory->nr_directories = 0;
    directory->max_allowed_dirs = INITIAL_DIR_SIZE;
    directory->sub_directories = malloc(INITIAL_DIR_SIZE * sizeof(DIRECTORIES*));
    return directory;
}
void free_directories(DIRECTORIES* directories) {
    for (int i = 0; i < directories->nr_directories; i++) {
        free_directories(directories->sub_directories[i]);
    }
    free(directories->sub_directories);
    free(directories);
}
void directories_expand_sub_dir_size(DIRECTORIES* directories) {
    directories->max_allowed_dirs += INITIAL_DIR_SIZE;
    DIRECTORIES** old = directories->sub_directories;
    DIRECTORIES** new = malloc(sizeof(DIRECTORIES*) * directories->max_allowed_dirs);
    memcpy(new, old, sizeof(DIRECTORIES*) * directories->nr_directories);
    directories->sub_directories = new;
    free(old);
}
void add_directory(DIRECTORIES* directories, DIRECTORIES *dir) {
    if (directories->nr_directories >= directories->max_allowed_dirs) {
        directories_expand_sub_dir_size(directories);
    }
    directories->sub_directories[directories->nr_directories] = malloc(sizeof(DIRECTORIES));
    memcpy(directories->sub_directories[directories->nr_directories], dir, sizeof(DIRECTORIES));
    directories->nr_directories++;
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
WIN32_FIND_DATAA get_data_for_current_directoty(char* path) {
    WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
    WIN32_FIND_DATAA data;
    int x = GetFileAttributesExA(path, GetFileExInfoStandard, &fileAttributes);
    data.dwFileAttributes = fileAttributes.dwFileAttributes;
    data.ftCreationTime = fileAttributes.ftCreationTime;
    data.ftLastAccessTime = fileAttributes.ftLastAccessTime;
    data.ftLastWriteTime = fileAttributes.ftLastWriteTime;
    data.nFileSizeHigh = fileAttributes.nFileSizeHigh;
    data.nFileSizeLow = fileAttributes.nFileSizeLow;
    strcpy(data.cFileName, get_dir_name_from_path(path));
    if (x < 0) {
        data.dwFileAttributes = -1;
    }
    return data;
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
boolean recursive_make_directories_from_path(DIRECTORIES* directories, char* path, int recursion) {
    WIN32_FIND_DATAA data;
    directories->main_directory = get_data_for_current_directoty(path);
    directories->path = path;
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
            //printf("new_path: %s %d  get_dir_name = %s %d\n", new_path, strlen(new_path), get_dir_name_from_path(new_path), strlen(get_dir_name_from_path(new_path)));
            add_directory(directories, new_directory(new_path, directories));

            recursive_make_directories_from_path(directories->sub_directories[directories->nr_directories - 1], new_path, recursion - 1);
        } while (FindNextFileA(hFind, &data));
        FindClose(hFind);
    }
    else{
        return 0;
    }
    return 1;
}



DIRECTORIES* find_directories_by_name(DIRECTORIES* directories, char* name) {
    for (int i = 0; i < directories->nr_directories; i++) {
        if (directories->sub_directories[i] != NULL)
            if (strcasecmp(directories->sub_directories[i]->main_directory.cFileName, name) == 0) {
                return directories->sub_directories[i];
            }
    }
    return NULL;
}

void direct_transfer_directories_to_list(DIRECTORIES* directories, DIRECTORIES_LIST* directories_list) {
    directories_list->directories_list = directories->sub_directories;
    directories_list->nr_directories = directories->nr_directories;
    directories_list->max_allowed_dirs = directories->max_allowed_dirs;
}
void add_all_sub_dirs_to_list(DIRECTORIES* directories, DIRECTORIES_LIST* directories_list) {
    for (int i = 0; i < directories->nr_directories; i++) {
        add_to_directories_list(directories_list, directories->sub_directories[i]);
    }
}
boolean get_named_file_from_dir(DIRECTORIES* directories, char* band_name, DIRECTORIES_LIST* directories_list) {
    if (band_name == NULL) {
        direct_transfer_directories_to_list(directories, directories_list);
        return 1;
    }
    else {
        DIRECTORIES* dir = find_directories_by_name(directories, band_name);
        if (dir == NULL) {
            return 0;
        }
        add_to_directories_list(directories_list, dir);
    }
    return 1;
}
boolean take_file_based_on_type(DIRECTORIES* dir, int type) {
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
boolean take_file_based_on_name(DIRECTORIES* dir, char* name) {
    if (name == NULL) return 1;
    if (name[0] == 0) return 1;
    if (strcasecmp(dir->main_directory.cFileName, name) == 0) return 1;

    if (str_ends_with(dir->main_directory.cFileName, name)) return 1;
    return str_starts_with(dir->main_directory.cFileName, name);

}
int add_named_files_from_dir_to_list(DIRECTORIES* directories, char* name, int type, DIRECTORIES_LIST* directories_list) {
    int nr_added = 0;
    for (int i = 0; i < directories->nr_directories; i++) {
        if (directories->sub_directories[i] != NULL) {
            if (take_file_based_on_name(directories->sub_directories[i], name)) {
                printf("");
            }
            if (take_file_based_on_name(directories->sub_directories[i], name) && take_file_based_on_type(directories->sub_directories[i], type)) {
                add_to_directories_list(directories_list, directories->sub_directories[i]);
                nr_added++;
            }
        }
    }
    return nr_added;
}
int add_named_files_from_dir_list_to_list(DIRECTORIES_LIST* directories, char* name, int type, DIRECTORIES_LIST* directories_list) {
    int nr_added = 0;
    for (int i = 0; i < directories->nr_directories; i++) {
        nr_added += add_named_files_from_dir_to_list(directories->directories_list[i], name, type, directories_list);
    }
    return nr_added;
}
void copy_from_list_to_list(DIRECTORIES_LIST* source, DIRECTORIES_LIST* destination) {
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