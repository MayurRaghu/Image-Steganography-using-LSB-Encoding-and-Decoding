#ifndef DECODE_H
#define DECODE_H
#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret file from source Image
 * Info about output and intermediate data is
 * also stored
 */
 #define MAX_SECRET_BUF_SIZE 1
  #define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
  #define MAX_FILE_SUFFIX 4
typedef struct _DecodeInfo
{
    /* default info*/
    char *default_fname;

    /*stego image info*/
    FILE *fptr_d_stego_image;
    char *d_stego_image_fname;

    /*extention info*/
    char d_file_extn[20];
    char *d_extn;
    long extn_size;

    /*secret file info*/
    FILE *fptr_d_secret_file;
    char secret_file_fname[15];
    long d_secret_file_size;
    char *d_secret_file_data;
} DecodeInfo;

 
/* Read and validate Decode args from argv*/
D_Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/*Perform decoding*/
D_Status do_decoding(DecodeInfo *decInfo);

/*Get file pointers from i/p and o/p string*/
D_Status open_files_decoding(DecodeInfo *decInfo);

/*compare if magic string is equal*/
D_Status decode_magic_string(const char *magic_string,int size, DecodeInfo *decInfo);

/*decode data from image*/
D_Status decode_data_from_image(char *data, int size, FILE *fptr_d_src_image);

/*decode byte to lsb*/
uint decode_byte_to_lsb(char *image_buffer);

/*decode secret file extn*/
D_Status decode_secret_file_extn(DecodeInfo *decInfo);

uint decode_secret_file_extn_size(DecodeInfo *decInfo);

/*Decode secret file data size*/
D_Status decode_secret_file_size(DecodeInfo *decInfo);

/*Decode secret file data*/
D_Status decode_secret_file_data(FILE *fptr_d_secret_file, long file_size, FILE *fptr_d_stego_image, DecodeInfo *decInfo);

//D_Status decode_lsb_for_data(char *image_buffer);
#endif
