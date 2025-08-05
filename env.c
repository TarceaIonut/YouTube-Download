#include "directory.h"
#include "env.h"
#include <stdio.h>





int rm_abv_var(ABV_VARIABLES* abv_variables, char* abv) {
    int x = get_string_index_from_abv(abv_variables, abv);
    if (x == -1) return 0;
    free(abv_variables->variable_list[x]);
    abv_variables->variable_list[x] = NULL;
    return 1;
}
int get_string_index_from_abv(ABV_VARIABLES* abv_variables, char* abv) {
    for (int i = 0; i < abv_variables->size; i++) {
        if (abv_variables->variable_list[i] != NULL) {
            if (strcasecmp(abv_variables->variable_list[i]->abv, abv) == 0) {
                return i;
            }
        }
    }
    return -1;
}
void expand_abv_variables(ABV_VARIABLES* abv_variables) {
    abv_variables->size += DEFAULT_SIZE;
    ABV_VARIABLE** vars = malloc(abv_variables->size * sizeof(ABV_VARIABLE*));
    free(abv_variables->variable_list);
    abv_variables->variable_list = vars;
}
int add_abv_vars(ABV_VARIABLES* abv_variables, char* abv, char* string) {
    if (get_string_index_from_abv(abv_variables, abv) != -1) return 0;
    if (abv_variables->size >= abv_variables->max_size) {
        expand_abv_variables(abv_variables);
    }
    strcpy(abv_variables->variable_list[abv_variables->size]->abv, abv);
    strcpy(abv_variables->variable_list[abv_variables->size]->string, string);
    abv_variables->size++;
    return 1;
}



int set_string(char* string, char* new_string) {
    if (strlen(new_string) > MAX_STRING_LENGHT) return 0;
    strcpy(string, new_string);
    return 1;
}

void print_environment_variables(ENV_VARIABLES* env_variables) {
    printf("Environment variables:\n");
    printf("out_path: %s\n", env_variables->out_path);
    printf("yt_dl_path: %s\n", env_variables->yt_dl_path);
    printf("main_dir_path: %s\n", env_variables->main_dir_path);
}
void printf_current_variables(CURR_VARIABLES* curr_variables) {
    printf("Current variables:\n");
    printf("band: %s\n", curr_variables->band);
    printf("album: %s\n", curr_variables->album);
    printf("song: %s\n", curr_variables->song);
}
void print_abv_variables(ABV_VARIABLES* abv_variables) {
    printf("Abv variables:\n");
    for (int i = 0; i < abv_variables->size; i++) {
        if (abv_variables->variable_list[i] != NULL) {
            printf("abv: %s -> string:%s", abv_variables->variable_list[i]->abv, abv_variables->variable_list[i]->string);
        }
    }
}
void print_all_variables(ALL_VARIABLES* all_variables) {
    print_environment_variables(&all_variables->env_variables);
    printf_current_variables(&all_variables->curr_variables);
    print_abv_variables(&all_variables->abv_variables);
}