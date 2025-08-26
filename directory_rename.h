//
// Created by Ionut on 8/4/2025.
//
#include "directory.h"

#ifndef DIRECTORY_RENAME_H
#define DIRECTORY_RENAME_H

int rename_directory(DIRECTORY* dir, char* new_name);
char* get_name_with_number(DIRECTORY* dir, char* name, int number, int nr_digits);
char* create_new_path_from_full_path(char* full_path, char* name);
int rename_directory_with_number(DIRECTORY* dir, char* name, int number, int nr_digits);
int order_directory_list(DIRECTORY_LIST* dir_list, int numbers[], int nr_numbers);
void cut_chars_from_directory_list(DIRECTORY_LIST* dir_list, int poz, int size);
void cut_chars_from_directory(DIRECTORY* dir, int poz, int size);

#endif //DIRECTORY_RENAME_H
