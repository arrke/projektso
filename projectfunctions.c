#include "projectfunctions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

int ifnumeric(char *argument){
  char tmp[strlen(argument)];
  strcpy(tmp, argument);
  for(int i= 0; i<strlen(tmp); i++){
    char tmp1 = tmp[i];
    if(!isdigit(tmp1)) return 0;
  }
  return 1;
}


int addSlash(char *entry_path, int path_len){
  if (entry_path[path_len - 1] != '/') {
      entry_path[path_len] = '/';
      entry_path[path_len + 1] = '\0';
      return 1;
  }
  return 0;
}

static int one (const struct dirent *unused)
{
  return 1;
}

void browsingTheDirectories(char *source, char *destination, int recursion, int size){
  struct dirent **eps1, **eps2;
  int n_source, n_destination;
  char entry_path_source[PATH_MAX + 1];
  char entry_path_destination[PATH_MAX + 1];
  size_t path_len_destination;
  size_t path_len_source;


    strncpy (entry_path_source, source, sizeof (entry_path_source));
    strncpy (entry_path_destination, destination, sizeof (entry_path_destination));
    path_len_source = strlen (source);
    path_len_destination= strlen (destination);
    /* If the directory path doesn't end with a slash, append a slash. */

    if(addSlash(entry_path_source,path_len_source)) ++path_len_source;
    if(addSlash(entry_path_destination,path_len_destination)) ++path_len_destination;


    n_source = scandir (source, &eps1, one, alphasort);
    n_destination = scandir (destination, &eps2, one, alphasort);
    if (n_source >= 0)
    {
      int i = 2, j = 2, comparing;

        while(i < n_source){
          if(eps1[i]->d_type == DT_DIR)
          {
            if(recursion)
            {
              //funkcja rekurencyjna na danym folderze, włącznie ze stworzeniem folderu jezeli go nie ma
            }
            i++;
          }
          else if(eps1[i]->d_type == DT_REG)
          {
            comparing = strcmp(eps1[i]->d_name, eps2[j]->d_name);
            if ( comparing > 0){
              deletingFunction(eps2[j]->d_name,entry_path_destination,path_len_destination);
              fprintf(stdout, "Usuwanie pliku\n");
              j++;
            }
            else if(comparing < 0) {
              copyingFunction(eps1[i]->d_name, entry_path_source, entry_path_destination,
                              path_len_source, path_len_destination);
              // funkcja kopiujaca plik z jednego folderu do drugiego
              // F1: ab -> F2 :aa
              //     ac -> F2 :ac
              //              :ad
              fprintf(stdout, "Kopiowanie pliku\n");
              i++;
            }
            else{
              //Sprawdzenie daty modyfikacji danego pliku
              struct stat f1, f2;
              double modificationTime;
              strncpy (entry_path_source + path_len_source, eps1[i]->d_name,
                        sizeof (entry_path_source) - path_len_source);

              strncpy (entry_path_destination + path_len_destination, eps2[j]->d_name,
                        sizeof (entry_path_destination) - path_len_destination);

              if (stat( (entry_path_source), &f1) == -1) {
                  perror("1Stat file from source directory:");
                  exit(EXIT_FAILURE);
              }
              if (stat( (entry_path_destination), &f2) == -1) {
                  perror("2Stat file from destination directory:");
                  exit(EXIT_FAILURE);
              }
              modificationTime = difftime(f1.st_mtime, f2.st_mtime);
              if (modificationTime > 0){
                  deletingFunction(eps2[j]->d_name,entry_path_destination,path_len_destination);
                  copyingFunction(eps1[i]->d_name, entry_path_source, entry_path_destination,
                                  path_len_source, path_len_destination);
                  fprintf(stdout, "Inny czas modyfikacji\n");

              }
              else{
                fprintf(stdout, "ten sam czas modyfikacji. nic nie rób\n");
              }
              ++i;
              ++j;
            }
          }
        }
        while(j < n_destination){
          // funkcja usuwająca plik z drugiego folderu
          ++j;
        }
    }
  else
    perror ("Couldn't open the directory");
}



void copyingFunction(char *file_source, char *entry_path_source, char * entry_path_destination,
                    int path_len_source, int path_len_destination){
    strncpy (entry_path_source + path_len_source, eps1[i]->d_name,
            sizeof (entry_path_source) - path_len_source);
    strncpy (entry_path_destination + path_len_destination, eps1[i]->d_name,
            sizeof (entry_path_destination) - path_len_destination);
    src_fd = open(entry_path_source, O_RDONLY);
    dst_fd = open(entry_path_destination, O_CREAT | O_WRONLY);

    while (1) {
        err = read(src_fd, buffer, 4096);
        if (err == -1) {
            printf("Error reading file.\n");
            exit(1);
        }
        n = err;

        if (n == 0) break;

        err = write(dst_fd, buffer, n);
        if (err == -1) {
            printf("Error writing to file.\n");
            exit(1);
        }
    }

    close(src_fd);
    close(dst_fd);
}

void deletingFunction(char *file_to_delete,char *entry_path_destination, int path_len_destination){
  strncpy (entry_path_destination + path_len_destination, file_to_delete,
            sizeof (entry_path_destination) - path_len_destination);
  if ( remove (entry_path_destination) == -1) {
      perror("Removing error:");
      exit(EXIT_FAILURE);
  }
}
