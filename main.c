#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#include "directory.h"
#include "env.h"
#include "directory_search.h"
#include "directory_rename.h"
#include "string_helper_functions.h"
#include "yt_download.h"

ALL_VARIABLES all_variables;

void help_ui() {
    printf("HELP UI\n");
    printf("0: yt-dlp - download with yt-dlp\n");
    printf("1: dl - download whi Youtube-Download\n");
    printf("2: show - show files\n");
    printf("3: rename/move - rename files\n");
    printf("4: remove - remove files\n");
    printf("5: organize - order files\n");
    printf("6: env/curr - show enviroment or current variables\n");
    printf("8: ui mode\n");
    printf("9: exit\n");
}



#define FLAGS_TOUCHED    0x0100
#define RECURSIVE_FLAG   0x0200
#define DIRS_SELECT_FLAG 0x0400
#define ERROR_NULL_DIR   0x0800
#define ERROR_NO_PATH    0x1000
#define ERROR_NO_BAS     0x2000
#define ERROR_INCORRECT_PATH 0x4000
#define ERROR_ALL_CHOISE 0x8000
#define DEFAULT_FLAGS FILE_TYPE | DIR_TYPE | S_NUMBER | S_NAME;

void ui(int arg, char* args[]);

void handle_errors_show(int flag) {
    if (flag & ERROR_NULL_DIR) {
        printf("directory not known\n");
    }else if (flag & ERROR_NO_PATH) {
        printf("a path must be provided\n");
    }else if (flag & ERROR_NO_BAS) {
        printf("insufficient arguments\n");
    }else if (flag & ERROR_INCORRECT_PATH) {
        printf("incorrect path\n");
    }else if (flag & ERROR_ALL_CHOISE) {
        printf("unrecognized choise: -b,-a,-s\n");
    }
}
void ui_show(int arg, char **argv) {
    int poz = 0;
    DIRECTORY_LIST* dirs_to_list = directory_list_new();
    DIRECTORY_LIST* dirs_from_p = directory_list_new();
    int flags = 0;

    while (poz < arg) {
        if (strcasecmp(argv[poz], "-r") == 0) {
            flags |= RECURSIVE_FLAG;
            flags |= FLAGS_TOUCHED;
            poz++;
        }else if (strcasecmp(argv[poz], "-d") == 0) {
            flags |= DIR_TYPE;
            flags |= FLAGS_TOUCHED;
            poz++;
        }else if (strcasecmp(argv[poz], "-f") == 0) {
            flags |= FILE_TYPE;
            flags |= FLAGS_TOUCHED;
            poz++;
        }else if (strcasecmp(argv[poz], "-p") == 0) {
            poz++;
            if (poz == arg) {
                flags |= ERROR_NO_PATH;
                break;
            }
            char* path = argv[poz];
            poz++;
            DIRECTORY* dir = directory_new(path, NULL);
            if (dir == NULL) {
                flags |= ERROR_INCORRECT_PATH;
                break;
            }
            directory_make_from_path_recursive(dir, path, 1);
            add_all_sub_dirs_to_list(dir, dirs_to_list);
            flags |= DIRS_SELECT_FLAG;
        }else if (strcasecmp(argv[poz], "-m") == 0) {
            if (poz + 5 > arg) {
                flags |= ERROR_NO_BAS;
                break;
            }
            ALL* all = new_ALL_from_string(argv[poz+1][0], argv[poz+2], argv[poz+3], argv[poz+4]);
            get_all_correct_dirs(all_variables.temporary_variables.working_dir, all, dirs_to_list);
            poz += 5;
        }else {
            printf("Unknown option: %s\n", argv[poz]);
            return;
        }
    }
    if (flags >= ERROR_NULL_DIR) {
        handle_errors_show(flags);
        return;
    }
    if (!(flags & FLAGS_TOUCHED)) {
        flags = DEFAULT_FLAGS;
    }
    {
        directory_list_add_dir_list(dirs_to_list, dirs_from_p);
        int nr_digits = get_nr_digits(dirs_to_list->nr_directories);
        for (int i = 0; i < dirs_to_list->nr_directories; i++) {
            PRINT_DIRECTORY(dirs_to_list->directories_list[i], S_NAME | S_NUMBER | flags | S_TYPE, i, nr_digits);
        }
        int numbers[15] = {0, 10, 3, 14, 12, 11, 13, 8, 4, 7, 5, 1, 6, 9, 2};
        order_directory_list(dirs_to_list, numbers, 15);

        directory_list_free_dirs(&dirs_from_p);
        directory_list_free(&dirs_to_list);
    }
}
void var_ui(int arg, char **argv) {

}
enum FUNCTIONS {
    F_SHOW = 1,
    F_ORDER = 2,
    F_REMOVE = 3,
    F_RENAME = 4,
    F_MOVE = 5,
    F_CUT = 6,
    F_SAVE_LIST = 0x100,
    F_SAVE_DIR = 0x200
};
int p_command(DIRECTORY** directory, DIRECTORY_LIST* directory_list, char* path) {
    DIRECTORY* dir = directory_new(path, NULL);
    if (dir == NULL) {
        return ERROR_INCORRECT_PATH;
    }
    *directory = dir;
    directory_make_from_path_recursive(dir, path, 1);
    add_all_sub_dirs_to_list(dir, directory_list);
    return 0;
}
boolean get_number_from_string(int *number, char* string) {
    char* endptr;
    *number = strtol(string, &endptr, 10);

    if (errno == ERANGE) {

    }
    else if (endptr == string || *endptr != '\0') {

    }
    else {
        return 1;
    }
    return 0;
}

void dir_ui(int arg, char** argv) {
    int function = 0;
    int multiple_functions = 0;
    char* numbers_for_order = NULL;
    int poz = 0;
    char *cut_poz, *cut_size;
    DIRECTORY* directory = NULL;
    DIRECTORY_LIST* directory_list = directory_list_new();
    int what_to_print = S_NAME | S_NUMBER | S_TYPE | DIR_TYPE | FILE_TYPE;
    while (poz < arg) {
        if (strcasecmp(argv[poz], "-p") == 0) {
            if (poz + 2 > arg) break;
            p_command(&directory, directory_list, argv[poz + 1]);
            poz += 2;
        }else if (strcasecmp(argv[poz], "-m") == 0) {
            if (poz + 5 > arg) {
                break;
            }
            ALL* all = new_ALL_from_string(argv[poz+1][0], argv[poz+2], argv[poz+3], argv[poz+4]);
            get_all_correct_dirs(all_variables.temporary_variables.working_dir, all, directory_list);
            free_ALL(all);
            poz += 5;
        }else if (strcasecmp(argv[poz], "-saveL") == 0) {
            multiple_functions |= F_SAVE_LIST;
        }
        else if (strcasecmp(argv[poz], "-saveD") == 0) {
            multiple_functions |= F_SAVE_DIR;
        }
        else if (strcasecmp(argv[poz], "--show") == 0) {
            function = F_SHOW;
            poz++;
        }
        else if (strcasecmp(argv[poz], "-currL") == 0) {
            DIRECTORY_LIST* Tlist = all_variables.temporary_variables.temporary_dir_list;
            if (Tlist != NULL && Tlist->nr_directories != 0) {
                directory_list_add_dir_list(directory_list, Tlist);
            }
            poz++;
        }
        else if (strcasecmp(argv[poz], "-currD") == 0) {
            DIRECTORY* Tdir = all_variables.temporary_variables.temporary_dir;
            if (Tdir != NULL) {
                directory = Tdir;
            }
            poz++;
        }
        else if (strcasecmp(argv[poz], "-currSD") == 0) {
            DIRECTORY* Tdir = all_variables.temporary_variables.temporary_dir;
            if (Tdir != NULL) {
                DIRECTORY_LIST* Tlist = Tdir->directory_list;
                if (Tlist != NULL && Tlist->nr_directories != 0) {
                    directory_list_add_dir_list(directory_list, Tlist);
                }
            }
            poz++;
        }
        else if (strcasecmp(argv[poz], "--order") == 0) {
            if (poz + 2 > arg) {
                break;
            }
            function = F_ORDER;
            numbers_for_order = argv[poz+1];
            poz += 2;
        }else if (strcasecmp(argv[poz], "--cut") == 0) {
            if (poz + 3 > arg) {
                break;
            }
            cut_poz = argv[poz+1];
            cut_size = argv[poz+2];
            function = F_CUT;
            poz += 3;
        }
        else if (strcasecmp(argv[poz], "-s_path") == 0) {
            if (poz + 2 > arg) {
                break;
            }
            if (strcasecmp(argv[poz+1], "0") == 0) what_to_print &= ~S_PATH;
            else what_to_print |= S_PATH;
            poz += 2;
        }
        else if (strcasecmp(argv[poz], "-s_number") == 0 || strcasecmp(argv[poz], "-s_nr") == 0) {
            if (poz + 2 > arg) {
                break;
            }
            if (strcasecmp(argv[poz+1], "0") == 0) what_to_print &= ~S_NUMBER;
            else what_to_print |= S_NUMBER;
            poz += 2;
        }
        else if (strcasecmp(argv[poz], "-s_type") == 0) {
            if (poz + 2 > arg) {
                break;
            }
            if (strcasecmp(argv[poz+1], "0") == 0) what_to_print &= ~S_TYPE;
            else what_to_print |= S_TYPE;
            poz += 2;
        }
        else if (strcasecmp(argv[poz], "-s_name") == 0) {
            if (poz + 2 > arg) {
                break;
            }
            if (strcasecmp(argv[poz+1], "0") == 0) what_to_print &= ~S_NAME;
            else what_to_print |= S_NAME;
            poz += 2;
        }
        else if (strcasecmp(argv[poz], "-s_file") == 0) {
            if (poz + 2 > arg) {
                break;
            }
            if (strcasecmp(argv[poz+1], "0") == 0) what_to_print &= ~FILE_TYPE;
            else what_to_print |= FILE_TYPE;
            poz += 2;
        }
        else if (strcasecmp(argv[poz], "-s_folder") == 0 || strcasecmp(argv[poz], "-s_dir") == 0  ) {
            if (poz + 2 > arg) {
                break;
            }
            if (strcasecmp(argv[poz+1], "0") == 0) what_to_print &= ~DIR_TYPE;
            else what_to_print |= DIR_TYPE;
            poz += 2;
        }
        else {
            printf("Unknown option: %s\n", argv[poz]);
            poz++;
        }

    }
    if (multiple_functions & F_SAVE_LIST) {
        directory_list_free(&all_variables.temporary_variables.temporary_dir_list);
        all_variables.temporary_variables.temporary_dir_list = directory_list;
    }
    if (multiple_functions & F_SAVE_DIR) {
        directory_free(&all_variables.temporary_variables.temporary_dir);
        all_variables.temporary_variables.temporary_dir = directory;
    }
    if (function == F_CUT) {
        int poz;
        int size;
        if (get_number_from_string(&poz, cut_poz) && get_number_from_string(&size, cut_size)) {
            cut_chars_from_directory_list(directory_list, poz, size);
        }
    }
    if (function == F_SHOW) {
        int nr_digits = get_nr_digits(directory_list->nr_directories);
        for (int i = 0; i < directory_list->nr_directories; i++) {
            PRINT_DIRECTORY(directory_list->directories_list[i], what_to_print, i, nr_digits);
        }
    }else if (function == F_ORDER) {
        int numbers[directory_list->nr_directories];
        int nr_numbers = 0;
        char* token = strtok(numbers_for_order, " ");
        char *endptr;
        while (token != NULL && nr_numbers < directory_list->nr_directories) {

            if (get_number_from_string(numbers + nr_numbers, token)) {
                nr_numbers++;
            }

            token = strtok(NULL, " ");
        }
        order_directory_list(directory_list, numbers, nr_numbers);
    }
    if (multiple_functions & F_SAVE_LIST == 0) {
        directory_list_free(&directory_list);
    }
    if (multiple_functions & F_SAVE_DIR == 0) {
        directory_free(&directory);
    }
}
void dl_ui(int arg, char** argv) {
    DL_INFO dl_info;
    dl_info_init(&dl_info);

    int poz = 0;
    while (poz < arg) {
        if ((int)strcasecmp(argv[poz], "simple") == 0) {
            if (poz + 4 > arg) {
                break;
            }
            // char band[MAX_STRING_LENGHT];
            // char album[MAX_STRING_LENGHT];
            // char url[MAX_STRING_LENGHT];


            // fgets(band, sizeof(band), stdin);
            // fgets(album, sizeof(album), stdin);
            // fgets(url, sizeof(url), stdin);

            char* band;
            char* album;
            char* url;

            band = argv[poz+1];
            album = argv[poz+2];
            url = argv[poz+3];

            dl_info.flags |= ORDER;
            dl_info.search_string = url;

            dl_info.nr_path_strings = 2;
            if (dl_info.path_strings != NULL) free(dl_info.path_strings);
            dl_info.path_strings = malloc(2 * sizeof(char*));
            dl_info.path_strings[0] = band;
            dl_info.path_strings[1] = album;

            break;
        }
        if (strcasecmp(argv[poz], "-u") == 0) {
            if (poz + 2 > arg) {
                break;
            }
            dl_info.search_string = argv[poz+1];
            poz += 2;
        }
        if (strcasecmp(argv[poz], "-op") == 0) {
            if (poz + 2 > arg) {
                break;
            }
            dl_info.overwrite_options = argv[poz+1];
            poz += 2;
        }
        else if (strcasecmp(argv[poz], "-o") == 0) {
            if (poz + 2 > arg) {
                break;
            }
            dl_info.overwrite_output_format = argv[poz+1];
            poz += 2;
        }
        else if (strcasecmp(argv[poz], "-t") == 0) {
            if (poz + 2 > arg) {
                break;
            }
            dl_info.overwrite_title = argv[poz+1];
            poz += 2;
        }
        else if (strcasecmp(argv[poz], "--search") == 0) {
            dl_info.flags |= SEARCH;
            poz++;
        }
        else if (strcasecmp(argv[poz], "--order") == 0) {
            dl_info.flags |= ORDER;
            poz++;
        }
        else if (strcasecmp(argv[poz], "-url") == 0) {
            if (poz + 2 > arg) {
                break;
            }
            dl_info.search_string = argv[poz+1];
            poz += 2;
        }
        else if (strcasecmp(argv[poz], "-start_with") == 0) {
            if (poz + 2 > arg) {
                break;
            }
            dl_info.start_with_string = argv[poz+1];
            poz += 2;
        }
        else if (strcasecmp(argv[poz], "-path") == 0) {
            if (poz + 2 > arg) {
                break;
            }
            int nr = 0;
            while (nr + poz + 1 < arg && strcasecmp(argv[nr + poz + 1], "-end") != 0) {
                nr++;
            }

            dl_info.nr_path_strings = nr;
            if (dl_info.path_strings != NULL) free(dl_info.path_strings);
            dl_info.path_strings = malloc(nr * sizeof(char*));

            for (int i = 0; i < nr; i++) {
                dl_info.path_strings[i] = argv[i + poz + 1];
            }

            poz += nr + 2;
        }
        else if (strcasecmp(argv[poz], "-ba") == 0) {
            if (poz + 3 > arg) {
                break;
            }
            dl_info.nr_path_strings = 2;
            if (dl_info.path_strings != NULL) free(dl_info.path_strings);
            dl_info.path_strings = malloc(2 * sizeof(char*));
            dl_info.path_strings[0] = argv[poz+1];
            dl_info.path_strings[1] = argv[poz+2];
            poz += 3;
        }
        else {
            dl_info.search_string = argv[poz];
            poz++;
        }
    }
    download_from_dl_info(&dl_info, all_variables.env_variables.main_dir_path);
}
int read_and_execute_command(FILE* file) {
    char buffer[1024];
    fgets(buffer, 1024, file);
    char** commands = NULL;
    int commands_nr = 0;
    helper_get_commands_from_string(buffer, &commands, &commands_nr);
    ui(commands_nr, commands);
}

int execute_commands_from_file(char* file_name) {
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        return -1;
    }
    while (!feof(file)) {
        read_and_execute_command(file);
    }

}
int execute_commands_from_input(int nr_commands) {
    for (int i = 0; i < nr_commands; i++) {
        read_and_execute_command(stdin);
    }
}

void ui(int arg, char* args[]){
    if (arg == 0) {
        printf("more params needed\n");
        return;
    }
    if (strcasecmp(args[0], "dir") == 0) {
        dir_ui(arg - 1, args + 1);
    }
    else if (strcasecmp(args[0], "var") == 0) {
        var_ui(arg - 1, args + 1);
    }
    else if (strcasecmp(args[0], "dl") == 0) {
        dl_ui(arg - 1, args + 1);
    }
    else if (strcasecmp(args[0], "fread") == 0) {
        if (arg > 1) {
            execute_commands_from_file(args[1]);
        }
    }
    else if (strcasecmp(args[0], "read") == 0) {
        if (arg > 1) {
            int nr = 0;
            if (get_number_from_string(&nr, args[1])) {
                execute_commands_from_input(nr);
            }
        }
    }
}


int main(int argc, char *argv[]){
    char path[MAX_PATH];
    if (GetModuleFileName(NULL, path, MAX_PATH) == 0) {
        printf("Failed to get executable path. Error code: %lu\n", GetLastError());
        exit(0);
    }
    int rez = init_all_vars(&all_variables, path);
    if (rez > 0) {
        deal_with_errors(rez);
        return 0;
    }
    ui(argc - 1, argv + 1);
}