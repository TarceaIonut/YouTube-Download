//
// Created by Ionut on 8/5/2025.
//

#ifndef STRING_HELPER_FUNCTIONS_H
#define STRING_HELPER_FUNCTIONS_H

char* helper_get_quotation_string(int *poz, int argc, char** argv, int string_length, char string[]);
int helper_change_path_with_new_name(char** path, char* new_name);
int helper_concat_strings_to_path(char path[], char** strings, int nr_strings);

#endif //STRING_HELPER_FUNCTIONS_H
