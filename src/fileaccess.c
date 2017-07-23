/******************************************************************************
fileaccess.c
Copyright (C) 2016  {fullname}

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

Correspondence concerning RMSynth_GPU should be addressed to: 
sarrvesh.ss@gmail.com

******************************************************************************/
#include "fitsio.h"
#include "structures.h"
#include "constants.h"
#include "fileaccess.h"

#define IM_TYPE FLOAT_IMG
#define BUNIT   "JY/BEAM"
#define CTYPE3  "PHI"

/*************************************************************
*
* Check Fitsio error and exit if required.
*
*************************************************************/
void checkFitsError(int status) {
    if(status) {
        printf("ERROR:");
        fits_report_error(stdout, status);
        printf("\n");
        exit(FAILURE);
    }
}

/*************************************************************
*
* Read header information from the fits files
*
*************************************************************/
int getFitsHeader(struct optionsList *inOptions, struct parList *params) {
    int fitsStatus = SUCCESS;
    char fitsComment[FLEN_COMMENT];
    
    /* Get the image dimensions from the Q cube */
    fits_read_key(params->qFile, TINT, "NAXIS", &params->qAxisNum, 
      fitsComment, &fitsStatus);
    fits_read_key(params->qFile, TINT, "NAXIS1", &params->qAxisLen1, 
      fitsComment, &fitsStatus);
    fits_read_key(params->qFile, TINT, "NAXIS2", &params->qAxisLen2, 
      fitsComment, &fitsStatus);
    fits_read_key(params->qFile, TINT, "NAXIS3", &params->qAxisLen3, 
      fitsComment, &fitsStatus);
    /* Get the image dimensions from the Q cube */
    fits_read_key(params->uFile, TINT, "NAXIS", &params->uAxisNum, 
      fitsComment, &fitsStatus);
    fits_read_key(params->uFile, TINT, "NAXIS1", &params->uAxisLen1, 
      fitsComment, &fitsStatus);
    fits_read_key(params->uFile, TINT, "NAXIS2", &params->uAxisLen2, 
      fitsComment, &fitsStatus);
    fits_read_key(params->uFile, TINT, "NAXIS3", &params->uAxisLen3,
      fitsComment, &fitsStatus);
    /* Get WCS information */
    fits_read_key(params->qFile, TFLOAT, "CRVAL1", &params->crval1,
      fitsComment, &fitsStatus);
    fits_read_key(params->qFile, TFLOAT, "CRVAL2", &params->crval2,
      fitsComment, &fitsStatus);
    fits_read_key(params->qFile, TFLOAT, "CRPIX1", &params->crpix1, 
      fitsComment, &fitsStatus);
    fits_read_key(params->qFile, TFLOAT, "CRPIX2", &params->crpix2,
      fitsComment, &fitsStatus);
    fits_read_key(params->qFile, TFLOAT, "CDELT1", &params->cdelt1,
      fitsComment, &fitsStatus);
    fits_read_key(params->qFile, TFLOAT, "CDELT2", &params->cdelt2,
      fitsComment, &fitsStatus);
    fits_read_key(params->qFile, TSTRING, "CTYPE1", &params->ctype1,
      fitsComment, &fitsStatus);
    fits_read_key(params->qFile, TSTRING, "CTYPE2", &params->ctype2,
      fitsComment, &fitsStatus);
    
    return(fitsStatus);
}

/*************************************************************
*
* Create output images
*
*************************************************************/
void makeOutputImages(struct optionsList *inOptions, struct parList *params) {
   int stat = SUCCESS;
   char filenamefull[FILENAME_LEN];
   long naxis[FITS_OUT_NAXIS];
   char fComment[FILENAME_LEN];
   float tempVar;

   /* Create the output Q, U, and P images */
   sprintf(filenamefull, "%s%s", inOptions->outPrefix, Q_DIRTY);
   fits_create_file(&params->qDirty, filenamefull, &stat);
   sprintf(filenamefull, "%s%s", inOptions->outPrefix, U_DIRTY);
   fits_create_file(&params->uDirty, filenamefull, &stat);
   sprintf(filenamefull, "%s%s", inOptions->outPrefix, P_DIRTY);
   fits_create_file(&params->pDirty, filenamefull, &stat);
   checkFitsError(stat);
   
   /* What are the output cube sizes */
   naxis[0] = inOptions->nPhi;
   naxis[1] = params->qAxisLen1;
   naxis[2] = params->qAxisLen2;
   
   /* Create the header for each output image */
   fits_create_img(params->qDirty, IM_TYPE, FITS_OUT_NAXIS, naxis, &stat);
   fits_create_img(params->uDirty, IM_TYPE, FITS_OUT_NAXIS, naxis, &stat);
   fits_create_img(params->pDirty, IM_TYPE, FITS_OUT_NAXIS, naxis, &stat);
   checkFitsError(stat);
   
   /* Set the relevant keyvalues */
   fits_write_key(params->qDirty, TSTRING, "BUNIT", BUNIT, fComment, &stat);
   fits_write_key(params->uDirty, TSTRING, "BUNIT", BUNIT, fComment, &stat);
   fits_write_key(params->pDirty, TSTRING, "BUNIT", BUNIT, fComment, &stat);
   
   fits_write_key(params->qDirty, TFLOAT, "CRVAL1", &params->crval1, fComment, &stat);
   fits_write_key(params->uDirty, TFLOAT, "CRVAL1", &params->crval1, fComment, &stat);
   fits_write_key(params->pDirty, TFLOAT, "CRVAL1", &params->crval1, fComment, &stat);
   
   fits_write_key(params->qDirty, TFLOAT, "CDELT1", &params->cdelt1, fComment, &stat);
   fits_write_key(params->uDirty, TFLOAT, "CDELT1", &params->cdelt1, fComment, &stat);
   fits_write_key(params->pDirty, TFLOAT, "CDELT1", &params->cdelt1, fComment, &stat);
   
   fits_write_key(params->qDirty, TFLOAT, "CRPIX1", &params->crpix1, fComment, &stat);
   fits_write_key(params->uDirty, TFLOAT, "CRPIX1", &params->crpix1, fComment, &stat);
   fits_write_key(params->pDirty, TFLOAT, "CRPIX1", &params->crpix1, fComment, &stat);
   
   fits_write_key(params->qDirty, TSTRING, "CTYPE1", params->ctype1, fComment, &stat);
   fits_write_key(params->uDirty, TSTRING, "CTYPE1", params->ctype1, fComment, &stat);
   fits_write_key(params->pDirty, TSTRING, "CTYPE1", params->ctype1, fComment, &stat);
   
   fits_write_key(params->qDirty, TFLOAT, "CRVAL2", &params->crval2, fComment, &stat);
   fits_write_key(params->uDirty, TFLOAT, "CRVAL2", &params->crval2, fComment, &stat);
   fits_write_key(params->pDirty, TFLOAT, "CRVAL2", &params->crval2, fComment, &stat);
   
   fits_write_key(params->qDirty, TFLOAT, "CDELT2", &params->cdelt2, fComment, &stat);
   fits_write_key(params->uDirty, TFLOAT, "CDELT2", &params->cdelt2, fComment, &stat);
   fits_write_key(params->pDirty, TFLOAT, "CDELT2", &params->cdelt2, fComment, &stat);
   
   fits_write_key(params->qDirty, TFLOAT, "CRPIX2", &params->crpix2, fComment, &stat);
   fits_write_key(params->uDirty, TFLOAT, "CRPIX2", &params->crpix2, fComment, &stat);
   fits_write_key(params->pDirty, TFLOAT, "CRPIX2", &params->crpix2, fComment, &stat);
   
   fits_write_key(params->qDirty, TSTRING, "CTYPE2", params->ctype2, fComment, &stat);
   fits_write_key(params->uDirty, TSTRING, "CTYPE2", params->ctype2, fComment, &stat);
   fits_write_key(params->pDirty, TSTRING, "CTYPE2", params->ctype2, fComment, &stat);
   
   fits_write_key(params->qDirty, TFLOAT, "CRVAL3", &inOptions->phiMin, 
                  fComment, &stat);
   fits_write_key(params->uDirty, TFLOAT, "CRVAL3", &inOptions->phiMin, 
                  fComment, &stat);
   fits_write_key(params->pDirty, TFLOAT, "CRVAL3", &inOptions->phiMin, 
                  fComment, &stat);
   
   fits_write_key(params->qDirty, TFLOAT, "CDELT3", &inOptions->dPhi, fComment, &stat);
   fits_write_key(params->uDirty, TFLOAT, "CDELT3", &inOptions->dPhi, fComment, &stat);
   fits_write_key(params->pDirty, TFLOAT, "CDELT3", &inOptions->dPhi, fComment, &stat);
   
   tempVar = 1;
   fits_write_key(params->qDirty, TFLOAT, "CRPIX3", &tempVar, fComment, &stat);
   fits_write_key(params->uDirty, TFLOAT, "CRPIX3", &tempVar, fComment, &stat);
   fits_write_key(params->pDirty, TFLOAT, "CRPIX3", &tempVar, fComment, &stat);
   
   fits_write_key(params->qDirty, TSTRING, "CTYPE3", CTYPE3, fComment, &stat);
   fits_write_key(params->uDirty, TSTRING, "CTYPE3", CTYPE3, fComment, &stat);
   fits_write_key(params->pDirty, TSTRING, "CTYPE3", CTYPE3, fComment, &stat);
   
   checkFitsError(stat);

}

/*************************************************************
*
* Read the list of frequencies from the input freq file
*
*************************************************************/
int getFreqList(struct optionsList *inOptions, struct parList *params) {
    int i;
    float tempFloat;
    
    params->freqList = calloc(params->qAxisLen1, sizeof(params->freqList));
    if(params->freqList == NULL) {
        printf("Error: Mem alloc failed while reading in frequency list\n\n");
        return(FAILURE);
    }
    for(i=0; i<params->qAxisLen1; i++) {
        fscanf(params->freq, "%f", &params->freqList[i]);
        if(feof(params->freq)) {
            printf("Error: Frequency values and fits frames don't match\n");
            return(FAILURE);
        }
    }
    fscanf(params->freq, "%f", &tempFloat);
    if(! feof(params->freq)) {
        printf("Error: More frequency values present than fits frames\n\n");
        return(FAILURE);
    }
    
    /* Compute \lambda^2 from the list of generated frequencies */
    params->lambda2  = calloc(params->qAxisLen1, sizeof(params->lambda2));
    if(params->lambda2 == NULL) {
        printf("Error: Mem alloc failed while reading in frequency list\n\n");
        return(FAILURE);
    }
    params->lambda20 = 0.0;
    for(i=0; i<params->qAxisLen1; i++)
        params->lambda2[i] = (LIGHTSPEED / params->freqList[i]) * 
                             (LIGHTSPEED / params->freqList[i]);
    
    return(SUCCESS);
}

/*************************************************************
*
* Read in the input mask image
*
*************************************************************/
int getImageMask(struct optionsList *inOptions, struct parList *params) {
    int fitsStatus = SUCCESS;
    char fitsComment[FLEN_COMMENT];

    fits_read_key(params->maskFile, TINT, "NAXIS1", &params->maskAxisLen1, 
                  fitsComment, &fitsStatus);
    fits_read_key(params->maskFile, TINT, "NAXIS2", &params->maskAxisLen2, 
                  fitsComment, &fitsStatus);
    return(SUCCESS);
}
