#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <windows.h>
#include <unistd.h>

#include "yt_download.h"
#include "string_helper_functions.h"

#define default_input_path "D:/Music/urls.txt"
#define default_output_path "D:/Music"
#define default_exec_path "D:/yt-dlp-master/dist/yt-dlp"


#define MAX_STRING_LENGHT 1024

char* current_exec_path = default_exec_path;
char* current_output_path = default_output_path;
char* current_band = NULL;
char* current_album = NULL;

typedef struct _FILE_NAMES{
    char dir_name[MAX_STRING_LENGHT];
    int nr_files;
    char* files[MAX_STRING_LENGHT];
    int file_types[MAX_STRING_LENGHT];
}FILE_NAMES;
int playlist_poz1, playlist_poz2;

void file_names_free(FILE_NAMES* file_names);
void file_names_list(FILE_NAMES* file_names);
int get_all_files_from_dir(FILE_NAMES* file_names, char* dir_path);
int list_dir_contents(char* path);
int check_band();
int check_album();
int download_album(char* band_name, char* album_name, char* url);
int go_to_path(char* path);
int new_wait_process(char* command);
HANDLE new_process(char* command);
void transform_filepath_to_dirpath(char* dirpath, char* filepath);
int get_file_type(WIN32_FIND_DATAA* data);
int add_file(FILE_NAMES* file_names, char* file_name, int type);
void file_names_initializer(FILE_NAMES* file_names, char* dir_name);
void runing_a_command(char* command_cmd);
void create_command_cmd(char* complete_path, int is_playlist, char* result_name, int is_url, char* to_find_video, char* command_cmd);



void file_names_initializer(FILE_NAMES* file_names, char* dir_name){
    file_names->nr_files = 0;
    strcpy(file_names->dir_name, dir_name);
}
int add_file(FILE_NAMES* file_names, char* file_name, int type){
    if (file_names->nr_files == MAX_STRING_LENGHT)
        return 0;
    int len = strlen(file_name) + 1;
    file_names->files[file_names->nr_files] = malloc(len);
    memcpy(file_names->files[file_names->nr_files], file_name, len);
    file_names->file_types[file_names->nr_files] = type;
    file_names->nr_files++;
    return 1;
}
int get_file_type(WIN32_FIND_DATAA* data){
    return data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}
void transform_filepath_to_dirpath(char* dirpath, char* filepath){
    sprintf(dirpath, "%s/*", filepath);
}
HANDLE new_process(char* command){
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    int p = CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (!p) {
        fprintf(stdout, "CreateProcess failed (%lu) %d.\n", GetLastError(), p);
    }
    return pi.hProcess;
}
int new_wait_process(char* command){
    HANDLE h_process = new_process(command);

    WaitForSingleObject(h_process, INFINITE);
}
int go_to_path(char* path){
    char* command = malloc(1024);
    memset(command, 0, 1024);
    sprintf(command, "cd %s", path);
    new_wait_process(command);
}
void create_command_cmd(char* complete_path, int is_playlist, char* result_name, int is_url, char* to_find_video, char* command_cmd){
    char command_cd[MAX_STRING_LENGHT];
    sprintf(command_cd, "cd /d \"%s\"", complete_path);
    char* if_playlist;
    char o_command[MAX_STRING_LENGHT];
    char command_to_find[MAX_STRING_LENGHT];
    char command_ytdl[MAX_STRING_LENGHT];

    if (is_playlist){
        sprintf(o_command, "%s/%%(playlist_index)s - %%(title)s.%%(ext)s", result_name);
        if_playlist = "--yes-playlist";
    } 
    else{
        sprintf(o_command, "%s", result_name);
        if_playlist = "--no-playlist";
    }
    if (is_url){
        sprintf(command_to_find, "%s", to_find_video);
    }
    else{
        sprintf(command_to_find, "tysearch:%s", to_find_video);
    }
    sprintf(command_cd, "cd /d \"%s\"", complete_path);
    sprintf(command_ytdl, "%s %s -o \"%s\" \"%s\"", current_exec_path, if_playlist, o_command, command_to_find);
    sprintf(command_cmd, "%s && %s", command_cd, command_ytdl);
}
void runing_a_command(char* command_cmd){
    int fd = open("D:/ytdl.bat", O_CREAT | O_RDWR, 0777);
    int len = strlen(command_cmd);
    ftruncate(fd, len);
    int nr_w = write(fd, command_cmd, len);
    char* command = "cmd.exe /C \"start \"\" D:/ytdl.bat\"";
    new_process(command);
    close(fd);
    free(command);
    sleep(1);
}
int check_album(){
    if (current_album[0] == 0){
        printf("current_album not selected\n");
    }
    return current_album[0] != 0;
}
int check_band(){
    if (current_band[0] == 0){
        printf("current_band not selected\n");
    }
    return current_band[0] != 0;
}
int list_dir_contents(char* path){
    FILE_NAMES file_names;
    file_names_initializer(&file_names, path);
    char* dir_path = malloc(1024);
    transform_filepath_to_dirpath(dir_path, path);
    if (!get_all_files_from_dir(&file_names, dir_path)){
        return 0;
    }
    file_names_list(&file_names);
    file_names_free(&file_names);
    free(dir_path);
    return 1;
}
int get_all_files_from_dir(FILE_NAMES* file_names, char* dir_path){
    char* dir = dir_path;
    WIN32_FIND_DATAA data;
    HANDLE hFind = FindFirstFileA(dir, &data);
    char* name;
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            
            name = data.cFileName;
            if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0){
                continue;
            }
            if (!add_file(file_names, name, get_file_type(&data))){
                break;
            }
        } while (FindNextFileA(hFind, &data));
        FindClose(hFind);
    }
    else{
        return 0;
    }
    return 1;
}
void file_names_list(FILE_NAMES* file_names){
    printf("DIRECTORY: %s\n", file_names->dir_name);
    for (int i = 0; i < file_names->nr_files; i++){
        if (file_names->file_types[i]){
            printf("DIR: %s\n", file_names->files[i]);
        }
        else{
            printf("VID: %s\n", file_names->files[i]);
        }
    }
}
void file_names_free(FILE_NAMES* file_names){
    for (int i = 0; i < file_names->nr_files; i++){
        free(file_names->files[i]);
    }
}
int show_all_bands(){
    return list_dir_contents(current_output_path);
}
int show_albums_of_band(){
    char name[MAX_STRING_LENGHT];
    scanf("%s", name);
    char path[MAX_STRING_LENGHT];
    sprintf(path, "%s/%s", current_output_path, name);
    return list_dir_contents(path);
}
int show_albums_of_current_band(){
    check_band();
    char path[MAX_STRING_LENGHT];
    sprintf(path, "%s/%s", current_output_path, current_band);
    return list_dir_contents(path);
}
int show_songs_of_album(char* album_name){
    char path[MAX_STRING_LENGHT];
    sprintf(path, "%s/%s/%s", current_output_path, current_band, album_name);
    return list_dir_contents(path);
}
int download_album(char* band_name, char* album_name, char* url){
    char* dir_path = malloc(1024);
    sprintf(dir_path, "%s/%s", current_output_path, band_name);
    mkdir(dir_path);
    free(dir_path);
    char* command_cmd = malloc(1024);
    char* command_cd = malloc(1024);
    char* command_ytdl = malloc(1024);
    char* complete_path = malloc(1024);
    memset(command_cmd, 0, 1024);
    memset(command_cd, 0, 1024);
    memset(command_ytdl, 0, 1024);
    memset(complete_path, 0, 1024);
    sprintf(complete_path, "%s/%s", current_output_path, band_name);
    sprintf(command_cd, "cd /d \"%s\"", complete_path);
    sprintf(
        command_ytdl,
        "%s --yes-playlist -t mp3 -o \"%s%s\" \"%s\"",
        current_exec_path,
        album_name,
        "/%%(playlist_index)s - %%(title)s.%%(ext)s",
        url
    );
    sprintf(command_cmd, "%s && %s", command_cd, command_ytdl);
    //printf(command_cmd);

    int fd = open("D:/ytdl.bat", O_CREAT | O_RDWR, 0777);
    int len = strlen(command_cmd);
    ftruncate(fd, len);
    int nr_w = write(fd, command_cmd, len);
    char* command = "cmd.exe /C \"start D:/ytdl.bat\"";

    new_process(command);
    sleep(1);
    free(command);
    free(command_cd);
    free(command_cmd);
    free(command_ytdl);
}

void dl_info_init(DL_INFO* dl_info) {
    dl_info->flags = 0;
    dl_info->search_string = NULL;
    dl_info->path_strings = NULL;
    dl_info->nr_path_strings = 0;
    dl_info->overwrite_output_format = NULL;
    dl_info->overwrite_options = NULL;
    dl_info->overwrite_title = NULL;
    dl_info->start_with_string = NULL;
}
int download_options_init(DL_INFO* dl_info, char download_options[]) {
    if (dl_info->overwrite_options == NULL) {
        strcpy(download_options, DEFAULT_DL_OPTIONS);
    }
    else {
        strcpy(download_options, dl_info->overwrite_options);
    }
    return 0;
}
enum DL_INFO_ERROR {
    NO_SEARCH_STRING = 1,

};
int search_command_init(DL_INFO* dl_info, char search_command[]) {
    if (dl_info->search_string == NULL) {
        return NO_SEARCH_STRING;
    }
    if (dl_info->flags & SEARCH) {
        sprintf(dl_info->search_string, "ytsearch:%s", search_command);
    }
    else {
        sprintf(search_command, "%s", dl_info->search_string);
    }
    return 0;
}
int output_format_init(DL_INFO* dl_info, char output_format[]) {
    if (dl_info->overwrite_output_format != NULL) {
        sprintf(output_format, "%s", dl_info->overwrite_output_format);
        return 0;
    }
    int poz_output_format = helper_concat_strings_to_path(output_format, dl_info->path_strings, dl_info->nr_path_strings);

    char* title = "%(title)s";
    if (dl_info->overwrite_title != NULL) {
        title = dl_info->overwrite_title;
    }
    char* order_string = "";
    if (dl_info->flags & ORDER) {
        order_string = "%(playlist_index)s - ";
    }
    sprintf(output_format + poz_output_format, "%s%s.%%(ext)s", order_string, title);
}
int download_from_dl_info(DL_INFO* dl_info, char download_path[]) {
    int errors = 0;
    char download_options[MAX_STRING_LENGHT];
    char search_string[MAX_STRING_LENGHT];
    char output_format[MAX_STRING_LENGHT];

    errors |= download_options_init(dl_info, download_options);
    errors |= search_command_init(dl_info, search_string);
    errors |= output_format_init(dl_info, output_format);

    printf("download_options = %s, output_format = %s, search_string = %s\n", download_options, output_format, search_string);

    download(download_options, output_format, search_string, download_path);

    return errors;
}
void download(char download_options[], char output_format[], char search_command[], char main_path[]) {
    char download_command[MAX_STRING_LENGHT];

    sprintf(download_command, "cmd /C start cmd /C yt-dlp %s -o \"%s/%s\" \"%s\"",
        download_options, main_path, output_format, search_command);

    printf("%s\n", download_command);
    new_process(download_command);
    sleep(1);
}