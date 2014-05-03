/******************************************************************************
 *  
 * EPSUTIL.CPP      This contains the non-Tiff related private member functions
 *                  for the BitmapIO_EPS class
 *                  Started from the code for 3DSR4 and converted to
 *                  code for MAX.
 * 
 *  Written by Autodesk for 3D Studio MAX
 *  
 *  (C) Copyright 1995 by Autodesk, Inc.
 *
 *  This program is copyrighted by Autodesk, Inc. and is licensed to you under
 *  the following conditions.  You may not distribute or publish the source
 *  code of this program in any form.  You may incorporate this code in object
 *  form in derivative works provided such derivative works are (i.) are de-
 *  signed and intended to work solely with Autodesk, Inc. products, and (ii.)
 *  contain Autodesk's copyright notice "(C) Copyright 1994 by Autodesk, Inc."
 *
 *  AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS.  AUTODESK SPE-
 *  CIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR
 *  A PARTICULAR USE.  AUTODESK, INC.  DOES NOT WARRANT THAT THE OPERATION OF
 *  THE PROGRAM WILL BE UNINTERRUPTED OR ERROR FREE.
 *
 * AUTHOR:           Keith Trummel
 *
 * HISTORY:          05/22/95 Originated
 * 
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "max.h"
#include "bmmlib.h"
#include "eps.h"
#include "epsres.h"

extern HINSTANCE hResource;

/* To speed up writing of hexidecimal image data to file */
static const char charTable[] = {'0', '1', '2', '3', '4', '5', '6', '7',
				 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};


/* This writes the boiler plate for the PostScript
 * We don't bother to check the return value of every single _ftprintf, but
 * we do check the last so that if we run out of disk space we don't attempt
 * do any more and report the error.
 */
int
BitmapIO_EPS::WriteHeader (Bitmap *bitmap, int xOffset, int yOffset)
{
    TCHAR date_string[80];
    TCHAR *ch_ptr;
    time_t  cur_time;
    
    cur_time = time(NULL);
    
    /* First the header */
	if (_ftprintf (mOutStream, _T("%%!PS-Adobe-3.0 EPSF-3.0\r\n")) < 0)
        return BMMRES_IOERROR;
	if (fflush (mOutStream) == EOF)
        return BMMRES_IOERROR;
    
    if (userSettings.orientation == PORTRAIT)
		_ftprintf (mOutStream, _T( "%%%%BoundingBox: %d %d %d %d\r\n"), xOffset, yOffset,
		 (int) (xOffset + 72.0 * bitmap->Width() / 
			userSettings.xResolution + 0.999),
		 (int) (yOffset + 72.0 * bitmap->Height() / 
			userSettings.yResolution + 0.999));
    else
		_ftprintf (mOutStream, _T("%%%%BoundingBox: %d %d %d %d\r\n"), xOffset, yOffset,
		 (int) (xOffset + 72.0 * bitmap->Height() / 
			userSettings.yResolution + 0.999),
		 (int) (yOffset + 72.0 * bitmap->Width() / 
			userSettings.xResolution + 0.999));
	_ftprintf (mOutStream, 
#ifdef DESIGN_VER
	     _T("%%%%Creator: Autodesk VIZ (TM)\r\n"));
#else
	     _T("%%%%Creator: Autodesk 3D Studio MAX (TM)\r\n"));
#endif // DESIGN_VER
    
    // The title is unfortunately not entirely straight forward
    // We would like to use the name of the current file, but that
    // may contain characters outside the printable ASCII character set
    // which is not recommended for PostScript.  Therefore, we
    // print the title only if it is printable ASCII
    Interface *max = TheManager->Max ();
    TSTR &maxFileName = max->GetCurFileName ();
    const TCHAR *str = maxFileName.data ();
    int legal = 1;
    int length = static_cast<int>(_tcslen(str));	// SR DCAST64: Downcast to 2G limit.
    for (int i = 0; i < length; i++) {
	if (str[i] < 32 && str[i] > 127) {
	    legal = 0;
	    break;
	}
    }

    if (legal)
		_ftprintf (mOutStream, _T("%%%%Title: %s\r\n"), maxFileName);
    
    /* ctime puts a linefeed on the string, but we wish to be DOS centric so
     * we want a carriage return - linefeed combination, so we have to do
     * the following contortions.
     */
    _tcscpy (date_string, _tctime (&cur_time));
    ch_ptr = _tcschr (date_string, '\n');
    if (ch_ptr) {
        *ch_ptr++ = '\r';
        *ch_ptr++ = '\n';
        *ch_ptr = 0;
    }
	_ftprintf (mOutStream, _T("%%%%CreationDate: %s"), date_string);
	_ftprintf (mOutStream, _T("%%%%DocumentFonts:\r\n"));
	_ftprintf (mOutStream, _T("%%%%DocumentNeededFonts:\r\n"));
    if (userSettings.binary)
		_ftprintf (mOutStream, _T("%%%%DocumentData: Binary\r\n"));
    else
		_ftprintf (mOutStream, _T("%%%%DocumentData: Clean7Bit\r\n"));
	_ftprintf (mOutStream, _T("%%%%EndComments\r\n"));
    
    /* Now the prolog */
	_ftprintf (mOutStream, _T("\r\n%%%%BeginProlog\r\n"));
	_ftprintf (mOutStream, _T("%%%%EndProlog\r\n"));
    
    /* Now the setup */
	_ftprintf (mOutStream, _T("\r\n%%%%BeginSetup\r\n"));
	_ftprintf (mOutStream, _T("%%%%EndSetup\r\n\r\n"));
    
	_ftprintf (mOutStream, _T("save\r\n"));
    
    /* Create our own dictionary so don't pollute current dictionary.
     * Requires only room for "rgb2gray", "rgbstring" and "graystring"
     */
	_ftprintf (mOutStream, _T("3 dict begin\r\n"));
    
	_ftprintf (mOutStream,
             _T("%% Takes a string of RGB values and a string for the gray\r\n"));
	_ftprintf (mOutStream,
             _T("%% values and writes the gray values into the provided\r\n"));
	_ftprintf (mOutStream, _T("%% string and leaves it on the stack\r\n"));
	_ftprintf (mOutStream, _T("/rgb2gray\r\n"));
	_ftprintf (mOutStream, _T("{\r\n"));
	_ftprintf (mOutStream, _T("  1 index length 3 div cvi 0 1 3 -1 roll 1 sub \r\n"));
	_ftprintf (mOutStream, _T("  {\r\n"));
	_ftprintf (mOutStream,
             _T("    2 copy 4 index 3 index 3 mul get 0.3 mul 5 index 4 index\r\n"));
	_ftprintf (mOutStream,
             _T("    3 mul 1 add get 0.59 mul add 5 index 5 -1 roll 3 mul 2\r\n")); 
	_ftprintf (mOutStream, _T("    add get 0.11 mul add cvi put\r\n"));
	_ftprintf (mOutStream, _T("  } \r\n"));
	if (_ftprintf (mOutStream, _T("  for\r\n  exch pop\r\n} bind def\r\n")) < 0)
        return BMMRES_IOERROR;
	if (fflush (mOutStream) == EOF)
        return BMMRES_IOERROR;
    
    return BMMRES_SUCCESS;    
}

/* This writes the PostScript that positions the image correctly */
int
BitmapIO_EPS::WriteImagePosition (Bitmap *bitmap, 
				  int xOffset, int yOffset)
{
    if (userSettings.orientation == PORTRAIT)
		_ftprintf (mOutStream, _T("%d %d translate\r\n"), xOffset, yOffset);
    else
    {
		_ftprintf (mOutStream, _T("%d %d translate\r\n"),
                 xOffset + (int) (bitmap->Height() * 72.0 / 
				  userSettings.yResolution),
                 yOffset);
		_ftprintf (mOutStream, _T("90 rotate\r\n"));
    }
	_ftprintf (mOutStream, _T("%s "),
             PeriodFloat (bitmap->Width() * 72.0 / userSettings.xResolution,
	                  _T("%lf")));
	_ftprintf (mOutStream, _T("%s scale\r\n"),
             PeriodFloat (bitmap->Height() * 72.0 / userSettings.yResolution,
	                  _T("%lf")));
    
    return BMMRES_SUCCESS;
}

/* This writes the image data when ASCII RGB data is desired 
 * We don't bother to check the return value of every single _ftprintf, but
 * we do check the last so that if we run out of disk space we don't attempt
 * do any more and report the error.
 */
int
BitmapIO_EPS::WriteAsciiRGBImage (Bitmap *bitmap)
{
    BMM_Color_32 *fullColor, *color;
    unsigned char buffer[MAX_CHARS_PER_LINE + 8];
    int status = BMMRES_SUCCESS;
    int i, j, n;

    fullColor = (BMM_Color_32 *) malloc (bitmap->Width() *
					 sizeof(BMM_Color_32));
    if (! fullColor) {
	status = BMMRES_MEMORYERROR;
	goto cleanup;
    }

	_ftprintf (mOutStream, _T("/rgbstring %d string def\r\n"), 3 * bitmap->Width());
	_ftprintf (mOutStream, _T("/graystring %d string def\r\n"), bitmap->Width());
	_ftprintf (mOutStream, _T("%d %d 8 [%d 0 0 -%d 0 %d]\r\n"),
             bitmap->Width(), bitmap->Height(),
             bitmap->Width(), bitmap->Height(), bitmap->Height());
    
	_ftprintf (mOutStream, _T("/colorimage where\r\n"));
	_ftprintf (mOutStream,
       _T("{pop {currentfile rgbstring readhexstring pop} bind false 3 colorimage}\r\n"));
	_ftprintf (mOutStream,
    _T("{{currentfile rgbstring readhexstring pop graystring rgb2gray} bind image}\r\n"));
	_ftprintf (mOutStream, _T("ifelse\r\n"));

    /* Loop through a scanline at a time */
    for (i = 0, n = 0; i < bitmap->Height(); i++)
    {
	if (GetDitheredOutputPixels (0, i, bitmap->Width(), fullColor) != 1) {
	    status = BMMRES_IOERROR;
	    goto cleanup;
	}
	if (hWnd)
	    SendMessage (hWnd, BMM_PROGRESS, i, bitmap->Height()-1);
        for (j = 0, color = fullColor; j < bitmap->Width(); j++, color++)
        {
            buffer[n++] = charTable[color->r >> 4];
            buffer[n++] = charTable[color->r & 0xf];            
            buffer[n++] = charTable[color->g >> 4];
            buffer[n++] = charTable[color->g & 0xf];            
            buffer[n++] = charTable[color->b >> 4];
            buffer[n++] = charTable[color->b & 0xf];

            if (n >= MAX_CHARS_PER_LINE)
            {
                buffer[n++] = '\r';
                buffer[n++] = '\n';
				if (fwrite (buffer, n, 1, mOutStream) != 1)
                {
                    status = BMMRES_IOERROR;
		    goto cleanup;
                }
                n = 0;
            }
        }
    }

    /* Take care of any remaining data */
    if (n > 0)
    {
        buffer[n++] = '\r';
        buffer[n++] = '\n';
		if (fwrite (buffer, n, 1, mOutStream) != 1)
        {
	    status = BMMRES_IOERROR;
	    goto cleanup;
        }
    }

 cleanup:
    free (fullColor);
    return status;
}

/* This writes the image data when ASCII gray data is desired 
 * We don't bother to check the return value of every single _ftprintf, but
 * we do check the last so that if we run out of disk space we don't attempt
 * do any more and report the error.
 */
int
BitmapIO_EPS::WriteAsciiGrayImage (Bitmap *bitmap)
{
    BMM_Color_32 *fullColor, *color;
    unsigned char gray;
    unsigned char buffer[MAX_CHARS_PER_LINE + 8];
    int status = BMMRES_SUCCESS;
    int i, j, n;

    fullColor = (BMM_Color_32 *) malloc (bitmap->Width() * sizeof(BMM_Color_32));
    if (! fullColor) {
	status = BMMRES_IOERROR;
	goto cleanup;
    }

	_ftprintf (mOutStream, _T("/graystring %d string def\r\n"), bitmap->Width());
	_ftprintf (mOutStream, _T("%d %d 8 [%d 0 0 -%d 0 %d]\r\n"),
             bitmap->Width(), bitmap->Height(),
             bitmap->Width(), bitmap->Height(), bitmap->Height());
    
	_ftprintf (mOutStream,
             _T("{currentfile graystring readhexstring pop} bind\r\nimage\r\n"));

    /* Loop through a scanline at a time */
    for (i = 0, n = 0; i < bitmap->Height(); i++) {
	if (GetDitheredOutputPixels (0, i, bitmap->Width(), fullColor) != 1) {
	    status = BMMRES_IOERROR;
	    goto cleanup;
	}
	if (hWnd)
	    SendMessage (hWnd, BMM_PROGRESS, i, bitmap->Height()-1);
        for (j = 0, color = fullColor; j < bitmap->Width(); j++, color++) {
            gray = (unsigned char) (0.3 * color->r + 
				    0.59 * color->g + 
				    0.11 * color->b + 0.5);
            buffer[n++] = charTable[gray >> 4];
            buffer[n++] = charTable[gray & 0xf];            
            if (n >= MAX_CHARS_PER_LINE) {
                buffer[n++] = '\r';
                buffer[n++] = '\n';
				if (fwrite (buffer, n, 1, mOutStream) != 1) {
		    status = BMMRES_IOERROR;
		    goto cleanup;
                }
                n = 0;
            }
        }
    }

    /* Take care of any remaining data */
    if (n > 0) {
        buffer[n++] = '\r';
        buffer[n++] = '\n';
		if (fwrite (buffer, n, 1, mOutStream) != 1) {
	    status = BMMRES_IOERROR;
	    goto cleanup;
        }
    }

 cleanup:
    free (fullColor);
    return status;
}

/* This writes the image data when binary RGB data is desired
 * We don't bother to check the return value of every single _ftprintf, but
 * we do check the last so that if we run out of disk space we don't attempt
 * do any more and report the error.
 */
int
BitmapIO_EPS::WriteBinaryRGBImage (Bitmap *bitmap)
{
    BMM_Color_32 *fullColor;
    unsigned char *buffer;
    int status = BMMRES_SUCCESS;
    int i, j, n;

    fullColor = (BMM_Color_32 *) malloc (bitmap->Width() *
					 sizeof(BMM_Color_32));
    buffer = (unsigned char *) malloc (3 * bitmap->Width());
    if (! fullColor || ! buffer) {
	status = BMMRES_MEMORYERROR;
	goto cleanup;
    }
    
	_ftprintf (mOutStream, _T("/rgbstring %d string def\r\n"), 3 * bitmap->Width());
	_ftprintf (mOutStream, _T("/graystring %d string def\r\n"), bitmap->Width());
	_ftprintf (mOutStream, _T("%d %d 8 [%d 0 0 -%d 0 %d]\r\n"),
             bitmap->Width(), bitmap->Height(),
             bitmap->Width(), bitmap->Height(), bitmap->Height());
	_ftprintf (mOutStream, _T("/colorimage where\r\n"));
	_ftprintf (mOutStream,
       _T("{pop {currentfile rgbstring readstring pop} bind false 3 colorimage}\r\n"));
	_ftprintf (mOutStream,
    _T("{{currentfile rgbstring readstring pop graystring rgb2gray} bind image}\r\n"));
	_ftprintf (mOutStream, _T("%%%%BeginBinary: %d\r\n"),
             strlen ("ifelse\r\n") + 3 * bitmap->Width() * bitmap->Height());
	_ftprintf (mOutStream, _T("ifelse\r\n"));

    /* Loop through a scanline at a time */
    for (i = 0; i < bitmap->Height(); i++)
    {
	if (GetDitheredOutputPixels (0, i, bitmap->Width(), fullColor) != 1) {
	    status = BMMRES_IOERROR;
	    goto cleanup;
	}
	if (hWnd)
	    SendMessage (hWnd, BMM_PROGRESS, i, bitmap->Height()-1);
        for (j = 0, n = 0; j < bitmap->Width(); j++) {
            buffer[n++] = fullColor[j].r;
            buffer[n++] = fullColor[j].g;
            buffer[n++] = fullColor[j].b;
        }
		if (fwrite (buffer, 3 * bitmap->Width(), 1, mOutStream) != 1) {
	    status = BMMRES_IOERROR;
	    goto cleanup;
        }
    }

	_ftprintf (mOutStream, _T("\r\n%%%%EndBinary\r\n"));

 cleanup:
    free (fullColor);
    free (buffer);

    return status;
}

/* This writes the image data when binary gray data is desired
 * We don't bother to check the return value of every single _ftprintf, but
 * we do check the last so that if we run out of disk space we don't attempt
 * do any more and report the error.
 */
int
BitmapIO_EPS::WriteBinaryGrayImage (Bitmap *bitmap)
{
    BMM_Color_32 *fullColor;
    unsigned char *buffer;
    int status = BMMRES_SUCCESS;
    int i, j;

    fullColor = (BMM_Color_32 *) malloc (bitmap->Width() *
					 sizeof(BMM_Color_32));
    buffer = (unsigned char *) malloc (3 * bitmap->Width());
    if (! fullColor || ! buffer) {
	status = BMMRES_MEMORYERROR;
	goto cleanup;
    }
    
	_ftprintf (mOutStream, _T("/graystring %d string def\r\n"), bitmap->Width());
	_ftprintf (mOutStream, _T("%d %d 8 [%d 0 0 -%d 0 %d]\r\n"),
             bitmap->Width(), bitmap->Height(),
             bitmap->Width(), bitmap->Height(), bitmap->Height());
	_ftprintf (mOutStream,
             _T("{currentfile graystring readstring pop} bind\r\n"));
	_ftprintf (mOutStream, _T("%%%%BeginBinary: %d\r\n"),
             strlen ("image\r\n") + bitmap->Width() * bitmap->Height());
	_ftprintf (mOutStream, _T("image\r\n"));

    /* Loop through a scanline at a time */
    for (i = 0; i < bitmap->Height(); i++)
    {
	if (GetDitheredOutputPixels (0, i, bitmap->Width(), fullColor) != 1) {
	    status = BMMRES_IOERROR;
	    goto cleanup;
	}
	if (hWnd)
	    SendMessage (hWnd, BMM_PROGRESS, i, bitmap->Height()-1);
        for (j = 0; j < bitmap->Width(); j++)
            buffer[j] = (unsigned char) (0.3 * fullColor[j].r + 
					 0.59 * fullColor[j].g + 
					 0.11 * fullColor[j].b + 0.5);
		if (fwrite (buffer, bitmap->Width(), 1, mOutStream) != 1) {
	    status = BMMRES_IOERROR;
	    goto cleanup;
        }
    }

	_ftprintf (mOutStream, _T("\r\n%%%%EndBinary\r\n"));

 cleanup:
    free (fullColor);
    free (buffer);
    return status;
}

/* Write the standard termination.
 * Note that we do output a showpage which is optional for EPSF.
 * We choose to do so because that allows someone to print the file as
 * is and not have the nasty feature of sending a huge file to a PostScript
 * printer and have it chug away for ever only to never print anything.
 * We don't bother to check the return value of every single _ftprintf, but
 * we do check the last so that if we run out of disk space we report the
 * error.
 */
int
BitmapIO_EPS::WriteTrailer ()
{
    /* Terminate our dictionary */
	_ftprintf (mOutStream, _T("end\r\n"));

	_ftprintf (mOutStream, _T("restore\r\n"));

	_ftprintf (mOutStream, _T("\r\nshowpage\r\n"));
	_ftprintf (mOutStream, _T("\r\n%%%%Trailer\r\n"));
	if (_ftprintf (mOutStream, _T("%%%%EOF\r\n")) < 0)
        return BMMRES_IOERROR;
	if (fflush (mOutStream) == EOF)
        return BMMRES_IOERROR;

    return BMMRES_SUCCESS;        
}


/* Computes where to place the image given the size and resolution of the image
 */
void
BitmapIO_EPS::Position (Bitmap *bitmap, int *xOffset, int *yOffset)
{
    /* If the page width or height is 0 or less we simply position the image's
     * lower left corner at (0,0)
     */
    if (userSettings.paperWidth <= 0 || userSettings.paperHeight <= 0) {
        *xOffset = 0;
        *yOffset = 0;
    }

    /* Place the image so that the center of the image is on the center of
     * the page
     */
    else {
        if (userSettings.orientation == PORTRAIT) {
            *xOffset = (int) (0.5 * PS_UNITS_PER_INCH *
			      (userSettings.paperWidth - bitmap->Width() / 
			       userSettings.xResolution));
            *yOffset = (int) (0.5 * PS_UNITS_PER_INCH *
			      (userSettings.paperHeight - bitmap->Height() / 
			       userSettings.yResolution));
        }
        else {
            *xOffset = (int) (0.5 * PS_UNITS_PER_INCH *
			      (userSettings.paperWidth - bitmap->Height() / 
			       userSettings.yResolution));
            *yOffset = (int) (0.5 * PS_UNITS_PER_INCH *
			      (userSettings.paperHeight - bitmap->Width() / 
			       userSettings.xResolution));
        }
    }
}


BMMRES
BitmapIO_EPS::ReadHeader (const TCHAR *filename)
{
    char line[MAX_DSC_LINE_LENGTH + 1];
    BMMRES status = BMMRES_SUCCESS;
    int done = 0;
    
    LoadString (hResource, IDS_UNKNOWN, creator, _countof(creator));
    LoadString (hResource, IDS_UNKNOWN, title, _countof(title));
    LoadString (hResource, IDS_UNKNOWN, creationDate, _countof(creationDate));

    // We also need to hande case of a TIFF preview
	if ((mInputStream = _tfopen(filename, _T("r"))) == NULL) {
        status = BMMRES_FILENOTFOUND;
        goto cleanup;
    }

    // If there is an included preview we need to read the header
    // and determine where the PostScript code is
    if (! PositionToBeginOfPostScript ()) {
        status = BMMRES_INVALIDFORMAT;
        goto cleanup;
    }

    // Now read and parse the comments
    chPending = 0;
    if (! PSReadLine (line)) {
        status = BMMRES_INVALIDFORMAT;
        goto cleanup;
    }
    if (strncmp (line, EPSF_COMMENT, strlen(EPSF_COMMENT)) != 0  ||
        ! (strstr(line, "EPSF"))) {
        status = BMMRES_INVALIDFORMAT; 
        goto cleanup;
    }

	Interface14 *iface = GetCOREInterface14();
	LANGID langID = iface->LanguageToUseForFileIO();
	UINT codePage = iface->CodePageForLanguage(langID);

    while (PSReadLine (line) && ! done) {
        // Only read until hit end of prolog 
        if (strncmp (line, END_COMMENTS, strlen (END_COMMENTS)) == 0)
            done = 1;
        else if (strncmp (line, TITLE, strlen (TITLE)) == 0)
			_tcsncpy (title, TSTR::FromCP(codePage, &line[strlen (TITLE)]).data(), MAX_DSC_LINE_LENGTH + 1);
        else if (strncmp (line, CREATOR, strlen (CREATOR)) == 0)
			_tcsncpy (creator, TSTR::FromCP(codePage, &line[strlen (CREATOR)]).data(), MAX_DSC_LINE_LENGTH + 1);
        else if (strncmp (line, CREATION_DATE, strlen (CREATION_DATE)) == 0)
			_tcsncpy (creationDate, TSTR::FromCP(codePage, &line[strlen (CREATION_DATE)]).data(), MAX_DSC_LINE_LENGTH + 1);
    }
     
 cleanup:   
	if (mInputStream) {
		fclose (mInputStream);
		mOutStream = NULL;
    }
    return status;
}

// Position to the beginning of the PostScript code.
// If there is no EPS file header (i.e. there is a preview image) that
// is just the beginning of the file, else the header tells where to
// position to
int
BitmapIO_EPS::PositionToBeginOfPostScript ()
{
    struct EPSFileHeader  header;
    int status = 1;

	if (fread (&header, sizeof(struct EPSFileHeader), 1, mInputStream) != 1)
        return 0;

    if (header.magic[0] == 0xc5 &&
        header.magic[1] == 0xd0 &&
        header.magic[2] == 0xd3 &&
        header.magic[3] == 0xc6) {
        includesPreview = 1;
			if (fseek (mInputStream, header.psStart, SEEK_SET) == -1)
            status = 0;
    } else {
        includesPreview = 0;
		if (fseek (mInputStream, 0L, SEEK_SET) == -1)
            status = 0;
    }

    return status;
}
        

// We need to handle line termination with either CR, CR-LF, or LF
int
BitmapIO_EPS::PSReadLine (char *line)
{
    int numCh = 0;
    int done = 0;
    int status = 0;
    int ch, nextCh;

    // If a character pending now stick it in
    if (chPending) {
        line[numCh++] = pendingCh;
        chPending = 0;
    }

    // Since we need to handle all of the various line terminations
    // CR, LF-CR, or LF then we can't use fgets.  Instead we need to
    // fill in our buffer a character at a time
    while (! done) {

        if (numCh > MAX_DSC_LINE_LENGTH) {
            status = 0;
            done = 1;
        }

		ch = fgetc (mInputStream);
        if (ch == EOF) {
            status = (numCh > 0) ? 1 : 0;
            done = 1;
        }

        else if (ch == CR) {    // Check to see if a linefeed
			nextCh = fgetc(mInputStream);
            if (nextCh == EOF || nextCh == LF) {
                status = 1;
                done = 1;
            } else {
                chPending = 1;
                pendingCh = nextCh;
                status = 1;
                done = 1;
            }

        } else if (ch == LF) {
            status = 1;
            done = 1;

        } else
            line[numCh++] = ch;
    }

    line[numCh] = 0;

    return status;

#if 0   
	if (fgets (line, MAX_DSC_LINE_LENGTH, mInputStream) == NULL)
        return 0;
    else
        return 1;
#endif
}


// This function reads the configuration file and sets all the options
// for writing the EPS file.
void
BitmapIO_EPS::Configure (void)
{
    TCHAR line[MAX_LINE_LENGTH+1];
    FILE *fp;
    TCHAR *buffer;
    TCHAR filename[257];
    double value;
    int on, i;
    int xResolutionSet = 0;
    int yResolutionSet = 0;
    int widthSet = 0;
    int heightSet = 0;

    /* We look for the config file in the MAX plugin configuration directory
     * if we can not find the config file then we go with the defaults
     */
    _tcscpy (filename, TheManager->GetDir (APP_PLUGCFG_DIR));
    int len = static_cast<int>(_tclen (filename));	// SR DCAST64: Downcast to 2G limit.
    if (len) {
	if (_tcscmp (&filename[len-1], _T("\\")))
	    _tcscat (filename, _T("\\"));
    }
    _tcscat (filename, _T("EPS.CFG"));
	#pragma message("UNICODE WATCH: ccs=UTF-8 in the future - File: " __FILE__ )
    if ((fp = _tfopen (filename, _T("r"))) == NULL)
        return;

    /* Read a line at a time throwing away any lines that start with ";" */
    while (_fgetts (line, MAX_LINE_LENGTH, fp))
    {
        /* Change to upper case to make matching case insensitive */
        i = 0;
        while (line[i] != 0)
        {
            line[i] = toupper(line[i]);
            i++;
        }
        if (line[0] != _T(';'))
        {
            /* Remove any leading spaces */
            buffer = line;
            while (*buffer == _T(' '))
                buffer++;

            /* Find if matches one of the key words */
            if (! _tcsncmp (buffer, _T("UNITS"), _tcslen(_T("UNITS"))))
            {
                on = GetUnits (buffer);
                if (on != -1)
                    userSettings.units = on;
            }
            else if (! _tcsncmp (buffer, _T("XRESOLUTION"), _tcslen(_T("XRESOLUTION"))))
            {
                value = GetValue (buffer);
                if (value > 0)
                {
                    userSettings.xResolution = (float) value;
                    xResolutionSet = 1;
                }
            }
            else if  (! _tcsncmp (buffer, _T("YRESOLUTION"), _tcslen(_T("YRESOLUTION"))))
            {
                value = GetValue (buffer);
                if (value > 0)
                {
                    userSettings.yResolution = (float) value;
                    yResolutionSet = 1;
                }
            }
            else if  (! _tcsncmp (buffer, _T("WIDTH"), _tcslen(_T("WIDTH"))))
            {
                value = GetValue (buffer);
                if (value >= 0)
                {
                    userSettings.paperWidth = (float) value;
                    widthSet = 1;
                }
            }
            else if  (! _tcsncmp (buffer, _T("HEIGHT"), _tcslen(_T("HEIGHT"))))
            {
                value = GetValue (buffer);
                if (value >= 0)
                {
                    userSettings.paperHeight = (float) value;
                    heightSet = 1;
                }
            }
            else if  (! _tcsncmp (buffer, _T("DATAFORMAT"), _tcslen(_T("DATAFORMAT"))))
            {
                on = GetDataFormat (buffer);
                if (on != -1)
                    userSettings.binary = on;
            }
            else if  (! _tcsncmp (buffer, _T("PREVIEW"), _tcslen(_T("PREVIEW"))))
            {
                on = GetOn (buffer);
                if (on != -1)
                    userSettings.preview = on;
            }
            else if  (! _tcsncmp (buffer, _T("ORIENTATION"), _tcslen(_T("ORIENTATION"))))
            {
                on = GetOrientation (buffer);
                if (on != -1)
                    userSettings.orientation = on;
            }
            else if  (! _tcsncmp (buffer, _T("COLORTYPE"), _tcslen(_T("COLORTYPE"))))
            {
                on = GetColorType (buffer);
                if (on != -1)
                    userSettings.colorType = on;
            }
        }
    }

    // We always keep the info in inches
    if (userSettings.units == MM) {
	if (heightSet)
	    userSettings.paperHeight /= MM_PER_INCHES;
	if (widthSet)
	    userSettings.paperWidth /= MM_PER_INCHES;
	if (xResolutionSet)
	    userSettings.xResolution *= MM_PER_INCHES;
	if (yResolutionSet)
	    userSettings.yResolution *= MM_PER_INCHES;
    }

    fclose (fp);
}


/* This function returns the value that occurs after the equal sign in
 * the given string.  If there is no equal sign, then it will return -1
 * to signify no value
 */
double
BitmapIO_EPS::GetValue (const TCHAR *string)
{
    double value = -1.0;

    /* Search for '=' making sure not to go past end of string */
    while (*string && *string != _T('='))
        string++;

    if (*string) 
        _stscanf (LocaleFloat(++string), _T("%lf"), &value);

    return value;
}

/* This function returns 1 if the string that occurs after the equal sign is
 * "ON", 0 if it is "OFF", and -1 if it is neither
 */
int
BitmapIO_EPS::GetOn (const TCHAR *string)
{
    int value = -1;

    /* Search for '=' making sure not to go past end of string */
    while (*string && *string != _T('='))
        string++;

    if (_tcsstr (string, _T("ON")))
        value = 1;

    else if (_tcsstr (string, _T("OFF")))
        value = 0;

    return value;
}

/* This function returns -1 if the string that occurs after the equal sign is
 * neither "PORTRAIT" nor "LANDSCAPE" otherwise returns the appropriate value
 */
int
BitmapIO_EPS::GetOrientation (const TCHAR *string)
{
    int value = -1;

    /* Search for '=' making sure not to go past end of string */
    while (*string && *string != _T('='))
        string++;

    if (_tcsstr (string, _T("PORTRAIT")))
        value = PORTRAIT;

    else if (_tcsstr (string, _T("LANDSCAPE")))
        value = LANDSCAPE;

    return value;
}

/* This function returns -1 if the string that occurs after the equal sign is
 * neither "BINARY" nor "ASCII" otherwise returns the appropriate value
 */
int
BitmapIO_EPS::GetDataFormat (const TCHAR *string)
{
    int value = -1;

    /* Search for '=' making sure not to go past end of string */
    while (*string && *string != _T('='))
        string++;

    if (_tcsstr (string, _T("BINARY")))
        value = BINARY;

    else if (_tcsstr (string, _T("ASCII")))
        value = ASCII;

    return value;
}


/* This function returns -1 if the string that occurs after the equal sign is
 * neither "RGB" nor "GRAY" otherwise returns the appropriate value
 */
int
BitmapIO_EPS::GetColorType (const TCHAR *string)
{
    int value = -1;

    /* Search for '=' making sure not to go past end of string */
    while (*string && *string != _T('='))
        string++;

    if (_tcsstr (string, _T("RGB")))
        value = RGBIMAGE;

    else if (_tcsstr (string, _T("GRAY")))
        value = GRAYIMAGE;

    return value;
}

/* This function returns -1 if the string that occurs after the equal sign is
 * neither "INCHES" nor "MM" otherwise returns the appropriate value
 */
int
BitmapIO_EPS::GetUnits (const TCHAR *string)
{
    int value = -1;

    /* Search for '=' making sure not to go past end of string */
    while (*string && *string != _T('='))
        string++;

    if (_tcsstr (string, _T("INCHES")))
        value = INCHES;

    else if (_tcsstr (string, _T("MM")))
        value = MM;

    return value;
}

/* This function writes out the configuration file to the 3DS home directory.
 * It includes comments for helping someone who reads the file.
 */
void
BitmapIO_EPS::WriteConfigFile (void)
{
    FILE *fp;
    TCHAR filename[257];
    float factor;
    
    /* Write the data to the configuration file */
    _tcscpy (filename, TheManager->GetDir (APP_PLUGCFG_DIR));
    int len = static_cast<int>(_tclen (filename));	// SR DCAST64: Downcast to 2G limit.
    if (len) {
	if (_tcscmp (&filename[len-1], _T("\\")))
	    _tcscat (filename, _T("\\"));
    }
    _tcscat (filename, _T("EPS.CFG"));
	#pragma message("UNICODE WATCH: ccs=UTF-8 in the future - File: " __FILE__ )
    if ((fp = _tfopen(filename, _T("w"))) == NULL)
    {
#if 0
        char errBuffer[200];        
        int result;
        
        sprintf (errBuffer,
                 "[%s %s.|%s][%s]", S_OPEN_FOR_WRITING_ERROR_LINE1, path,
                 S_OPEN_FOR_WRITING_ERROR_LINE2, S_CONTINUE);
        gfx_alert (0, errBuffer, result);
#endif
        return;
    }

    // We always keep the info in inches, so compute proper value of factor
    factor = (userSettings.units == INCHES) ? 1.0F : MM_PER_INCHES;	

    /* For user's ease write in same order as appears in dialog box */
    _ftprintf (fp, _T("; Configuration file for writing EPS files\n\n"));
    
    _ftprintf (fp, _T("; Sets EPS units to INCHES or MM; default is INCHES.\n"));
    _ftprintf (fp, _T("UNITS = %s\n\n"), userSettings.units ? _T("MM") : _T("INCHES"));

    _ftprintf (fp,
     _T( "; Sets default orientation to PORTRAIT (height greater than width)\n"));
    _ftprintf (fp,
      _T(";     or LANDSCAPE (width longer than height); default is PORTRAIT.\n"));
    _ftprintf (fp, _T(";     Default is ORIENTATION = PORTRAIT\n"));
    _ftprintf (fp, _T("ORIENTATION = %s\n\n"),
             userSettings.orientation ? _T("LANDSCAPE") : _T("PORTRAIT"));

    _ftprintf (fp,
        _T("; Sets whether to write image data as binary or ASCII\n"));
    _ftprintf (fp,
        _T(";     ASCII data requires twice the disk space, but if the\n"));
    _ftprintf (fp,
        _T(";     communications channel to your PostScript printer\n"));
    _ftprintf (fp,
             _T(";     can't handle 8-bit data, use ASCII.\n"));
    _ftprintf (fp, _T(";     Default is DATAFORMAT = ASCII\n"));
    _ftprintf (fp, _T("DATAFORMAT = %s\n\n"), userSettings.binary ? _T("BINARY") : 
	     _T("ASCII"));

    _ftprintf (fp,
             _T("; Sets default file type to RGB (color) or GRAY (grayscale).\n"));
    _ftprintf (fp, _T(";     Default is COLORTYPE = RGB\n"));
    _ftprintf (fp, _T("COLORTYPE = %s\n\n"),
             userSettings.colorType ? _T("GRAY") : _T("RGB"));

    _ftprintf (fp,
    _T("; Sets whether to include preview (thumbnail) image in PostScript file.\n"));
    _ftprintf (fp,
         _T(";     Options are ON to include image or OFF not to include it.\n"));
    _ftprintf (fp, _T(";     The only format currently supported is TIFF\n"));
    _ftprintf (fp, _T(";     Default is PREVIEW = OFF\n"));
    _ftprintf (fp, _T("PREVIEW = %s\n\n"), userSettings.preview ? _T("ON") : _T("OFF"));

    _ftprintf (fp,
       _T("; Sets default page size on which the image is centered.\n"));
    _ftprintf (fp,
             _T(";     If UNITS = INCHES, page size is defined in inches.\n"));
    _ftprintf (fp,
            _T( ";     If UNITS = MM, page size is defined in millimeters.\n"));
    _ftprintf (fp, _T(";     Default is WIDTH = 8.5 and HEIGHT = 11.0.\n"));
    _ftprintf (fp, _T("WIDTH = %s\n"),
	     PeriodFloat (userSettings.paperWidth * factor, _T("%.3f")));
    _ftprintf (fp, _T("HEIGHT = %s\n\n"),
	     PeriodFloat (userSettings.paperHeight * factor, _T("%.3f")));

    _ftprintf (fp, _T("; Sets default image resolution.\n"));
    _ftprintf (fp,
       _T(";     If UNITS = INCHES, resolution is defined in dots per inch.\n"));
    _ftprintf (fp,
       _T(";     If UNITS = MM, resolution is defined in dots per millimeter.\n"));
    _ftprintf (fp,
             _T(";     Default is XRESOLUTION = 72.0 and YRESOLUTION = 72.0\n"));
    _ftprintf (fp, _T("XRESOLUTION = %s\n"),
	     PeriodFloat (userSettings.xResolution / factor, _T("%.5f")));
    _ftprintf (fp, _T("YRESOLUTION = %s\n"),
	     PeriodFloat (userSettings.yResolution / factor, _T("%.5f")));

    fclose (fp);
}

// Locale info
static TCHAR decimalPt[3] = _T(".");
static int localeDetermined = 0;

// Determine what is the decimal point
static void
GetNumericFormat ()
{
    TCHAR buf[3];
    int status;

    if (localeDetermined)
	return;

    localeDetermined = 1;

    status = GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, buf, 3);
    if (status)
	_tcsncpy( decimalPt, buf, 2);
}

// This function produces a string that has the locale setting for the 
// decimal point
TCHAR *
BitmapIO_EPS::LocaleFloat (const TCHAR *string)
{
    static TCHAR buffer[50];
    TCHAR *bp;
    const TCHAR *sp;

    GetNumericFormat ();

    sp = string;
    bp = buffer;
    while (*sp) {
	if(_tcsnicmp(sp, _T("."), 1))
	    _tcsncpy (bp, sp, 1);
	else 
	    _tcsncpy (bp, decimalPt, 1);
	sp = CharNext( sp );
	bp = CharNext( bp );
    }
    buffer[_tcslen(string)]  = _T('\0');

    return buffer;
}

// This function produces a string that always has a period as the
// decimal point
TCHAR *
BitmapIO_EPS::PeriodFloat (double num, const TCHAR *fmt)
{
    static TCHAR buffer[50];
    TCHAR string[50];
    TCHAR *bp;
    const TCHAR *sp;

    _stprintf (string, fmt, num);

    GetNumericFormat ();

    sp = string;
    bp = buffer;
    while (*sp) {
	if(_tcsnicmp(sp, decimalPt, 1))
	    _tcsncpy (bp, sp, 1);
	else 
	    _tcsncpy (bp, _T("."), 1);
	sp = CharNext( sp );
	bp = CharNext( bp );
    }
    buffer[_tcslen(string)]  = _T('\0');

    return buffer;
}

