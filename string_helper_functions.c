//
// Created by Ionut on 8/5/2025.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "string_helper_functions.h"

#include <Windows.h>

char* helper_get_quotation_string(int *poz, int argc, char** argv, int string_length, char string[]) {
    if (*poz >= argc) {
        return NULL;
    }
    if (argv[*poz][0] != '\"') {
        *poz += 1;
        return argv[*poz - 1];
    }
    int current_string_length = 0;
    int i;
    for (i = *poz; i < argc; i++) {
        int len = strlen(argv[i]);
        if (current_string_length + len <= string_length) {
            strcpy(string + current_string_length, argv[i]);
            current_string_length += len;
        }
        if (argv[*poz][len - 1] == '\"') {
            string[current_string_length] = 0;
            break;
        }
    }
    *poz = i;
    return string;
}
int helper_change_path_with_new_name(char** path, char* new_name){
    if (path == NULL || new_name == NULL) {
        return 0;
    }
    int path_len = strlen(*path);
    int new_name_len = strlen(new_name);
    int poz = -1;
    for (int i = path_len - 1; i >= 0; i--) {
        if ((*path)[i] == '/' || (*path)[i] == '\\') {
            poz = i;
            break;
        }
    }
    char* new_path = malloc(poz + 2 + new_name_len);
    strncpy(new_path, *path, poz + 1);
    strcpy(new_path + poz + 1, new_name);
    // printf("free\n");
    // free(*path);
    // printf("free\n");
    *path = new_path;
    return 1;
}
int helper_concat_strings_to_path(char path[], char** strings, int nr_strings) {
    int current_poz = 0;
    for (int i = 0; i < nr_strings; i++) {
        int len = strlen(strings[i]);
        strcpy(path + current_poz, strings[i]);
        current_poz += len;
        path[current_poz++] = '/';
    }
    return current_poz;
}
int cut_chars_from_string(char* string, int poz, int size) {
    int len = strlen(string);
    if (len <= poz + size) {
        return -1;
    }
    for (int i = poz; i < len - size; i++) {
        string[i] = string[i + size];
    }
    string[len - size] = 0;
    return size;
}
void new_command(char*** commands, int* nr_commands, int* max_nr_commands) {
    (*nr_commands)++;
    if (*nr_commands > *max_nr_commands) {
        *max_nr_commands += *nr_commands;
    }
    char** temp = realloc(*commands, *max_nr_commands * sizeof(char*));
    *commands = temp;
}
void set_command(char** commands, int nr_commands, int offset, int size, char* string) {
    commands[nr_commands - 1] = malloc(size + 1);
    memcpy(commands[nr_commands - 1], string + offset, size);
    commands[nr_commands - 1][size] = 0;
}
int helper_get_commands_from_string(char* string, char*** commands, int* nr_commands) {
    *commands = NULL;
    *nr_commands = 0;
    int max_nr_commands = 0;
    boolean in_quotes = 0;
    int current_command_off = 0;
    int current_command_size = 0;
    int string_len = strlen(string);
    boolean start_command = 0;
    for (int i = 0; i < string_len; i++) {
        if (string[i] == '\"') {
            if (in_quotes) {
                in_quotes = 0;
                set_command(*commands, *nr_commands, current_command_off, i - current_command_off, string);
            }
            else {
                in_quotes = 1;
                current_command_off = i + 1;
                new_command(commands, nr_commands, &max_nr_commands);
            }
        }
        else if (string[i] == ' ' && !in_quotes && start_command) {
            start_command = 0;
            set_command(*commands, *nr_commands, current_command_off, i - current_command_off, string);
        }
        else if (string[i] != ' ' && !in_quotes && !start_command) {
            start_command = 1;
            current_command_off = i;
            new_command(commands, nr_commands, &max_nr_commands);
        }
    }
    if (!in_quotes && start_command) {
        start_command = 0;
        set_command(*commands, *nr_commands, current_command_off, string_len - current_command_off, string);
    }
    return *nr_commands;
}
