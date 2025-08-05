//
// Created by Ionut on 8/4/2025.
//
#include "directory.h"
#ifndef ENV_H
#define ENV_H

#define DEFAULT_SIZE 16

typedef struct ENV_VARIABLES{
    char out_path[MAX_STRING_LENGHT];
    char yt_dl_path[MAX_STRING_LENGHT];
    char main_dir_path[MAX_STRING_LENGHT];
}ENV_VARIABLES;

typedef struct CURR_VARIABLES{
    char band[MAX_STRING_LENGHT];
    char album[MAX_STRING_LENGHT];
    char song[MAX_STRING_LENGHT];
    DIRECTORY* dir;
    DIRECTORY_LIST* dir_list;
}CURR_VARIABLES;

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
    CURR_VARIABLES curr_variables;
    ABV_VARIABLES abv_variables;
}ALL_VARIABLES;
int set_string(char* string, char* new_string);

void print_environment_variables(ENV_VARIABLES* env_variables);
void printf_current_variables(CURR_VARIABLES* curr_variables);
void print_abv_variables(ABV_VARIABLES* abv_variables);
void print_all_variables(ALL_VARIABLES* all_variables);

int get_string_index_from_abv(ABV_VARIABLES* abv_variables, char* abv);
void expand_abv_variables(ABV_VARIABLES* abv_variables);
int add_abv_vars(ABV_VARIABLES* abv_variables, char* abv, char* string);
int rm_abv_var(ABV_VARIABLES* abv_variables, char* abv);


#endif //ENV_H
