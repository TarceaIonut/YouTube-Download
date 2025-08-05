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

typedef struct DIRECTORIES{
    WIN32_FIND_DATAA main_directory;
    char* path;
    unsigned int nr_directories;
    unsigned int max_allowed_dirs;
    struct DIRECTORIES** sub_directories;
    struct DIRECTORIES* parrent_directory;
}DIRECTORIES;

typedef struct DIRECTORIES_LIST {
    unsigned int nr_directories;
    unsigned int max_allowed_dirs;
    DIRECTORIES** directories_list;
}DIRECTORIES_LIST;


/// DIRECTORY
DIRECTORIES* new_directory(char* path, DIRECTORIES* parrent);
boolean is_dir(const WIN32_FIND_DATAA* data);
WIN32_FIND_DATAA get_data_for_current_directoty(char* path);
void add_directory(DIRECTORIES* directories, DIRECTORIES *dir);
void directories_expand_sub_dir_size(DIRECTORIES* directories);
void free_directories(DIRECTORIES* directories);

boolean recursive_make_directories_from_path(DIRECTORIES* directories, char* path, int recursion);
DIRECTORIES* make_music_directory(char* path);
DIRECTORIES* find_directories_by_name(DIRECTORIES* directories, char* name);

/// DIRECTORIES_LIST
DIRECTORIES_LIST* new_directories_list();
void add_to_directories_list(DIRECTORIES_LIST* directories_list, DIRECTORIES* directories);
void expand_directories_list(DIRECTORIES_LIST* directories_list);
void direct_transfer_directories_to_list(DIRECTORIES* directories, DIRECTORIES_LIST* directories_list);
void copy_from_list_to_list(DIRECTORIES_LIST* source, DIRECTORIES_LIST* destination);
int add_named_files_from_dir_to_list(DIRECTORIES* directories, char* name, int type, DIRECTORIES_LIST* directories_list);
int add_named_files_from_dir_list_to_list(DIRECTORIES_LIST* directories, char* name, int type, DIRECTORIES_LIST* directories_list);
void add_all_sub_dirs_to_list(DIRECTORIES* directories, DIRECTORIES_LIST* directories_list);
int free_directories_list(DIRECTORIES_LIST* dir);


/// STRING
char* get_dir_name_from_path(char* path);
char* get_new_path_from_dirpath(char* path, char* name);
char* get_dirpath_from_filepath(char* filepath);
boolean get_named_file_from_dir(DIRECTORIES* directories, char* band_name, DIRECTORIES_LIST* directories_list);
boolean str_starts_with(char* str1, char* str2);
boolean str_ends_with(char* main_str, char* str);


/// TAKE
boolean take_file_based_on_type(DIRECTORIES* dir, int type);
boolean take_file_based_on_name(DIRECTORIES* dir, char* name);
int get_nr_digits(int number);
char* get_file_index_number_string(int number, int nr_digits);

#endif //DIRECTORY_H
