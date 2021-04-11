#ifndef PROJECTFUNCTIONS_H
#define PROJECTFUNCTIONS_H

int ifnumeric(char *argument);
int addSlash(char *entry_path, int path_len);
static int one (const struct dirent *unused);
void copyingFunction(char *entry_path_source, char *entry_path_destination);
void deletingFunction(char *entry_path_destination);
void browsingTheDirectories(char *source, char *destination, int recursion, int size);

#endif
