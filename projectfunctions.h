#ifndef PROJECTFUNCTIONS_H
#define PROJECTFUNCTIONS_H
#include "projectfunctions.h"


int addSlash(char *entry_path, int path_len);
void bigFileCopyingFunction(char *entry_path_source, char *entry_path_destination);
void smallFilecopyingFunction(char *entry_path_source, char *entry_path_destination);
void copyingFunction(char *entry_path_source, char *entry_path_destination, int size);
void deletingFunction(char *entry_path_destination);
void browsingTheDirectories(char *source, char *destination, int recursion, int size);
void man_projekt_program();

#endif
