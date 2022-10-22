#ifndef _STRSORT_H
#define _STRSORT_H


typedef int comp_t (const void *param1, const void *param2);


struct line {
    
    char *start = nullptr;
    size_t len = 0;
};


struct text {

    char *source = nullptr;
    line *content = nullptr;

    size_t lines_count = 0;
};


/// param [in]  file_name file name to open
/// param [out] file      pointer on struct for writing a file
///
/// retval 0  if there is no errors
/// retval -1 if there is an error
int GetText         (const char *file_name, text *file);

/// param [in] file_name file name to open
///
/// retval [int] size of file in bytes
/// retval -1    if there is an error
int GetFileSize     (const char *file_name);

/// param [out] file      pointer on struct for writing a file
/// param [in]  file_name file name to open
///
/// retval [int] count of lines ii source string
/// retval -1    if there is an error
int ReadFile        (text *file, size_t file_size, const char *file_name);

int UpdateFile (text *file, size_t char_count);

/// param [out] file pointer on updating text
///
/// retval 0  if there is no errors
/// retval -1 if there is an error
int SepLines        (text *file);

/// param [in] pr1  pointer on first line
/// param [in] pr2  pointer on second line
/// param [in] len1 length of first line
/// param [in] len1 length of second line
/// param [in] mode of sort
int GlobalCmp       (const char *pr1, const char *pr2, size_t len1, size_t len2, int mode);

/// param [in] param1  pointer on first line
/// param [in] param2  pointer on second line
int LineCmp         (const void *param1, const void *param2);

/// param [in] param1  pointer on first line
/// param [in] param2  pointer on second line
int LineReverceCmp  (const void *param1, const void *param2);

/// param [in] file     pointer on struct to write
/// param [in] out_file name of ouput file
///
/// retval 0  if there is no errors
/// retval -1 if there is an error
int WriteTextinFile (text *file, const char *out_file);

/// param [in, out] file pointer on struct to free
///
/// retval 0  if there is no errors
/// retval -1 if there is an error
int FreeText        (text *file);


#endif // _STRSORT
