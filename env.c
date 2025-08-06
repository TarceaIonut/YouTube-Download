#include "directory.h"
#include "env.h"
#include <stdio.h>

#include "string_helper_functions.h"

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
    abv_variables->max_size += DEFAULT_SIZE;
    ABV_VARIABLE** vars = malloc(abv_variables->max_size * sizeof(ABV_VARIABLE*));
    if (abv_variables->variable_list != NULL) {
        memcpy(vars, abv_variables->variable_list, sizeof(ABV_VARIABLE*) * abv_variables->size);
        free(abv_variables->variable_list);
    }
    abv_variables->variable_list = vars;
}
ABV_VARIABLE* abv_variable_new(char* abv, char* string) {
    ABV_VARIABLE* abv_variable = malloc(sizeof(ABV_VARIABLE));
    strcpy(abv_variable->abv, abv);
    strcpy(abv_variable->string, string);
    return abv_variable;
}
int add_abv_vars(ABV_VARIABLES* abv_variables, char* abv, char* string) {
    if (get_string_index_from_abv(abv_variables, abv) != -1) return 0;
    if (abv_variables->size >= abv_variables->max_size) {
        expand_abv_variables(abv_variables);
    }
    abv_variables->variable_list[abv_variables->size] = abv_variable_new(abv, string);
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
    //printf("out_path: %s\n", env_variables->out_path);
    printf("yt_dl_path: %s\n", env_variables->yt_dlp_path);
    printf("main_dir_path: %s\n", env_variables->main_dir_path);
}
void printf_current_variables(TEMPORARY_VARIABLES* curr_variables) {
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
    printf_current_variables(&all_variables->temporary_variables);
    print_abv_variables(&all_variables->abv_variables);
    all_variables->abv_variables.size = 0;
    all_variables->abv_variables.variable_list = nullptr;
    all_variables->abv_variables.max_size = 0;
}
int read_all_variables(ALL_VARIABLES* all_variables) {
    int errors = 0;
    FILE* file_env_variables_path = fopen(all_variables->env_variables.env_variables_path, "r");

    if (file_env_variables_path == NULL) errors |= ERROR_env_variables_path;

    if (errors > 0) return errors;

    char string1[MAX_STRING_LENGHT];
    char string2[MAX_STRING_LENGHT];
    char string3[MAX_STRING_LENGHT];
    int string1_rez = 0;
    int string2_rez = 0;
    int string3_rez = 0;
    while (!feof(file_env_variables_path)) {
        string1_rez = fscanf(file_env_variables_path, "%s", string1);
        string2_rez = fscanf(file_env_variables_path, "%s", string2);
        if (string1_rez != 1 || string2_rez != 1) break;

        if (strcasecmp(string1, "abv") == 0) {
            add_abv_vars(&all_variables->abv_variables, string2, string3);
        }else if (strcasecmp(string1, "yt_dl_path") == 0) {
            strcpy(all_variables->env_variables.yt_dlp_path, string2);
        }else if (strcasecmp(string1, "main_dir_path") == 0) {
            strcpy(all_variables->env_variables.main_dir_path, string2);
        }else {
            printf("command idk man:%s\n", string1);
            break;
        }
    }
    return errors;
}
int verify_all_variables(ALL_VARIABLES* all_variables) {
    int errors = 0;
    if (!check_file(all_variables->env_variables.yt_dlp_path, FILE_TYPE)) errors |= ERROR_yt_dl_path;
    // if (check_file(all_variables->env_variables.ffmpeg_path, FILE_TYPE)) errors |= ERROR_ffmpeg_path;
    // if (check_file(all_variables->env_variables.out_path, DIR_TYPE)) errors |= ERROR_out_path;
    if (!check_file(all_variables->env_variables.main_dir_path, DIR_TYPE)) errors |= ERROR_main_dir_path;

    return errors;
}
void deal_with_errors(int errors) {
    if (errors & ERROR_env_variables_path) {
        printf("Environment variables path incorrect:\n");
    }else if (errors & ERROR_yt_dl_path) {
        printf("yt-dlp path incorrect:\n");
    // }else if (errors & ERROR_ffmpeg_path) {
    //     printf("ffmpeg path incorrect:\n");
    }else if (errors & ERROR_main_dir_path) {
        printf("Main output path incorrect:\n");
    }
}
int init_all_vars(ALL_VARIABLES* all_variables, char* full_exe_path) {
    char path[MAX_STRING_LENGHT];
    strcpy(path, full_exe_path);
    int len = strlen(full_exe_path);
    for (int i = len-1; i >= 0; i--) {
        if (path[i] == '\\') {
            path[i] = 0;
            break;
        }
    }
    return init_all_vars_from_current_path(all_variables, path);
}
int init_all_vars_from_current_path(ALL_VARIABLES* all_variables, char* current_path){
    sprintf(all_variables->env_variables.env_variables_path, "%s\\%s", current_path, "env.txt");
    sprintf(all_variables->env_variables.yt_dlp_path, "%s\\%s", current_path, "yt-dlp.exe");
    //sprintf(all_variables->env_variables.ffmpeg_path, "%s/%s", current_path, "ffmpeg.exe");

    int errors = read_all_variables(all_variables);

    if (errors > 0) return errors;

    errors = verify_all_variables(all_variables);

    if (errors > 0) return errors;

    all_variables->temporary_variables.dir = directory_make_music_dir(all_variables->env_variables.main_dir_path);

    return 0;
}
boolean check_file(char* file_path, int type) {
    DWORD attributes = GetFileAttributesA(file_path);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return FALSE;
    }
    if (attributes & FILE_ATTRIBUTE_DIRECTORY && (type & DIR_TYPE)) {
        return TRUE;
    }
    if (!(attributes & FILE_ATTRIBUTE_DIRECTORY) && (type & FILE_TYPE)) {
        return TRUE;
    }
    return FALSE;
}
