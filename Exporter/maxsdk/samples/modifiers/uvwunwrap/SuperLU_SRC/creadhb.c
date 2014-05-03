
/*! @file creadhb.c
 * \brief Read a matrix stored in Harwell-Boeing format
 *
 * <pre>
 * -- SuperLU routine (version 2.0) --
 * Univ. of California Berkeley, Xerox Palo Alto Research Center,
 * and Lawrence Berkeley National Lab.
 * November 15, 1997
 *
 * Purpose
 * =======
 * 
 * Read a COMPLEX PRECISION matrix stored in Harwell-Boeing format 
 * as described below.
 * 
 * Line 1 (A72,A8) 
 *  	Col. 1 - 72   Title (TITLE) 
 *	Col. 73 - 80  Key (KEY) 
 * 
 * Line 2 (5I14) 
 * 	Col. 1 - 14   Total number of lines excluding header (TOTCRD) 
 * 	Col. 15 - 28  Number of lines for pointers (PTRCRD) 
 * 	Col. 29 - 42  Number of lines for row (or variable) indices (INDCRD) 
 * 	Col. 43 - 56  Number of lines for numerical values (VALCRD) 
 *	Col. 57 - 70  Number of lines for right-hand sides (RHSCRD) 
 *                    (including starting guesses and solution vectors 
 *		       if present) 
 *           	      (zero indicates no right-hand side data is present) 
 *
 * Line 3 (A3, 11X, 4I14) 
 *   	Col. 1 - 3    Matrix type (see below) (MXTYPE) 
 * 	Col. 15 - 28  Number of rows (or variables) (NROW) 
 * 	Col. 29 - 42  Number of columns (or elements) (NCOL) 
 *	Col. 43 - 56  Number of row (or variable) indices (NNZERO) 
 *	              (equal to number of entries for assembled matrices) 
 * 	Col. 57 - 70  Number of elemental matrix entries (NELTVL) 
 *	              (zero in the case of assembled matrices) 
 * Line 4 (2A16, 2A20) 
 * 	Col. 1 - 16   Format for pointers (PTRFMT) 
 *	Col. 17 - 32  Format for row (or variable) indices (INDFMT) 
 *	Col. 33 - 52  Format for numerical values of coefficient matrix (VALFMT) 
 * 	Col. 53 - 72 Format for numerical values of right-hand sides (RHSFMT) 
 *
 * Line 5 (A3, 11X, 2I14) Only present if there are right-hand sides present 
 *    	Col. 1 	      Right-hand side type: 
 *	         	  F for full storage or M for same format as matrix 
 *    	Col. 2        G if a starting vector(s) (Guess) is supplied. (RHSTYP) 
 *    	Col. 3        X if an exact solution vector(s) is supplied. 
 *	Col. 15 - 28  Number of right-hand sides (NRHS) 
 *	Col. 29 - 42  Number of row indices (NRHSIX) 
 *          	      (ignored in case of unassembled matrices) 
 *
 * The three character type field on line 3 describes the matrix type. 
 * The following table lists the permitted values for each of the three 
 * characters. As an example of the type field, RSA denotes that the matrix 
 * is real, symmetric, and assembled. 
 *
 * First Character: 
 *	R Real matrix 
 *	C Complex matrix 
 *	P Pattern only (no numerical values supplied) 
 *
 * Second Character: 
 *	S Symmetric 
 *	U Unsymmetric 
 *	H Hermitian 
 *	Z Skew symmetric 
 *	R Rectangular 
 *
 * Third Character: 
 *	A Assembled 
 *	E Elemental matrices (unassembled) 
 *
 * </pre>
 */
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "slu_cdefs.h"


/*! \brief Eat up the rest of the current line */
int cDumpLine(FILE *fp)
{
    register int c;
    while ((c = _fgettc(fp)) != '\n') ;
    return 0;
}

int cParseIntFormat(TCHAR *buf, int *num, int *size)
{
    TCHAR *tmp;

    tmp = buf;
    while (*tmp++ != '(') ;
    _stscanf(tmp, _T("%d"), num);
    while (*tmp != 'I' && *tmp != 'i') ++tmp;
    ++tmp;
    _stscanf(tmp, _T("%d"), size);
    return 0;
}

int cParseFloatFormat(TCHAR *buf, int *num, int *size)
{
    TCHAR *tmp, *period;
    
    tmp = buf;
    while (*tmp++ != '(') ;
    *num = _ttoi(tmp); /*_stscanf(tmp, "%d", num);*/
    while (*tmp != 'E' && *tmp != 'e' && *tmp != 'D' && *tmp != 'd'
	   && *tmp != 'F' && *tmp != 'f') {
        /* May find kP before nE/nD/nF, like (1P6F13.6). In this case the
           num picked up refers to P, which should be skipped. */
        if (*tmp=='p' || *tmp=='P') {
           ++tmp;
           *num = _ttoi(tmp); /*_stscanf(tmp, "%d", num);*/
        } else {
           ++tmp;
        }
    }
    ++tmp;
    period = tmp;
    while (*period != '.' && *period != ')') ++period ;
    *period = '\0';
    *size = _ttoi(tmp); /*_stscanf(tmp, "%2d", size);*/

    return 0;
}

int cReadVector(FILE *fp, int n, int *where, int perline, int persize)
{
    register int i, j, item;
    TCHAR tmp, buf[100];
    
    i = 0;
    while (i < n) {
	_fgetts(buf, 100, fp);    /* read a line at a time */
	for (j=0; j<perline && i<n; j++) {
	    tmp = buf[(j+1)*persize];     /* save the char at that place */
	    buf[(j+1)*persize] = 0;       /* null terminate */
	    item = _ttoi(&buf[j*persize]); 
	    buf[(j+1)*persize] = tmp;     /* recover the char at that place */
	    where[i++] = item - 1;
	}
    }

    return 0;
}

/*! \brief Read complex numbers as pairs of (real, imaginary) */
int cReadValues(FILE *fp, int n, complex *destination, int perline, int persize)
{
    register int i, j, k, s, pair;
    register float realpart;
    TCHAR tmp, buf[100];
	realpart = 0;
    
    i = pair = 0;
    while (i < n) {
	_fgetts(buf, 100, fp);    /* read a line at a time */
	for (j=0; j<perline && i<n; j++) {
	    tmp = buf[(j+1)*persize];     /* save the char at that place */
	    buf[(j+1)*persize] = 0;       /* null terminate */
	    s = j*persize;
	    for (k = 0; k < persize; ++k) /* No D_ format in C */
		if ( buf[s+k] == 'D' || buf[s+k] == 'd' ) buf[s+k] = 'E';
	    if ( pair == 0 ) {
	  	/* The value is real part */
		realpart = _tstof(&buf[s]);
		pair = 1;
	    } else {
		/* The value is imaginary part */
	        destination[i].r = realpart;
		destination[i++].i = _tstof(&buf[s]);
		pair = 0;
	    }
	    buf[(j+1)*persize] = tmp;     /* recover the char at that place */
	}
    }

    return 0;
}


void
creadhb(int *nrow, int *ncol, int *nonz,
	complex **nzval, int **rowind, int **colptr)
{

    register int i, numer_lines = 0, rhscrd = 0;
    int tmp, colnum, colsize, rownum, rowsize, valnum, valsize;
    TCHAR buf[100], type[4];
    FILE *fp;

    fp = stdin;

    /* Line 1 */
    _fgetts(buf, 100, fp);
    _fputts(buf, stdout);
#if 0
    _ftscanf(fp, "%72c", buf); buf[72] = 0;
    _tprintf("Title: %s", buf);
    _ftscanf(fp, "%8c", key);  key[8] = 0;
    _tprintf("Key: %s\n", key);
    cDumpLine(fp);
#endif

    /* Line 2 */
    for (i=0; i<5; i++) {
	_ftscanf(fp, _T("%14c"), buf); buf[14] = 0;
	_stscanf(buf, _T("%d"), &tmp);
	if (i == 3) numer_lines = tmp;
	if (i == 4 && tmp) rhscrd = tmp;
    }
    cDumpLine(fp);

    /* Line 3 */
    _ftscanf(fp, _T("%3c"), type);
    _ftscanf(fp, _T("%11c"), buf); /* pad */
    type[3] = 0;
#ifdef DEBUG
    _tprintf("Matrix type %s\n", type);
#endif
    
    _ftscanf(fp, _T("%14c"), buf); _stscanf(buf, _T("%d"), nrow);
    _ftscanf(fp, _T("%14c"), buf); _stscanf(buf, _T("%d"), ncol);
    _ftscanf(fp, _T("%14c"), buf); _stscanf(buf, _T("%d"), nonz);
    _ftscanf(fp, _T("%14c"), buf); _stscanf(buf, _T("%d"), &tmp);
    
    if (tmp != 0)
	  _tprintf(_T("This is not an assembled matrix!\n"));
    if (*nrow != *ncol)
	_tprintf(_T("Matrix is not square.\n"));
    cDumpLine(fp);

    /* Allocate storage for the three arrays ( nzval, rowind, colptr ) */
    callocateA(*ncol, *nonz, nzval, rowind, colptr);

    /* Line 4: format statement */
    _ftscanf(fp, _T("%16c"), buf);
    cParseIntFormat(buf, &colnum, &colsize);
    _ftscanf(fp, _T("%16c"), buf);
    cParseIntFormat(buf, &rownum, &rowsize);
    _ftscanf(fp, _T("%20c"), buf);
    cParseFloatFormat(buf, &valnum, &valsize);
    _ftscanf(fp, _T("%20c"), buf);
    cDumpLine(fp);

    /* Line 5: right-hand side */    
    if ( rhscrd ) cDumpLine(fp); /* skip RHSFMT */
    
#ifdef DEBUG
    _tprintf(_T("%d rows, %d nonzeros\n"), *nrow, *nonz);
    _tprintf(_T("colnum %d, colsize %d\n"), colnum, colsize);
    _tprintf(_T("rownum %d, rowsize %d\n"), rownum, rowsize);
    _tprintf(_T("valnum %d, valsize %d\n"), valnum, valsize);
#endif
    
    cReadVector(fp, *ncol+1, *colptr, colnum, colsize);
    cReadVector(fp, *nonz, *rowind, rownum, rowsize);
    if ( numer_lines ) {
        cReadValues(fp, *nonz, *nzval, valnum, valsize);
    }
    
    fclose(fp);

}

