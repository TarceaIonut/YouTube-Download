//
// Created by Ionut on 8/4/2025.
//
#include <Windows.h>

#ifndef DIRECTORY_H
#define DIRECTORY_H

#define MAX_STRING_LENGHT 256
#define INITIAL_DIR_SIZE 32
#define DEFAULT_DEPTH 3
#define DIR_TYPE 1
#define FILE_TYPE 2
#define ALL_TYPE 3

typedef struct DIRECTORY{
    WIN32_FIND_DATAA main_directory;
    char* path;
    unsigned int nr_directories;
    unsigned int max_allowed_dirs;
    struct DIRECTORY** sub_directories;
    struct DIRECTORY* parrent_directory;
}DIRECTORY;

typedef struct DIRECTORY_LIST {
    unsigned int nr_directories;
    unsigned int max_allowed_dirs;
    DIRECTORY** directories_list;
}DIRECTORY_LIST;


/// DIRECTORY
DIRECTORY* directory_new(char* path, DIRECTORY* parrent);
boolean is_dir(const WIN32_FIND_DATAA* data);
int get_data_for_current_directory(char* path, WIN32_FIND_DATAA* data);
void directory_add_dir(DIRECTORY* directories, DIRECTORY *dir);
void directory_expand_sub_dir_size(DIRECTORY* directories);
void directory_free(DIRECTORY* directories);

boolean directory_make_from_path_recursive(DIRECTORY* directories, char* path, int recursion);
DIRECTORY* directory_make_music_dir(char* path);
DIRECTORY* directory_find_by_name(DIRECTORY* directories, char* name);

/// DIRECTORIES_LIST
DIRECTORY_LIST* directory_list_new();
void directory_list_add_dir(DIRECTORY_LIST* directories_list, DIRECTORY* directories);
void directory_list_add_dir_list(DIRECTORY_LIST* final_list, DIRECTORY_LIST* list);
void directory_list_expand_by_value(DIRECTORY_LIST* directories_list, int value);
void expand_directories_list(DIRECTORY_LIST* directories_list);
void direct_transfer_directories_to_list(DIRECTORY* directories, DIRECTORY_LIST* directories_list);
void copy_from_list_to_list(DIRECTORY_LIST* source, DIRECTORY_LIST* destination);
int add_named_files_from_dir_to_list(DIRECTORY* directories, char* name, int type, DIRECTORY_LIST* directories_list);
int add_named_files_from_dir_list_to_list(DIRECTORY_LIST* directories, char* name, int type, DIRECTORY_LIST* directories_list);
void add_all_sub_dirs_to_list(DIRECTORY* directories, DIRECTORY_LIST* directories_list);
int free_directories_list(DIRECTORY_LIST* dir);


/// STRING
char* get_dir_name_from_path(char* path);
char* get_new_path_from_dirpath(char* path, char* name);
char* get_dirpath_from_filepath(char* filepath);
boolean get_named_file_from_dir(DIRECTORY* directories, char* band_name, DIRECTORY_LIST* directories_list);
boolean str_starts_with(char* str1, char* str2);
boolean str_ends_with(char* main_str, char* str);


/// TAKE
boolean take_file_based_on_type(DIRECTORY* dir, int type);
boolean take_file_based_on_name(DIRECTORY* dir, char* name);
int get_nr_digits(int number);
char* get_file_index_number_string(int number, int nr_digits);

#endif //DIRECTORY_H
