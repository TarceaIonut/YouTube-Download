//
// Created by Ionut on 8/5/2025.
//

#include <stdlib.h>
#include <string.h>

#include "string_helper_functions.h"

char* helper_get_quotation_string(int *poz, int argc, char** argv, int string_length, char string[]) {
    if (*poz >= argc) {
        return nullptr;
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
