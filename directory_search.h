//
// Created by Ionut on 8/4/2025.
//

#ifndef DIRECTORY_SEARCH_H
#define DIRECTORY_SEARCH_H


#define S_NAME 0x10
#define S_PATH 0x20
#define S_TYPE 0x40
#define S_NUMBER 0x80

#define PRINT_DIRECTORY_2(a,b) print_directory(a,b,0,-1,-1)
#define PRINT_DIRECTORY_3(a,b,c) print_directory(a,b,c,-1,-1)
#define PRINT_DIRECTORY_4(a,b,c,d) print_directory(a,b,0,c,d)
#define PRINT_DIRECTORY_5(a,b,c,d,e) print_directory(a,b,c,d,e)

#define CONCAT_IMPL(A, B) A ## B
#define CONCAT(a,b) CONCAT_IMPL(a,b)

#define GET_NR_PARAMS_IMPL(_1,_2,_3,_4,_5,NR,...) NR
#define GET_NR_PARAMS(...) GET_NR_PARAMS_IMPL(__VA_ARGS__,5,4,3,2,1,0)

#define PRINT_DIRECTORY(...) CONCAT(PRINT_DIRECTORY_, GET_NR_PARAMS(__VA_ARGS__))(__VA_ARGS__)

typedef struct ALL {
    int choice;
    int depth;
    char** names;
}ALL;
void free_ALL(ALL* new);
ALL* new_ALL_from_string(char c, char* b, char* a, char* s);
int find_if_correct_check_below(DIRECTORIES* potential_dir, ALL* all, int start);
int get_correct_files_check_above(DIRECTORIES* music, ALL* all, DIRECTORIES_LIST* directories_list);
int print_directory(DIRECTORIES* data, int what_to_show, int depth, int number, int nr_digits);
int get_all_correct_dirs(DIRECTORIES* music, ALL* all, DIRECTORIES_LIST* directories_list);
void recursive_show_directories(DIRECTORIES* directories, int recursion_number, int depth, int what_to_show);

#endif //DIRECTORY_SEARCH_H
