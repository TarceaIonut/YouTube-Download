//
// Created by Ionut on 8/20/2025.
//

#ifndef YT_DOWNLOAD_H
#define YT_DOWNLOAD_H

#define DEFAULT_DL_OPTIONS "--yes-playlist -x --audio-format mp3 --audio-quality 0 --embed-thumbnail --embed-metadata"

enum DL_FLAGS {
    SEARCH = 1,
    ORDER = 2,
};

typedef struct DL_INFO {
    int flags;
    char* search_string;
    char** path_strings;
    int nr_path_strings;
    char* overwrite_output_format;
    char* overwrite_options;
    char* overwrite_title;
    char* start_with_string;

}DL_INFO;

int download_from_dl_info(DL_INFO* dl_info, char download_path[]);
void dl_info_init(DL_INFO* dl_info);
int download_album(char* band_name, char* album_name, char* url);
void download(char download_options[], char output_format[], char search_command[], char main_path[]);

#endif //YT_DOWNLOAD_H
