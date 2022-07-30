#ifdef _USE_UNPACK
/*
   miniunz.c
   Version 1.1, February 14h, 2010
   sample part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

         Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

         Modifications of Unzip for Zip64
         Copyright (C) 2007-2008 Even Rouault

         Modifications for Zip64 support on both zip and unzip
         Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )
*/

#ifndef _WIN32
#ifndef __USE_FILE_OFFSET64
#define __USE_FILE_OFFSET64
#endif
#ifndef __USE_LARGEFILE64
#define __USE_LARGEFILE64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BIT
#define _FILE_OFFSET_BIT 64
#endif
#endif

#include "externals.h"

#ifdef unix
# include <unistd.h>
# include <utime.h>
#else
# include <direct.h>
# include <io.h>
#endif

#include "unzip.h"
#include "utils.h"

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (8192)
#define MAXFILENAME (256)

#ifdef _WIN32
#define USEWIN32IOAPI
#include "iowin32.h"
#endif

char zip_game_dirname[PATH_MAX];

/* change_file_date : change the date/time of a file
    filename : the filename of the file where date/time must be modified
    dosdate : the new date at the MSDos format (4 bytes)
    tmu_date : the SAME new date at the tm_unz format */
static void change_file_date(filename, dosdate, tmu_date)
const char *filename;
uLong dosdate;
tm_unz tmu_date;
{
#ifdef _WIN32
	HANDLE hFile;
	FILETIME ftm, ftLocal, ftCreate, ftLastAcc, ftLastWrite;

	hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE,
			    0, NULL, OPEN_EXISTING, 0, NULL);
	GetFileTime(hFile, &ftCreate, &ftLastAcc, &ftLastWrite);
	DosDateTimeToFileTime((WORD) (dosdate >> 16), (WORD) dosdate, &ftLocal);
	LocalFileTimeToFileTime(&ftLocal, &ftm);
	SetFileTime(hFile, &ftm, &ftLastAcc, &ftm);
	CloseHandle(hFile);
#else
#ifdef unix
	struct utimbuf ut;
	struct tm newdate;
	newdate.tm_sec = tmu_date.tm_sec;
	newdate.tm_min = tmu_date.tm_min;
	newdate.tm_hour = tmu_date.tm_hour;
	newdate.tm_mday = tmu_date.tm_mday;
	newdate.tm_mon = tmu_date.tm_mon;
	if (tmu_date.tm_year > 1900)
		newdate.tm_year = tmu_date.tm_year - 1900;
	else
		newdate.tm_year = tmu_date.tm_year;
	newdate.tm_isdst = -1;

	ut.actime = ut.modtime = mktime(&newdate);
	utime(filename, &ut);
#endif
#endif
}

/* mymkdir and change_file_date are not 100 % portable
   As I don't know well Unix, I wait feedback for the unix portion */

static int mymkdir(dirname)
const char *dirname;
{
	int ret = 0;
#ifdef _WIN32
	ret = _mkdir(dirname);
#else
#ifdef unix
	ret = mkdir(dirname, 0775);
#endif
#endif
	return ret;
}

static int makedir(newdir)
const char *newdir;
{
	char *buffer;
	char *p;
	int len = (int)strlen(newdir);

	if (len <= 0)
		return 0;

	buffer = (char *)malloc(len + 1);
	if (buffer == NULL) {
		fprintf(stderr, "Error allocating memory\n");
		return UNZ_INTERNALERROR;
	}
	strcpy(buffer, newdir);

	if (buffer[len - 1] == '/') {
		buffer[len - 1] = '\0';
	}
	if (mymkdir(buffer) == 0) {
		free(buffer);
		return 1;
	}

	p = buffer + 1;
	while (1) {
		char hold;

		while (*p && *p != '\\' && *p != '/')
			p++;
		hold = *p;
		*p = 0;
		if ((mymkdir(buffer) == -1) && (errno == ENOENT)) {
			fprintf(stderr, "couldn't create directory %s\n", buffer);
			free(buffer);
			return 0;
		}
		if (hold == 0)
			break;
		*p++ = hold;
	}
	free(buffer);
	return 1;
}

static int do_extract_currentfile(uf, password)
unzFile uf;
const char *password;
{
	char filename_inzip[256];
	char dir_inzip[256];
	char *filename_withoutpath;
	char *p;
	int err = UNZ_OK;
	FILE *fout = NULL;
	void *buf;
	uInt size_buf;

	unz_file_info64 file_info;
	err =
	    unzGetCurrentFileInfo64(uf, &file_info, filename_inzip,
				    sizeof(filename_inzip), NULL, 0, NULL, 0);

	if (err != UNZ_OK) {
		fprintf(stderr, "error %d with zipfile in unzGetCurrentFileInfo\n", err);
		return err;
	}

	size_buf = WRITEBUFFERSIZE;
	buf = (void *)malloc(size_buf);
	if (buf == NULL) {
		fprintf(stderr, "Error allocating memory\n");
		return UNZ_INTERNALERROR;
	}

	p = filename_withoutpath = filename_inzip;
	strcpy(dir_inzip, filename_inzip);
	while ((*p) != '\0') {
		if (((*p) == '/') || ((*p) == '\\'))
			filename_withoutpath = p + 1;
		p++;
	}

	p = dir_inzip + strcspn(dir_inzip, "/\\");
	*p = 0;

	if ((*filename_withoutpath) == '\0') {
		if (zip_game_dirname[0] && strncmp(zip_game_dirname, 
				filename_inzip, strlen(zip_game_dirname))) {
			err = -1;
			fprintf(stderr, "Too many dirs in zip...\n");
			goto out;
		}
		fprintf(stderr, "creating directory: %s\n", filename_inzip);
		mymkdir(filename_inzip);
		if (!*zip_game_dirname)
			strcpy(zip_game_dirname, dir_inzip);
	} else {
		const char *write_filename;
		int skip = 0;
		write_filename = filename_inzip;

		err = unzOpenCurrentFilePassword(uf, password);
		if (err != UNZ_OK) {
			fprintf
			    (stderr, "error %d with zipfile in unzOpenCurrentFilePassword\n",
			     err);
			goto out;
		}

		if (skip == 0) {
			fout = fopen64(write_filename, "wb");

			/* some zipfile don't contain directory alone before file */
			if ((fout == NULL)
			    && (filename_withoutpath != (char *)filename_inzip)) {
				char c = *(filename_withoutpath - 1);
				*(filename_withoutpath - 1) = '\0';
				makedir(write_filename);
				*(filename_withoutpath - 1) = c;
				fout = fopen64(write_filename, "wb");
			}

			if (fout == NULL) {
				fprintf(stderr, "error opening %s\n", write_filename);
			}
		}

		if ((filename_withoutpath != (char *)filename_inzip) && 
			!*zip_game_dirname) {
			strcpy(zip_game_dirname, dir_inzip);
		}


		if (fout != NULL) {
			fprintf(stderr, " extracting: %s\n", write_filename);

			do {
				err = unzReadCurrentFile(uf, buf, size_buf);
				if (err < 0) {
					fprintf
					    (stderr, "error %d with zipfile in unzReadCurrentFile\n",
					     err);
					break;
				}
				if (err > 0)
					if (fwrite(buf, err, 1, fout) != 1) {
						fprintf
						    (stderr, "error in writing extracted file\n");
						err = UNZ_ERRNO;
						break;
					}
			}
			while (err > 0);
			if (fout)
				fclose(fout);

			if (err == 0)
				change_file_date(write_filename,
						 file_info.dosDate,
						 file_info.tmu_date);
		}

		if (err == UNZ_OK) {
			err = unzCloseCurrentFile(uf);
			if (err != UNZ_OK) {
				fprintf
				    (stderr, "error %d with zipfile in unzCloseCurrentFile\n",
				     err);
			}
		} else
			unzCloseCurrentFile(uf);	/* don't lose the error */

		if (!*zip_game_dirname) {
			err = -1;
			fprintf(stderr, "No dir in zip...\n");
			if (idf_magic(write_filename)) {
				fprintf(stderr, "Idf unpacked: %s\n", write_filename);
				strcpy(zip_game_dirname, write_filename);
				err = 0;
			} else
				unlink(write_filename);
			goto out;
		}
	}

	if (*zip_game_dirname) {
		size_t s = strlen(zip_game_dirname);
		unix_path(zip_game_dirname);
		if (s && (zip_game_dirname[s - 1] == '/'))
			s --;
		zip_game_dirname[s] = 0;
	}
out:
	free(buf);
	return err;
}

static int do_extract(uf, password)
unzFile uf;
const char *password;
{
	uLong i;
	unz_global_info64 gi;
	int err;
	err = unzGetGlobalInfo64(uf, &gi);
	if (err != UNZ_OK)
		fprintf(stderr, "error %d with zipfile in unzGetGlobalInfo \n", err);

	for (i = 0; i < gi.number_entry; i++) {
		if (do_extract_currentfile(uf, password) != UNZ_OK)
			return -1;

		if ((i + 1) < gi.number_entry) {
			err = unzGoToNextFile(uf);
			if (err != UNZ_OK) {
				fprintf
				    (stderr, "error %d with zipfile in unzGoToNextFile\n",
				     err);
				return -1;
			}
		}
	}

	return 0;
}

int unpack(const char *zipfilename, const char *dirname)
{
	char game_cwd[PATH_MAX];
	unzFile uf;
	char filename_try[MAXFILENAME + 16] = "";
	int ret_value = 0;

	if (dirname && !getcwd(game_cwd, sizeof(game_cwd))) {
		fprintf(stderr, "Error: can not get current dir.\n");
		return -1;
	}
	
	uf = NULL;
	zip_game_dirname[0] = 0;
#        ifdef USEWIN32IOAPI
	zlib_filefunc64_def ffunc;
#        endif

	strncpy(filename_try, zipfilename, MAXFILENAME - 1);
	/* strncpy doesnt append the trailing NULL, of the string is too long. */
	filename_try[MAXFILENAME] = '\0';

#        ifdef USEWIN32IOAPI
	fill_win32_filefunc64A(&ffunc);
	uf = unzOpen2_64(zipfilename, &ffunc);
#        else
	uf = unzOpen64(zipfilename);
#        endif
	if (uf == NULL) {
		strcat(filename_try, ".zip");
#            ifdef USEWIN32IOAPI
		uf = unzOpen2_64(filename_try, &ffunc);
#            else
		uf = unzOpen64(filename_try);
#            endif
	}

	if (uf == NULL) {
		fprintf(stderr, "Cannot open %s or %s.zip\n", zipfilename,
			zipfilename);
		return -1;
	}
	fprintf(stderr, "%s opened\n", filename_try);
#ifdef _WIN32
	if (dirname && _chdir(dirname))
#else
	if (dirname && chdir(dirname))
#endif
	{
		ret_value = -1;
		fprintf(stderr, "Error changing dir to %s, aborting\n", dirname);
		goto out;
	}
	ret_value = do_extract(uf, NULL);
 out:
	unzClose(uf);
#ifdef _WIN32
	if (dirname)
		_chdir(game_cwd);
#else
	if (dirname) {
		if (chdir(game_cwd))
			fprintf(stderr, "Warning: can not chdir.\n");
	}
#endif
	return ret_value;
}
#endif
