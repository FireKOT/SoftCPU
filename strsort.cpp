#include <stdio.h>
#include <malloc.h>
#include <string.h> 
#include <ctype.h>
#include <sys/stat.h>
#include <windows.h>
#include "config.h"
#include "general.h"
#include "strsort.h"


static const unsigned int ADD_SIZE = 10;
static const unsigned int FREE = 13;


int GetText (const char *file_name, text *file) {

    RET_ON_VAL(!file_name || !file, ERR_NULL_PTR, -1);

    size_t file_size = GetFileSize(file_name);

    file->source = (char*) calloc(file_size + ADD_SIZE, sizeof(char));
    RET_ON_VAL(!file->source, ERR_NULL_PTR, -1);

    size_t source_lines_count = ReadFile(file, file_size, file_name);

    file->content = (line*) calloc(source_lines_count + ADD_SIZE, sizeof(line));
    RET_ON_VAL(!file->content, ERR_NULL_PTR, -1);

    SepLines(file);

    return 0;
}

int GetFileSize (const char *file_name) {

    struct stat file_stats;
    int status = stat(file_name, &file_stats);
    RET_ON_VAL(status == -1, ERR_ACC_DENi, -1);

    return file_stats.st_size;
}

int ReadFile (text *file, size_t file_size, const char *file_name) {

    RET_ON_VAL(!file, ERR_NULL_PTR, -1);

    FILE *input = fopen(file_name, "r");
    RET_ON_VAL(!input, ERR_ACC_DENi, -1);

    size_t char_count = fread(file->source, sizeof(char), file_size, input);
    size_t source_lines_count = UpdateFile(file, char_count);

    return source_lines_count;
}

int UpdateFile (text *file, size_t char_count) {

    RET_ON_VAL(!file, ERR_NULL_PTR, -1);

    size_t i = 0, lines_count = 0;
    for (i = 0; i < char_count; i++) {
        if (file->source[i] == '\n') {
            file->source[i] = '\0';
            lines_count++;
        }
    }
    file->source[i++] = '\0';
    file->source[i] = EOF;

    return lines_count;
}

int SepLines (text *file) {

    RET_ON_VAL(!file, ERR_NULL_PTR, -1);

    int i = 0, line = 0;
    while (file->source[i] != EOF) {
        if (file->source[i] != '\0') {
            file->content[line].start = &file->source[i];
            i++;

            file->content[line].len = 1;
            while (file->source[i++] != '\0') {
                file->content[line].len++;
            }

            line++;
        }
        else {
            i++;
        }
    }
    file->lines_count = line;

    return 0;
}

int GlobalCmp (const char *pr1, const char *pr2, size_t len1, size_t len2, int mode) {

    size_t i = 0, j = 0;
    while (!isalnum(*(pr1 + i*mode)) && i < len1) {
        i++;
    }
    while (!isalnum(*(pr2 + j*mode)) && j < len2) {
        j++;
    }
    
    while (tolower(*(pr1 + i*mode)) == tolower(*(pr2 + j*mode)) && i < len1 && j < len2) {
        i++;
        j++;
    }

    if (i == len1 && j == len2) {
        return 0;
    }
    else if (i == len1) {
        return -1;
    }
    else if(j == len2) {
        return 1;
    }
    
    return tolower(*(pr1 + i*mode)) - tolower(*(pr2 + j*mode));
}

int LineCmp (const void *param1, const void *param2) {

    char *pr1_start = ((const line*) param1)->start;
    char *pr2_start = ((const line*) param2)->start;
    size_t pr1_len =  ((const line*) param1)->len;
    size_t pr2_len =  ((const line*) param2)->len;

    return GlobalCmp(pr1_start, pr2_start, pr1_len, pr2_len, 1);
}

int LineReverceCmp (const void *param1, const void *param2) {

    char *pr1_end = ((const line*) param1)->start + ((const line*) param1)->len - 1;
    char *pr2_end = ((const line*) param2)->start + ((const line*) param2)->len - 1;
    size_t pr1_len =  ((const line*) param1)->len;
    size_t pr2_len =  ((const line*) param2)->len;

    return GlobalCmp(pr1_end, pr2_end, pr1_len, pr2_len, -1);
}

int WriteTextinFile (text *file, const char *out_file) {

    RET_ON_VAL(!file || !file, ERR_NULL_PTR, -1);

    FILE *output = fopen(out_file, "w");
    RET_ON_VAL(!output, ERR_ACC_DENi, -1);

    for (size_t i = 0; i < file->lines_count; i++) {
        char *str = file->content[i].start;
        while (!isalnum(*str) && *str != '\0') {
            str++;
        }

        if (*str != '\0') {
            fprintf(output, "%s\n", str);
        }
    }
    
    fclose(output);
    return 0;
}

int FreeText (text *file) {
    RET_ON_VAL(!file, ERR_NULL_PTR, -1);
    RET_ON_VAL(!file->content || !file->source, ERR_NULL_PTR, -1);

    ISWARN(file->content == (line*) FREE || file->source == (char*) FREE, WARN_REFREE_PTR);
    
    free(file->content);
    UnmapViewOfFile(file->source);

    file->content = (line*) FREE;
    file->source  = (char*) FREE;

    return 0;
}