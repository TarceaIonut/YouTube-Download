//
// Created by Ionut on 8/4/2025.
//
#include "directory.h"
#ifndef ENV_H
#define ENV_H

#define ERROR_env_variables_path 1
#define ERROR_main_dir_path 2


#define DEFAULT_SIZE 16

typedef struct ENV_VARIABLES{
    char default_download_options[MAX_STRING_LENGHT];
    char env_variables_path[MAX_STRING_LENGHT];
    char main_dir_path[MAX_STRING_LENGHT];
}ENV_VARIABLES;

typedef struct TEMPORARY_VARIABLES{
    char band[MAX_STRING_LENGHT];
    char album[MAX_STRING_LENGHT];
    char song[MAX_STRING_LENGHT];
    DIRECTORY* working_dir;
    DIRECTORY* temporary_dir;
    DIRECTORY_LIST* temporary_dir_list;
}TEMPORARY_VARIABLES;

typedef struct ABV_VARIABLE{
    char abv[MAX_STRING_LENGHT];
    char string[MAX_STRING_LENGHT];
}ABV_VARIABLE;
typedef struct ABV_VARIABLES{
    int size;
    int max_size;
    ABV_VARIABLE** variable_list;
}ABV_VARIABLES;

typedef struct ALL_VARIABLES {
    ENV_VARIABLES env_variables;
    TEMPORARY_VARIABLES temporary_variables;
    ABV_VARIABLES abv_variables;
}ALL_VARIABLES;
int set_string(char* string, char* new_string);

void print_environment_variables(ENV_VARIABLES* env_variables);
void printf_current_variables(TEMPORARY_VARIABLES* curr_variables);
void print_abv_variables(ABV_VARIABLES* abv_variables);
void print_all_variables(ALL_VARIABLES* all_variables);

int get_string_index_from_abv(ABV_VARIABLES* abv_variables, char* abv);
void expand_abv_variables(ABV_VARIABLES* abv_variables);
int add_abv_vars(ABV_VARIABLES* abv_variables, char* abv, char* string);
int rm_abv_var(ABV_VARIABLES* abv_variables, char* abv);

int init_all_vars_from_current_path(ALL_VARIABLES* all_variables, char* current_path);
boolean check_file(char* file_path, int type);

void deal_with_errors(int errors);
int read_all_variables(ALL_VARIABLES* all_variables);
int verify_all_variables(ALL_VARIABLES* all_variables);
int init_all_vars(ALL_VARIABLES* all_variables, char* full_exe_path);

#endif //ENV_H
