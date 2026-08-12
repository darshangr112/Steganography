#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>

#include "types.h"
#include "common.h"




typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;      // To store the stego image name
    FILE *fptr_stego_image;       // To store the address of the stego image

    /* Secret File Info */
    char *output_fname;           // Output file name
    FILE *fptr_output;            // Output file pointer
    char extn_secret_file[5];     // Secret file extension
    long size_secret_file;        // Size of the secret file
    int extn_size;                // Extension size

} DecodeInfo;


DStatus read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the Decoding */
DStatus do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
DStatus open_decode_files(DecodeInfo *decInfo);


/* Store Magic String */
DStatus decode_magic_string(DecodeInfo *decInfo);

/*Decode extension size*/
DStatus decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
DStatus decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
DStatus decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
DStatus decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
char decode_byte_from_lsb(char *image_buffer);

// Decode a size to lsb
int decode_size_from_lsb(char *imageBuffer);

#endif
