#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */
/*
   -> Function read_and_validate_decode_args with arguments from CLA and decInfo.
   -> Check whether argv[2] is passed or not.
   -> If passed check if its a bmp file or not.
   -> If true then store the argument name in stego_image.
   -> If not then return e_failure.
   -> check whether argv[3] is passed.
   -> if passed then store it in default_fname.
   -> else store default in default_fname. 

 */
D_Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(argv[2] != NULL)
    {
	if(strstr(argv[2], ".") == NULL || strcmp(strstr(argv[2],"."),".bmp") != 0)
	{
	    printf("Error: Source file is not a .bmp file\n");
	    return e_failure;
	}
	else
	{
	    unsigned short ch;
	    decInfo->fptr_d_stego_image = fopen(argv[2], "r");
	    fread(&ch,2,1,decInfo->fptr_d_stego_image);
	    if(ch == 0x4d42)
	    {
		decInfo->d_stego_image_fname = argv[2];
	    }
	    else
	    {
		printf("Error: Source file is not a .bmp file\n");
		return d_failure;
	    }
	}
	    fclose(decInfo->fptr_d_stego_image);
    }
    if(argv[3] != NULL)
    {
	decInfo->default_fname = argv[3];
    }
    else
	decInfo->default_fname = "default";
    return d_success;
}
/* 
   -> Function do_decoding with arguments struture.
   -> All the functions called either return d_success or d_failure if success the image will be decoded successfully else it will result in d_failure.
 */
D_Status do_decoding(DecodeInfo *decInfo)
{
    printf("##Decoding procedure started##\n");
    if(open_files_decoding(decInfo)==d_success)
    {
	printf("INFO: Opended %s: DONE\n",decInfo->d_stego_image_fname);
    }
    else
    {
	printf("INFO: Open files: FAILURE\n");
	return d_failure;
    }
    if(decode_magic_string(MAGIC_STRING,strlen(MAGIC_STRING), decInfo)==d_success)
    {
	printf("INFO: Decoding Magic string signature\n");
	printf("DONE\n");
    }
    else
    {
	printf("INFO: Decode magic string signature: Failure\n");
	return d_failure;
    }
    if(decode_secret_file_extn(decInfo)==d_success)
    {
	printf("INFO: Decoding file extn size\n");
	printf("DONE\n");
    }
    else
    {
	printf("INFO: Decode file extn size: Failure\n");
	return e_failure;
    }
    return d_success;
}
/*
   -> Function open_files_decoding with arguments *decInfo.
   -> open file d_stego_image_fname in read mode and store it in file pointer fptr_d_stego_image.
   -> check if it is null.
   -> if not null then return d_success.
 */
D_Status open_files_decoding(DecodeInfo *decInfo)
{
    decInfo->fptr_d_stego_image = fopen(decInfo->d_stego_image_fname, "r");
    if(decInfo->fptr_d_stego_image == NULL)
    {
	printf("fopen\n Error: Unable to open file %s\n ",decInfo->d_stego_image_fname);
	return d_failure;
    }
    return d_success;
}
/*
   -> Function decode magic string
   -> point the data after the header files from the stego image i.e 54 bytes
   -> call function decode_data_from_image for further decoding of 1 byte of data from image is decoded into 1 bit
   -> compare the magic string if true then return d_success else failure.
 */
D_Status decode_magic_string(const char *magic_string, int size, DecodeInfo *decInfo)
{
    char data[size];
    fseek(decInfo->fptr_d_stego_image,54,SEEK_SET);
    decode_data_from_image(data,strlen(magic_string),decInfo->fptr_d_stego_image);
    //printf("magic %s",MAGIC_STRING);
    printf("data: %s\n",data);
    if(strcmp(data,MAGIC_STRING)==0)
	printf("INFO:Compare magic string: Success\n");
    else
	printf("INFO: Compare magic string: Failure\n");
    return d_success;
}
/*
   -> Function decode_data_from_image
   -> read 8 bytes of data from stego image
   -> call function decode_byte_to_lsb for further decoding of 1 byte of data into 1 bit
 */
D_Status decode_data_from_image(char *data, int size, FILE *fptr_d_stego_image)
{
    char ch[8];
    data[size] ='\0';
    int i;
    for(i=0;i<size;i++)
    {
	fread(ch,1,8,fptr_d_stego_image);
	data[i]=decode_byte_to_lsb (ch);
	//	printf("data: %c\n",data[i]i);
    }

    return d_success;
}
/*
   -> Function decode_byte_to_lsb
   -> storing LSB of 1byte of data into MSB of 1 bit
 */
uint decode_byte_to_lsb(char *image_buffer)
{
    char sh = 0;
    int i;
    for(i=0;i<=7;i++)
    {
	if((image_buffer[i] & 1))
	    sh =sh | (1<<(7-i));
    }
    //printf("sh:%c\n",sh);
    return sh;
}
/*
   -> Function decode_secret_file_extn.
   -> Copy the exntention and concatenate it with default name
   -> All the functions called either return d_success or d_failure if success the image will be decoded successfully else it will result in d_failure.
 */
D_Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    int file_extn_size;
    file_extn_size = decode_secret_file_extn_size(decInfo);
    if(decode_data_from_image(decInfo->d_file_extn, file_extn_size, decInfo->fptr_d_stego_image)==d_success)
    {
	strcpy(decInfo->secret_file_fname, decInfo->default_fname);
	strcat(decInfo->secret_file_fname, decInfo->d_file_extn);
	decInfo->default_fname = decInfo->secret_file_fname;

	printf("Decoding extention: DONE\n");
	//printf("Extn: %s\n",decInfo->default_fname);
	//printf("Extn size: %d\n",file_extn_size);
	//printf("Extn: %s\n",decInfo->d_file_extn);
    }
    else
	return d_failure;
    if(decode_secret_file_size(decInfo)==d_success)
    {
	printf("Decode secret file size: DONE\n");
    }
    else
	return d_failure;
    if(decode_secret_file_data(decInfo->fptr_d_secret_file,decInfo->d_secret_file_size, decInfo->fptr_d_stego_image,decInfo) == d_success)
    {
	printf("Decoding secret file data: DONE\n");
	//	printf("File Data: %s\n",decInfo->fptr_d_secret_file);
    }
    else
    {
	printf("decoding secret file data: Failure\n");
	return d_failure;
    }
    return d_success;
}
/*
   -> Function decode_secret_file_extn_size
   -> reading the data from the stego image and decoding each byte into 1 bit of data form the stego image and store it in secret extenton file size.
 */
uint decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char ch[64];
    int i,ind=0; 
    fread(ch,1,64,decInfo->fptr_d_stego_image);
    for(i=63;i>=0;i--)
    {
	decInfo->extn_size = decInfo->extn_size | (ch[ind] & 1)<<i;
	ind++;
    }   

    printf("Decoding extention size: DONE\n");
    // printf("extn size: %ld\n",decInfo->extn_size);
    return decInfo->extn_size;
}
/*
   -> Function decode_secret_file_size
   -> reading the data from the stego image and decoding each byte into 1 bit of data form the stego image and store it in secret file size.
 */
D_Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char ch[64];
    int i, ind=0;
    long data_size = 0;
    //    printf("ftell: %ld\n",ftell(decInfo->fptr_d_stego_image));
    fread(ch,1,64,decInfo->fptr_d_stego_image);
    //  decInfo->d_secret_file_size = decode_secret_file_extn_size(decInfo);
    for(int i = 63; i>=0;i--)
    {
	decInfo->d_secret_file_size = decInfo->d_secret_file_size | (ch[ind] & 1)<<i;
	ind++;
    }
    //   printf("Secret file size : %ld\n",decInfo->d_secret_file_size);
    return d_success;
}
/*
   -> Function decode_secret_file_data with the arguments below.
   -> open the secret file in write mode to store the deocded value.
   -> reading the data from stego image and it in secret data.
   -> calling the fucntion decode_byte_to_lsb for further decoding.
   -> close the file.
 */
D_Status decode_secret_file_data(FILE *fptr_d_secret_file, long file_size, FILE *fptr_d_stego_image, DecodeInfo *decInfo)
{
    fptr_d_secret_file = fopen(decInfo->default_fname,"w");
    //  fseek(fptr_d_secret_file,0,SEEK_SET);
    char ch[8],data[file_size];
    //ch[8*file_size] ='\0'
    data[file_size] = '\0';
    int ind = 0,i,j;
    //printf("ftell stego: %ld\n",ftell(fptr_d_stego_image));
    //printf("ftell secret: %ld\n",ftell(fptr_d_secret_file));
    for(i=0;i<file_size;i++)
    {
	fread(ch,1,8,fptr_d_stego_image);
	data[i] = decode_byte_to_lsb(ch);
    }
    fwrite(data,1,file_size,fptr_d_secret_file);
    //printf("data: %s\n",data);
    fclose(fptr_d_secret_file);
    return d_success;
}
