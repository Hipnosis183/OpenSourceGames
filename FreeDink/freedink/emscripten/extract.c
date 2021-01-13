/**
 * Efficient tar+bzip2 extractor for in-game use

 * Copyright (C) 2004  Andrew Reading
 * Copyright (C) 2007, 2008, 2014, 2018  Sylvain Beucler

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#define _(s) s
#endif

#include <bzlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <assert.h>

struct TarHeader {
  char Name[100+1];      // 100
  char Mode[8+1];        // 108
  char Uid[8+1];         // 116
  char Gid[8+1];         // 124
  char Size[12+1];       // 136
  char Mtime[12+1];      // 148
  char Chksum[8+1];      // 156
  char Linkflag[1+1];    // 157
  char Linkname[100+1];  // 257
  char Magic[8+1];       // 265
  char Uname[32+1];      // 297
  char Gname[32+1];      // 329
  char Devmajor[8+1];    // 337
  char Devminor[8+1];    // 345
  char Padding[167+1];   // 512
};


#define TAR_HEADER_SIZE	512

int mkdir_rec(char* path) {
  int ret;
  mode_t mode = 00755;
  ret = mkdir(path, mode);
  if (ret == 0 || errno == EEXIST) {
    return 0;
  } else {
    char* dir = strdup(path);
    dirname(dir);
    ret = mkdir_rec(dir);
    free(dir);
    if (ret < 0) {
      return ret;
    }
    if ((ret = mkdir(path, mode)) < 0) {
      perror("mkdir");
      return ret;
    } 
    return 0;
  }
}

int DirExists(char* path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

void print_bzerror(BZFILE* bzin, int bzerror) {
  printf("%s\n", BZ2_bzerror(bzin, &bzerror));
  if (bzerror == BZ_OK || bzerror == BZ_STREAM_END) {
    // Success
  } else if (bzerror == BZ_PARAM_ERROR) {
    printf(_("Invalid .dmod file specified."));
  } else if (bzerror == BZ_SEQUENCE_ERROR) {
    printf(_("Critical program function error: opened for write."));
  } else if (bzerror == BZ_IO_ERROR) {
    printf(_("Could not read .dmod file.") );
  } else if (bzerror == BZ_UNEXPECTED_EOF) {
    printf(_("Incomplete .dmod file. Please download it again.") );
  } else if (bzerror == BZ_DATA_ERROR) {
    printf(_("The .dmod file is corrupted.  Please download it again."));
  } else if (bzerror == BZ_DATA_ERROR_MAGIC) {
    printf(_("The file is not a valid .dmod file."));
  } else if (bzerror == BZ_MEM_ERROR) {
    printf(_("Out of memory error."));
  } else {
    printf(_("An unhandled error occurred."));
  }
  printf("\n");
}



/**
 * Read all tar headers from a flux, so it can work if reading file
 * with BZip2 directly (only the 'read' operation is available).
 */
int TarFillHeaderFromStream(BZFILE* bzin, struct TarHeader* tarHeader)
{
  memset(tarHeader, 0, sizeof(*tarHeader));
  
  // Read the data. Don't load the whole header to a struct, you
  // never know what the memory alignment will be, especially in
  // these 32->64bits days
  int bzerror;
  BZ2_bzRead(&bzerror, bzin, tarHeader->Name, 100);     if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
  BZ2_bzRead(&bzerror, bzin, tarHeader->Mode, 8);       if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
  BZ2_bzRead(&bzerror, bzin, tarHeader->Uid, 8);        if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
  BZ2_bzRead(&bzerror, bzin, tarHeader->Gid, 8);        if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
  BZ2_bzRead(&bzerror, bzin, tarHeader->Size, 12);      if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
  BZ2_bzRead(&bzerror, bzin, tarHeader->Mtime, 12);     if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
  BZ2_bzRead(&bzerror, bzin, tarHeader->Chksum, 8);     if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
  BZ2_bzRead(&bzerror, bzin, tarHeader->Linkflag, 1);   if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
  BZ2_bzRead(&bzerror, bzin, tarHeader->Linkname, 100); if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
  BZ2_bzRead(&bzerror, bzin, tarHeader->Magic, 8);      if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
  BZ2_bzRead(&bzerror, bzin, tarHeader->Uname, 32);     if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
  BZ2_bzRead(&bzerror, bzin, tarHeader->Gname, 32);     if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
  BZ2_bzRead(&bzerror, bzin, tarHeader->Devmajor, 8);   if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
  BZ2_bzRead(&bzerror, bzin, tarHeader->Devminor, 8);   if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
  BZ2_bzRead(&bzerror, bzin, tarHeader->Padding, 167);  if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
  
  return bzerror;
}

unsigned int TarComputeChecksum(struct TarHeader* tarHeader) {
  unsigned int sum = 0;
  char blankChecksum[8+1] = "        ";
  for (int i = 0; i < 100; i++) { sum += tarHeader->Name[i]; }
  for (int i = 0; i <   8; i++) { sum += tarHeader->Mode[i]; }
  for (int i = 0; i <   8; i++) { sum += tarHeader->Uid[i]; }
  for (int i = 0; i <   8; i++) { sum += tarHeader->Gid[i]; }
  for (int i = 0; i <  12; i++) { sum += tarHeader->Size[i]; }
  for (int i = 0; i <  12; i++) { sum += tarHeader->Mtime[i]; }
  //for (int i = 0; i <   8; i++) { sum += tarHeader->Chksum[i]; }
  for (int i = 0; i <   8; i++) { sum += blankChecksum[i]; }
  for (int i = 0; i <   1; i++) { sum += tarHeader->Linkflag[i]; }
  for (int i = 0; i < 100; i++) { sum += tarHeader->Linkname[i]; }
  for (int i = 0; i <   8; i++) { sum += tarHeader->Magic[i]; }
  for (int i = 0; i <  32; i++) { sum += tarHeader->Uname[i]; }
  for (int i = 0; i <  32; i++) { sum += tarHeader->Gname[i]; }
  for (int i = 0; i <   8; i++) { sum += tarHeader->Devmajor[i]; }
  for (int i = 0; i <   8; i++) { sum += tarHeader->Devminor[i]; }
  for (int i = 0; i < 167; i++) { sum += tarHeader->Padding[i]; }
  return sum;
}


void GetFirstDir(const char* path, char* dmodDir) {
    char* firstDir = "";
    char* previousDir = "";
    char* checkPath = malloc(strlen(path)+strlen("dummy")+1);
    strcpy(checkPath, path);
    // tokenizer never returns empty strings + distinguish dir// and a/$
    char last = path[strlen(path)-1];
    if (last == '\\' || last == '/')
      strcat(checkPath, "dummy");
    
    char *str, *curDir, *saveptr;
    for (str = checkPath; ; str = NULL) {
      curDir = strtok_r(str, "/\\", &saveptr);
      if (curDir == NULL)
	break;
      if (strcmp(curDir, ".") == 0)
	continue;
      if (strcmp(previousDir, "") != 0) {
	firstDir = previousDir;
	break;
      }
      previousDir = curDir;
    }

    strcpy(dmodDir, firstDir);
    free(checkPath);
}

int TarIsPathInsecure(char* path, char* prefix) {
  if (path[0] == '/' || path[0] == '\\')
    return 1;
  {
    char *str, *token, *saveptr;
    str = strdup(path);
    token = strtok_r(str, "/\\", &saveptr);
    if (strcmp(token, prefix) != 0) {
      free(str);
      return 1;
    }
    free(str);
  }
  {
    char *str, *token, *saveptr;
    for (str = path; ; str = NULL) {
      token = strtok_r(str, "/\\", &saveptr);
      if (token == NULL)
	break;
      if (strcmp(token, "..") == 0)
	return 1;
    }
  }
  return 0;
}

int DmodExtract(FILE* dmodFile, char* destDir, char** extractedDmodDir) {
  // Get the file size
  unsigned long dmodFileCompressedSize;
  if (fseek(dmodFile, 0, SEEK_END) < 0) perror("fseek");
  dmodFileCompressedSize = ftell(dmodFile);
  if (fseek(dmodFile, 0, SEEK_SET) < 0) perror("fseek");
  
  int bzerror = BZ_OK;
  BZFILE* bzin = BZ2_bzReadOpen(&bzerror, dmodFile, 0, 0, 0, 0);
  if (bzerror != BZ_OK) {
    print_bzerror(bzin, bzerror);
    printf(_("Error: Invalid .dmod file selected!"));
    printf("\n");
    return -1;
  }
  
  struct TarHeader tarHeader;
  unsigned int total_read = 0;
  int lastPercent = 0;
  char dmodDir[100+1] = {};
  
  // Keep getting chunks of the file
  while (1) {
    unsigned char* buf;
    int nbReadUncompressed;

    int ret = TarFillHeaderFromStream(bzin, &tarHeader);
    total_read += 512;
    if (ret < 0) {
      break;
    }

    // Define dmodDir which needs to be consistent in all the archive
    if (strcmp(dmodDir, "") == 0) {
      GetFirstDir(tarHeader.Name, dmodDir);
      if (strcmp(dmodDir, "") == 0 || strcmp(dmodDir, "..") == 0)
	{
	  printf(_("Error: empty D-Mod directory.  Stopping.\n"));
	  return 1;
	}
      char* dmodFullPath = malloc(strlen(destDir)+1+strlen(dmodDir)+1);
      sprintf(dmodFullPath, "%s/%s", destDir, dmodDir);
      if (DirExists(dmodFullPath)) {
	  printf(_("Error: directory '%s' already exists. Stopping.\n"), dmodDir);
	  free(dmodFullPath);
	  return 1;
      }
      free(dmodFullPath);
      printf("D-Mod dir: %s\n", dmodDir);
    }

    // Bug fix for phantom files that aren't really files...
    // This is due to a bug in DFArc1.5 with empty files
    if (strcmp(tarHeader.Name, "\xFF") == 0) {
      continue;
    }

    // Tar files (except for a design bug in DFArc1.5) have a
    // multitude of NULL characters at the end to round up to the
    // 10K mark.  If the header is blank, we don't have any more
    // data.
    if (strcmp(tarHeader.Name, "") == 0) {
      break;
    }

    // TODO: ISO-8869-1 -> UTF-8?

    // Size
    unsigned long fileSize = 0;
    sscanf((const char*)&tarHeader.Size, "%lo", &fileSize);
  
    // Extract
    char last = tarHeader.Name[strlen(tarHeader.Name)-1];
    if (last == '\\' || last == '/') {
      // directory, skip and let mkdir_rec do its job
    } else {
      unsigned int checksum = 0;
      unsigned int expected = TarComputeChecksum(&tarHeader);
      sscanf((const char*)&tarHeader.Chksum, "%o", &checksum);
      if (checksum != expected) {
	printf("Incorrect checksum, but proceeding anyway: found 0%o, expected 0%o\n", checksum, expected);
      }

      char* checkName = strdup(tarHeader.Name);
      if (TarIsPathInsecure(checkName, dmodDir)) {
	printf("Insecure path, stopping: %s\n", tarHeader.Name);
	free(checkName);
	return 1;
      }
      free(checkName);    

      char* destFullPath = malloc(strlen(destDir)+1+strlen(tarHeader.Name)+1);
      sprintf(destFullPath, "%s/%s", destDir, tarHeader.Name);
      //printf("* Extracting to: %s\n", destFullPath);
      char* dir = strdup(destFullPath);
      dirname(dir);
      int ret = mkdir_rec(dir);
      if (ret < 0) printf("Error creating directory %s\n", dir);
      free(dir);
      if (ret < 0) {
	free(destFullPath);
	break;
      }

      {
	// bzip2 -9 -> 900kiB blocks - randomly using 4MiB buf, we
	// might get better precision with low-level BZ2 API
	char buf[4*1024*1024];
	FILE* out = fopen(destFullPath, "wb");
	if (out == NULL) {
	  perror("fopen");
	  break;
	}
	unsigned long nbRemaining = fileSize;
	while (nbRemaining > 0) {
	  nbReadUncompressed = (nbRemaining > sizeof(buf)) ? sizeof(buf) : nbRemaining;
	  nbReadUncompressed = BZ2_bzRead(&bzerror, bzin, buf, nbReadUncompressed);
	  if (bzerror != BZ_OK && bzerror != BZ_STREAM_END) { print_bzerror(bzin, bzerror); return 1; };
	  fwrite(buf, nbReadUncompressed, 1, out);
	  nbRemaining -= nbReadUncompressed;

	  {
	    unsigned long pos = ftell(dmodFile);
	    double percent = 100.0 * pos / dmodFileCompressedSize;
	    if ((int)percent != lastPercent)
	      printf("progress: %lu %lu %.02f%%\n", pos, dmodFileCompressedSize, percent);
	    lastPercent = (int)percent;
	  }
	}

	if (bzerror == BZ_STREAM_END) {
	  // old .mod without end-of-archive record nor padding, no error output
	  break;
	}
	fclose(out);
      }

      free(destFullPath);
      total_read += fileSize;
   }

    // Move to the beginning of the next header
    char padding[512];
    int padding_size = (512 - (total_read % 512)) % 512;
    BZ2_bzRead(&bzerror, bzin, padding, padding_size);
    if (bzerror != BZ_OK) { print_bzerror(bzin, bzerror); return 1; }
    total_read += padding_size;
  }
  
  BZ2_bzReadClose(&bzerror, bzin);

  if (extractedDmodDir != NULL) {
    *extractedDmodDir = strdup(dmodDir);
  }
  return 0;
}

int emDmodExtract(char* filename, char* destdir, char** extractedDmodDir) {
  // printf("emDmodExtract(%s, %s)\n", filename, destdir);
  FILE* in = fopen(filename, "rb");
  if (in == NULL) {
    perror("fopen");
    printf("Cannot open %s\n", filename);
    return 1;
  }
  return DmodExtract(in, destdir, extractedDmodDir);
}

#ifdef TEST
int main(int argc, char* argv[]) {
  {
    char buf[100+1];
    assert((strcpy(buf, "../"),               TarIsPathInsecure(buf, "..") == 1));
    assert((strcpy(buf, "/../"),              TarIsPathInsecure(buf, "") == 1));
    assert((strcpy(buf, "/../"),              TarIsPathInsecure(buf, "..") == 1));
    assert((strcpy(buf, ".."),                TarIsPathInsecure(buf, "..") == 1));
    assert((strcpy(buf, "/.."),               TarIsPathInsecure(buf, "") == 1));
    assert((strcpy(buf, "/.."),               TarIsPathInsecure(buf, "..") == 1));
    assert((strcpy(buf, "path/.."),           TarIsPathInsecure(buf, "path") == 1));
    assert((strcpy(buf, "path/subpath/.."),   TarIsPathInsecure(buf, "path") == 1));
    assert((strcpy(buf, "path/../subpath"),   TarIsPathInsecure(buf, "path") == 1));
    assert((strcpy(buf, "../path/subpath"),   TarIsPathInsecure(buf, "..") == 1));
    assert((strcpy(buf, "../path/subpath"),   TarIsPathInsecure(buf, "path") == 1));
    assert((strcpy(buf, "/path/subpath"),     TarIsPathInsecure(buf, "") == 1));
    assert((strcpy(buf, "/path/subpath"),     TarIsPathInsecure(buf, "path") == 1));
    assert((strcpy(buf, "\\path/subpath"),    TarIsPathInsecure(buf, "") == 1));
    assert((strcpy(buf, "\\path/subpath"),    TarIsPathInsecure(buf, "path") == 1));
    assert((strcpy(buf, "path\\..\\subpath"), TarIsPathInsecure(buf, "path") == 1));
    assert((strcpy(buf, "path"),              TarIsPathInsecure(buf, "path") == 0));
    assert((strcpy(buf, "path"),              TarIsPathInsecure(buf, "path") == 0));
    assert((strcpy(buf, "path/"),             TarIsPathInsecure(buf, "path") == 0));
    assert((strcpy(buf, "path\\"),            TarIsPathInsecure(buf, "path") == 0));
    assert((strcpy(buf, "path/subpath"),      TarIsPathInsecure(buf, "path") == 0));
    assert((strcpy(buf, "path\\subpath"),     TarIsPathInsecure(buf, "path") == 0));
    assert((strcpy(buf, "path2\\subpath"),    TarIsPathInsecure(buf, "path") == 1));
    assert((strcpy(buf, "dink.exe"),          TarIsPathInsecure(buf, "path") == 1));
    assert((strcpy(buf, "dink.exe"),          TarIsPathInsecure(buf, "path") == 1));
  }
  return emDmodExtract(argv[1], "/tmp/testdmod", NULL);
}
#endif
