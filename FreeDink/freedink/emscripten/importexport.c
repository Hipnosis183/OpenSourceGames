/**
 * Import/export ~/.dink/ as .zip

 * Copyright (C) 2018  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zip.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>

extern int mkdir_rec(char* path);

int IsDir(char* path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

int AddFileToZip(zip_t* archive, char* path, char* prefix) {
  zip_source_t *s;
  s = zip_source_file(archive, path, 0, -1);
  if (s == NULL) {
    fprintf(stderr, "error opening %s\n", path);
    return 0;
  }
  char* zippath = path + strlen(prefix);
  if (zip_file_add(archive, zippath, s, ZIP_FL_ENC_UTF_8) < 0) { 
    zip_source_free(s);
    fprintf(stderr, "error adding %s: %s\n", path, zip_strerror(archive));
    return 0;
  }
  return 1;
}

int AddPathToZip(zip_t* archive, char* path, char* prefix) {
  //printf("processing %s\n", path);
  struct stat st;
  if (IsDir(path)) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
      perror("opendir");
      return 0;
    }
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
      //printf("dir: %s\n", ent->d_name);
      if (strcmp(ent->d_name, ".") == 0) continue;
      if (strcmp(ent->d_name, "..") == 0) continue;
      char* subpath = malloc(strlen(path) + 1 + strlen(ent->d_name) + 1);
      sprintf(subpath, "%s/%s", path, ent->d_name);
      AddPathToZip(archive, subpath, prefix);
      free(subpath);
    }
    closedir(dir);
  } else {
    AddFileToZip(archive, path, prefix);
  }
  return 1;
}

int emSavegamesExport(void) {
  int errorp;
  zip_t* archive = zip_open("savegames.zip", ZIP_CREATE|ZIP_TRUNCATE|ZIP_CHECKCONS, &errorp);
  if (archive == NULL) {
    zip_error_t error;
    zip_error_init_with_code(&error, errorp);
    fprintf(stderr, "error creating savegames.zip: %s\n", zip_error_strerror(&error));
    zip_error_fini(&error);
    return 0;
  }

  char* path = malloc(strlen(getenv("HOME"))+1+strlen(".dink")+1);
  sprintf(path, "%s/%s", getenv("HOME"), ".dink");
  char* prefix = malloc(strlen(getenv("HOME"))+1+strlen(".dink")+1+1);
  sprintf(prefix, "%s/%s/", getenv("HOME"), ".dink");
  AddPathToZip(archive, path, prefix);
  free(path);

  if (zip_get_num_entries(archive, 0) == 0) {
    printf("No savegames found!\n");
    return 0;
  } else if (zip_close(archive) < 0) {
    fprintf(stderr, "cannot write savegames.zip: %s\n", zip_strerror(archive));
    return 0;
  }

  chmod("savegames.zip", 00666);
  return 1;
}

void emSavegamesImport(void) {
  int errorp;
  zip_t* archive = zip_open("/savegames.zip", ZIP_RDONLY, &errorp);
  if (archive == NULL) {
    zip_error_t error;
    zip_error_init_with_code(&error, errorp);
    fprintf(stderr, "error opening savegames.zip: %s\n", zip_error_strerror(&error));
    zip_error_fini(&error);
    return;
  }

  char* prefix = malloc(strlen(getenv("HOME"))+1+strlen(".dink")+1+1);
  sprintf(prefix, "%s/%s/", getenv("HOME"), ".dink");
  for (int i = 0; i < zip_get_num_entries(archive, 0); i++) {
    zip_stat_t sb;
    zip_stat_index(archive, i, 0, &sb);
    if (!(sb.valid & ZIP_STAT_NAME)
	|| !(sb.valid & ZIP_STAT_SIZE)) {
      continue;
    }

    if (sb.name[strlen(sb.name)-1] == '/') {
      // will recursively create directories later
      continue;
    }

    char* destFullPath = malloc(strlen(prefix)+strlen(sb.name)+1);
    sprintf(destFullPath, "%s%s", prefix, sb.name);
    char* dir = strdup(destFullPath);
    dirname(dir);
    int ret = mkdir_rec(dir);
    if (ret < 0) printf("Error creating directory %s\n", dir);
    free(dir);
    if (ret < 0)
      continue;

    zip_file_t* file;
    if ((file = zip_fopen_index(archive, i, 0)) == NULL) {
      fprintf(stderr, "error opening file %d: %s\n", i, zip_strerror(archive));
      continue;
    }
    FILE* out;
    if ((out = fopen(destFullPath, "wb")) == NULL) {
      perror("fopen");
      continue;
    }
    
    unsigned char buf[4096];  // size from emscripten implementation
    zip_uint64_t total = 0;
    while (total < sb.size) {
      zip_int64_t nb_read;
      if ((nb_read = zip_fread(file, buf, sizeof(buf))) < 0) {
	fprintf(stderr, "error reading %s: %s\n", sb.name, zip_file_strerror(file));
	continue;
      }
      fwrite(buf, nb_read, 1, out);
      total += nb_read;
    }
    fclose(out);
    zip_fclose(file);
    free(destFullPath);
  }
}

#ifdef TEST
int main(void) {
  return !emSavegamesExport();
}
#endif
