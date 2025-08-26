//
// Created by Ionut on 8/5/2025.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "string_helper_functions.h"

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