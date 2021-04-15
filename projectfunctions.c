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
#include <syslog.h>
#include <sys/sendfile.h>

int addSlash(char *entry_path, int path_len)
{
  if (entry_path[path_len - 1] != '/')
  {
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

void bigFileCopyingFunction(char *entry_path_source, char *entry_path_destination)
{
  syslog(LOG_NOTICE, "Kopiowanie dużego pliku:%s do %s poprzez funkcję sendfile", entry_path_source, entry_path_destination);
  int source_fd;
  int destination_fd;
  struct stat stat_buf;
  off_t offset = 0;

  source_fd = open(entry_path_source, O_RDONLY, S_IRWXU | S_IRWXG | S_IROTH);
  if (source_fd == -1)
  {
    perror("Source_fd error:");
    exit(EXIT_FAILURE);
  }
  fstat(source_fd, &stat_buf);
  destination_fd = open(entry_path_destination, O_WRONLY | O_CREAT, stat_buf.st_mode);
  if (destination_fd == -1)
  {
    perror("Destination_fd error:");
    exit(EXIT_FAILURE);
  }
  if (sendfile(destination_fd, source_fd, &offset, stat_buf.st_size) == -1)
  {
    perror("Sendfile error:");
    exit(EXIT_FAILURE);
  }

  close(source_fd);
  close(destination_fd);
}

void smallFilecopyingFunction(char *entry_path_source, char *entry_path_destination)
{
  syslog(LOG_NOTICE, "Kopiowanie małego pliku:%s do %s poprzez read/write", entry_path_source, entry_path_destination);
  int src_fd, dst_fd, n;
  unsigned char buffer[4096];

  src_fd = open(entry_path_source, O_RDONLY);
  if (src_fd == -1)
  {
    perror("CANNOT OPEN THE SOURCE FILE");
    exit(EXIT_FAILURE);
  }

  dst_fd = open(entry_path_destination, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG | S_IROTH);
  if (dst_fd == -1)
  {
    perror("CANNOT OPEN THE TARGET FILE");
    exit(EXIT_FAILURE);
  }

  while ((n = read(src_fd, buffer, sizeof(buffer))) > 0)
    write(dst_fd, buffer, n);

  close(src_fd);
  close(dst_fd);
}

void copyingFunction(char *entry_path_source, char *entry_path_destination, int size)
{
  struct stat f_copy;
  if (stat((entry_path_source), &f_copy) == -1)
  {
    perror("Stat file from copying function error:");
    exit(EXIT_FAILURE);
  }
  if ((f_copy.st_size) >= size)
    bigFileCopyingFunction(entry_path_source, entry_path_destination);
  else
    smallFilecopyingFunction(entry_path_source, entry_path_destination);
}

void deletingFunction(char *entry_path)
{
  syslog(LOG_NOTICE, "Usuwanie pliku: %s\n", entry_path);
  if (remove(entry_path) == -1)
  {
    int e = errno;
    switch (e)
    {
    case ENOTEMPTY:
    {
      syslog(LOG_NOTICE, "Usuwany folder: %s nie jest pusty, usuwanie plików znajdujących się w folderze.", entry_path);
      struct dirent **rem1;
      int n_removing;
      char remove_path[PATH_MAX + 1];
      size_t path_len;

      strncpy(remove_path, entry_path, sizeof(remove_path));
      path_len = strlen(remove_path);
      if (addSlash(remove_path, path_len))
        ++path_len;

      n_removing = scandir(remove_path, &rem1, one1, alphasort);
      if (n_removing >= 0)
      {
        int i = 2;
        while (i < n_removing)
        {
          strncpy(remove_path + path_len, rem1[i]->d_name,
                  sizeof(remove_path) - path_len);
          deletingFunction(remove_path);
          ++i;
        }
      }
      else
      {
        perror("Couldn't open the directory");
      }

      if (remove(entry_path) == -1)
      {
        perror("Removing error:");
        exit(EXIT_FAILURE);
      }
    }
    break;
    default:
      perror("Removing error:");
      exit(EXIT_FAILURE);
      break;
    }
  }
}

void browsingTheDirectories(char *source, char *destination, int recursion, int size)
{
  char *source1 = source;
  char *destination1 = destination;
  struct dirent **eps1 = NULL, **eps2 = NULL;
  int n_source, n_destination;
  char entry_path_source[PATH_MAX + 1];
  char entry_path_destination[PATH_MAX + 1];
  size_t path_len_destination;
  size_t path_len_source;

  strncpy(entry_path_source, source1, sizeof(entry_path_source));
  strncpy(entry_path_destination, destination1, sizeof(entry_path_destination));
  path_len_source = strlen(source);
  path_len_destination = strlen(destination);
  /* If the directory path doesn't end with a slash, append a slash. */
  if (addSlash(entry_path_source, path_len_source))
    ++path_len_source;
  if (addSlash(entry_path_destination, path_len_destination))
    ++path_len_destination;

  n_source = scandir(source, &eps1, one1, alphasort);
  if (n_source == -1)
  {
    perror("N_source problem:");
    exit(EXIT_FAILURE);
  }

  n_destination = scandir(destination, &eps2, one1, alphasort);
  if (n_destination == -1)
  {
    perror("N_destination problem:");
    exit(EXIT_FAILURE);
  }

  int i = 2, j = 2, comparing, status;

  if (n_source >= 0)
  {

    syslog(LOG_NOTICE, "Przejrzenie obydwu katalogów: %s oraz %s", entry_path_source, entry_path_destination);
    while (i < n_source && j < n_destination)
    {
      comparing = strcmp(eps1[i]->d_name, eps2[j]->d_name);
      strncpy(entry_path_source + path_len_source, eps1[i]->d_name,
              sizeof(entry_path_source) - path_len_source);
      strncpy(entry_path_destination + path_len_destination, eps2[j]->d_name,
              sizeof(entry_path_destination) - path_len_destination);

      if (eps1[i]->d_type == DT_DIR)
      {

        if (recursion)
        {
          if (comparing < 0)
          {
            strncpy(entry_path_destination + path_len_destination, eps1[i]->d_name,
                    sizeof(entry_path_destination) - path_len_destination);
            status = mkdir(entry_path_destination, 0700);
            if (status == -1)
            {
              int e = errno;
              switch (e)
              {
              case EEXIST:
                syslog(LOG_NOTICE, "21Wejście do katalogów: %s, %s, poprzez funkcję rekurencyjną.", entry_path_source, entry_path_destination);
                browsingTheDirectories(entry_path_source, entry_path_destination, recursion, size);
                i++;
                break;
              default:
                perror("Error with mkdir:");
                exit(EXIT_FAILURE);
                break;
              }
            }
            else
              browsingTheDirectories(entry_path_source, entry_path_destination, recursion, size);
            i++;
          }
          else if (comparing > 0)
          {
            deletingFunction(entry_path_destination);
            j++;
          }
          else
          {
            browsingTheDirectories(entry_path_source, entry_path_destination, recursion, size);
            i++;
            j++;
          }
        }

        else
        {
          i++;
        }
      }
      else if (eps1[i]->d_type == DT_REG)
      {

        if (comparing > 0)
        {
          deletingFunction(entry_path_destination);
          j++;
        }
        else if (comparing < 0)
        {
          strncpy(entry_path_destination + path_len_destination, eps1[i]->d_name,
                  sizeof(entry_path_destination) - path_len_destination);
          copyingFunction(entry_path_source, entry_path_destination, size);
          // funkcja kopiujaca plik z jednego folderu do drugiego
          // F1: ab -> F2 :aa
          //     ac -> F2 :ac
          //              :ad

          i++;
        }
        else
        {
          //Sprawdzenie daty modyfikacji danego pliku
          struct stat f1, f2;
          double modificationTime;

          if (stat((entry_path_source), &f1) == -1)
          {
            perror("1Stat file from source directory:");
            exit(EXIT_FAILURE);
          }
          if (stat((entry_path_destination), &f2) == -1)
          {
            perror("2Stat file from destination directory:");
            exit(EXIT_FAILURE);
          }
          modificationTime = difftime(f1.st_mtime, f2.st_mtime);
          if (modificationTime > 0)
          {
            syslog(LOG_NOTICE, "Znaleziono pliki o tej samej nazwie, lecz o innym czasie modyfikacji!. Pliki to %s, %s", entry_path_source, entry_path_destination);
            deletingFunction(entry_path_destination);
            strncpy(entry_path_destination + path_len_destination, eps1[i]->d_name,
                    sizeof(entry_path_destination) - path_len_destination);
            copyingFunction(entry_path_source, entry_path_destination, size);
          }
          else
          {
            syslog(LOG_NOTICE, "Znaleziono pliki o tej samej nazwie i tym samym czasie modyfikacji!. Pliki to %s, %s", entry_path_source, entry_path_destination);
          }
          ++i;
          ++j;
        }
      }
    }
    while (i < n_source)
    {
      strncpy(entry_path_source + path_len_source, eps1[i]->d_name,
              sizeof(entry_path_source) - path_len_source);
      strncpy(entry_path_destination + path_len_destination, eps1[i]->d_name,
              sizeof(entry_path_destination) - path_len_destination);
      if (eps1[i]->d_type == DT_DIR)
      {
        strncpy(entry_path_destination + path_len_destination, eps1[i]->d_name,
                sizeof(entry_path_destination) - path_len_destination);
        status = mkdir(entry_path_destination, 0700);
        if (status == -1)
        {
          int e = errno;
          switch (e)
          {
          case EEXIST:
            syslog(LOG_NOTICE, "21Wejście do katalogów: %s, %s, poprzez funkcję rekurencyjną.", entry_path_source, entry_path_destination);
            browsingTheDirectories(entry_path_source, entry_path_destination, recursion, size);
            i++;
            break;
          default:
            perror("Error with mkdir:");
            exit(EXIT_FAILURE);
            break;
          }
        }
        else
        {
          browsingTheDirectories(entry_path_source, entry_path_destination, recursion, size);
          i++;
        }
      }
      else
        copyingFunction(entry_path_source, entry_path_destination, size);
      ++i;
    }
  }
  else
    perror("Couldn't open the directory");
}

void man_projekt_program()
{
  syslog(LOG_NOTICE, "Wywołanie pomocy");
  fprintf(stdout, "\nDemon synchronizyjący dwa podkatalogi. przyjmowane argumenty:\n-s [argument] - podanie demonowi ścieżki źródłowej,\n-d [argument] - podanie demonowi ścieżki docelowej,\n-R - opcjonalny argument do dzialania na podkatalogach,\n-T [argument] - opcjonalny argument zmieniający czas, co jaki czas demon ma się budzić,\n-S [argument] - opcjonalny argument ustawiający rozmiar, który ma odróżniać pliki duże od małych,\n-h - wyświetlenie pomocy,\n\n");
}
