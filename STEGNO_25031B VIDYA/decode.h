#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _DecodeInfo
{

     /* Destination Image Info */
    char *dest_image_fname; // To store the dest file name
    FILE *fptr_dest_image;  // To store the address of destination image
    
    /*Output image info*/
    char output_fname[20];//To store the output file name
    FILE *fptr_output;//To store the address of output file
  
    long int extn_secret_file_size;
    char extn_secret_file[20]; // To store the Secret file extension
    char magic_string[20];    // Magic string data
    long int size_secret_file;    // To store the size of the secret data

} DecodeInfo;

/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Get File pointers for dest and o/p files */
Status do_open_files(DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* decode Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/*Decode secret file extension size*/
Status decode_secret_file_extn_size( DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Deode a byte into LSB of image data array */
void decode_byte_from_lsb(char* data, unsigned char *image_buffer);

// Encode a size to lsb
void decode_size_from_lsb(long int *size, unsigned char *imageBuffer);


#endif
