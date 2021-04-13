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
#include <sys/stat.h>
#include <fcntl.h>

// int ifnumeric(char *argument){
//   char tmp[strlen(argument)];
//   strcpy(tmp, argument);
//   for(int i= 0; i<strlen(tmp); i++){
//     char tmp1 = tmp[i];
//     if(!isdigit(tmp1)) return 0;
//   }
//   return 1;
// }

int addSlash(char *entry_path, int path_len){
  if (entry_path[path_len - 1] != '/') {
      entry_path[path_len] = '/';
      entry_path[path_len + 1] = '\0';
      return 1;
  }
  return 0;
}

static int one1(const struct dirent *unused)
{
  return 1;
}


  void copyingFunction(char *entry_path_source, char *entry_path_destination){
      fprintf(stdout, "Kopiowanie pliku:%s do %s\n",entry_path_source,entry_path_destination);
      int src_fd, dst_fd, n, err;
      unsigned char buffer[1024];
      char * src_path, dst_path;

      src_fd = open(entry_path_source, O_RDONLY, S_IRWXU | S_IRWXG | S_IROTH);
      dst_fd = open(entry_path_destination, O_CREAT , S_IRWXU | S_IRWXG | S_IROTH);

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

  void deletingFunction(char *entry_path){
    fprintf(stdout, "Usuwanie pliku: %s\n", entry_path);
    if ( remove (entry_path) == -1) {
      int e = errno;
      switch (e) {
        case ENOTEMPTY:{
          struct dirent **rem1;
          int n_removing;
          char remove_path[PATH_MAX + 1];
          size_t path_len;

          strncpy (remove_path, entry_path, sizeof (remove_path));
          path_len = strlen (remove_path);
          if(addSlash(remove_path,path_len))
              ++path_len;

          n_removing = scandir (remove_path, &rem1, one1, alphasort);
          if (n_removing >= 0)
          {
            int i=2;
            while(i < n_removing){
              strncpy (remove_path + path_len, rem1[i]->d_name,
                      sizeof (remove_path) - path_len);
              deletingFunction(remove_path);
              ++i;
            }
          }
          else{
            perror ("Couldn't open the directory");
          }

          if ( remove (entry_path) == -1)
              perror("97 linijka: ");
        }
        break;
        default:
          perror("Removing error:");
          exit(EXIT_FAILURE);
        break;
      }
    }
  }

  void browsingTheDirectories(char *source, char *destination, int recursion, int size){
      struct dirent **eps1 = NULL, **eps2= NULL;
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


        n_source = scandir (source, &eps1, one1, alphasort);
        n_destination = scandir (destination, &eps2, one1, alphasort);
        if (n_source >= 0)
        {
          int i = 2, j = 2, comparing;

            while(i < n_source){
              comparing = strcmp(eps1[i]->d_name, eps2[j]->d_name);
              strncpy (entry_path_source + path_len_source, eps1[i]->d_name,
                      sizeof (entry_path_source) - path_len_source);
              strncpy (entry_path_destination + path_len_destination, eps2[j]->d_name,
                      sizeof (entry_path_destination) - path_len_destination);

              if(eps1[i]->d_type == DT_DIR)
              {
                int status;

                if(recursion)
                {
                  if(comparing > 0){
                    deletingFunction(entry_path_destination);
                    j++;
                  }
                  else if(comparing < 0){
                    strncpy (entry_path_destination + path_len_destination, eps1[i]->d_name,
                            sizeof (entry_path_destination) - path_len_destination);
                    status = mkdir(entry_path_destination, S_IRWXU | S_IRWXG | S_IROTH);
                    fprintf(stdout,"Tworzenie katalogu, początek funkcji rekurencyjnej");
                    if(status == -1){
                      int e = errno;
                      switch (e) {
                        case EEXIST:
                          browsingTheDirectories(entry_path_source, entry_path_destination, recursion, size);
                          i++;
                          break;
                        default:
                          perror("Error with mkdir:");
                          exit(EXIT_FAILURE);
                          break;
                      }
                    }
                    fprintf(stdout, "Kopiowanie folderu %s: \n", eps1[i]->d_name);
                  }
                  else{
                    fprintf(stdout, "Rekurencja\n");
                    browsingTheDirectories(entry_path_source, entry_path_destination, recursion, size);
                    i++;
                    j++;
                  }
                }

              }
              else if(eps1[i]->d_type == DT_REG)
              {

                if ( comparing > 0){
                  deletingFunction(entry_path_destination);
                  fprintf(stdout, "Usuwanie pliku\n");
                  j++;
                }
                else if(comparing < 0) {
                  strncpy (entry_path_destination + path_len_destination, eps1[i]->d_name,
                          sizeof (entry_path_destination) - path_len_destination);
                  copyingFunction(entry_path_source, entry_path_destination);
                  // funkcja kopiujaca plik z jednego folderu do drugiego
                  // F1: ab -> F2 :aa
                  //     ac -> F2 :ac
                  //              :ad

                  i++;
                }
                else{
                  //Sprawdzenie daty modyfikacji danego pliku
                  struct stat f1, f2;
                  double modificationTime;

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
                      deletingFunction(entry_path_destination);
                      strncpy (entry_path_destination + path_len_destination, eps1[i]->d_name,
                              sizeof (entry_path_destination) - path_len_destination);
                      copyingFunction(entry_path_source, entry_path_destination);
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
              strncpy (entry_path_destination + path_len_destination, eps2[j]->d_name,
                      sizeof (entry_path_destination) - path_len_destination);
              deletingFunction(entry_path_destination);
              ++j;
            }
        }
      else
       perror ("Couldn't open the directory");
  }


  void sleep_or_signal(int programTime, int flag)
  {
    int i=0;
    while(i<=programTime || flag != 1){
      sleep(1);
      i++;
    }
  }
