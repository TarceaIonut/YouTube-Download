#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "directory.h"
#include "env.h"
#include "directory_search.h"
#include "directory_rename.h"


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
            get_all_correct_dirs(all_variables.curr_variables.dir, all, dirs_to_list);
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
        directory_list_free_dirs(&dirs_from_p);
        directory_list_free(&dirs_to_list);
    }
}
void var_ui(int arg, char **argv) {

}
void ui(int arg, char* args[]){
    if (arg == 0) {
        printf("more params needed\n");
        return;
    }
    if (strcasecmp(args[0], "show") == 0) {
        ui_show(arg - 1, args + 1);
    }
    else if (strcasecmp(args[0], "var") == 0) {
        var_ui(arg - 1, args + 1);
    }

}
int set_up_env_vars() {

    return 1;
}

int main(int argc, char *argv[]){

    all_variables.curr_variables.dir = directory_make_music_dir("D:/Music");
    ui(argc - 1, argv + 1);
    // printf("%d\n", argc);
    // for (int i = 0; i < argc; i++) {
    //     printf("%s\n", argv[i]);
    // }
}