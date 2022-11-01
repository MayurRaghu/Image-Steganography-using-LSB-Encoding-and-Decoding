#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
 //   printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
   // printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)													//declaring function open_files with arguments of structure
{
    // Src Image file

    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen1");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }
    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    printf("%s\n",encInfo->secret_fname);
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen2");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen3");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
/*
   -> Function check_operation_type with arguments from the command line
   -> First will check if the command line argument is passed 
   -> if the second argument i.e argv[1] is -e you'll return e_encode  
   -> you'll do the encoding part if -e is passed
   -> if the second argument i.e argv[1] is -d you'll return e_decode
   -> you'll do the decoding part if -d is passed
   -> if nothing is passed then return e_unsupported
 */
OperationType check_operation_type(char *argv[]) 										//declaring fucntion check_operation type with arguments from the command line
{
    if(strcmp(argv[1],"-e")==0)													//comparing the argv[1] or the 2nd argument value using strcmp functionwhich will be used if string.h header file
	return e_encode;
    else if(strcmp(argv[1],"-d")==0)
	return e_decode;
    else															//if nothing is passed then return e_unsupported
	return e_unsupported;
}
/*
   -> Function read_and validate_encode_args with arguments taken from the command line and from the structure of EncodeInfo
   For agrv[2]
   -> check if argv[2] or 3rd argument from the command line is passed or not
   -> if passed then take a short variable sh
   -> then open the file of src image with read mode. Then read the file of src image to the address of short variable sh.
   -> the way to determine whether to check if the given image file is a .bmp or any image file is to check whether the first 2 bytes will be a certain value, 
   -> for a bmp file the first 2 bytes will be 0x42 and 0x4D
   -> so sh will check if the first 2 bytes are equal to 0x42 and 0x4D. And comparision will be done in the reverse order i.e 0x4d42 because all the systems will follow the little endian system.
   -> after the comparision will be done then store the 3rd argument from CLA i.e argv[2] to the source image name(encInfo->src_image_fname).
   -> encInfo is used to call the struture variable where the struture pointer variables are stored
   -> if the condition fails i.e if the argument is not passed in the CLA then print an error saying that source file is not a bmp file and return e_failure 

   For argv[3]
   -> Check whether argv[3] or the 4th argument is passed or not
   -> If true then 2 steps should be followed
   1.Store the extention of the secret file where the data needs to be encoded inside the image byte by byte.
   .) Encoding of extention of the secret file is required for decoding the data and if the data is in 1 format and the decoding file will be in another one.
   .) For example - .txt while decoding .sh is used there might be chance where the data might be lost.
   .) For storing the extention to the structure variable strstr should be used in order to get the extention file. 
   .) For example if the secret file name is data.txt and only extention file needs to stored in a variable so strstr command.
   .) strstr is used like - strstr(data.txt, ".") where it takes from '.' upto end of the name so .txt will be present.
   .) In order to save that name in a variable strcpy is also used along with strstr so it will copy only the extention string in the variable.
   2.Store the complete name of the secret file to structure variable encInfo->stego_image_fname.
   ->If the argument is not passed then return e_failure along with a comment saying secret file not passed in argument.

   For argv[4]
   -> Check whether argv[4] or the 5th argument is passed or not
   -> If true then store the argument in struture variable encInfo->stego_image_fname
   -> If the argument is not passed then create a default name stego.bmp and store it in encInfo->stego_image_fname

   ->After all the condition are passed return e_success
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(argv[2] != NULL)
    {
	if(strstr(argv[2], ".") == NULL || strcmp(strstr(argv[2], "."), ".bmp") != 0)
	{
	    printf("Error: Source file is not a .bmp file\n");
	    return e_failure;
	}
	else
	{
	    unsigned short sh;

	    if(fopen(argv[2], "r")== NULL)
	    {
		printf("Error: File not present in the directory\n");
		return e_failure;
	    }

	    else
	    {
		encInfo->fptr_src_image = fopen(argv[2], "r");
		fread(&sh,2,1,encInfo->fptr_src_image);
		if(sh == 0x4D42)
		{
		    encInfo->src_image_fname = argv[2];
		}
		else
		{
		    printf("Error: Source file is not a .bmp file\n");
		    return e_failure;
		}
		fclose(encInfo->fptr_src_image);
	    }
	}
    }
    //printf("12345\n");
    if(argv[3] != NULL)
    {
	strcpy(encInfo->extn_secret_file,(strstr(argv[3],".")));
	encInfo->secret_fname = argv[3];
	//	printf("%s\n",encInfo->secret_fname);
    }
    else
    {
	printf("Error: Secret file is not passed\n");
	return e_failure;
    }
    if(argv[4] != NULL)
	encInfo->stego_image_fname = argv[4];
    else
    {
	printf("INFO: Output file not mentioned. Creating stego.bmp as default\n");
	encInfo->stego_image_fname = "stego.bmp";
    }
    return e_success;
}
/*
   -> Function do_encoding along with arguments of all structures of encoding is passed i.e EncodeInfo *encInfo.
   -> This is the function where all the other functions necessary for decoding is called and either returned success or failure.
   -> First function open_files is called and the argument passed is encInfo which is all the structure variable for encoding.
   -> In open_files go to line 34 for better understanding of what the function does.
   -> If the function open_files does not return e_success then it will print out failed to open files and returns failure.
   -> Next is check_capacity is called.
   -> All the functions called either return e_success or e_failure if success the image will be encoded successfully else it will result in e_failure.
 */
Status do_encoding(EncodeInfo *encInfo)
{
    printf("Info: Opening required files\n");
    if(open_files(encInfo) == e_success)
    {
	printf("INFO: Opened Skeleton code/%s\n",encInfo->src_image_fname);
	printf("INFO: Opened %s\n",encInfo->secret_fname);
	printf("INFO: Opened %s\n",encInfo->stego_image_fname);
	printf("Done\n");
	printf("INFO:## Encodiing procedure started ##\n");

    }
    else
    {
	printf("INFO: Opening of files: FAILED\n");
	return e_failure;
    }
    if(check_capacity(encInfo)==e_success)
    {
	printf("INFO: Checking capacity: DONE\n");
    }
    else
    {
	printf("INFO: Checking capacity: FAILURE\n");
	return e_failure;
    }

    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
    {
	printf("INFO: Copy BMP Header:DONE\n");
    }
    else
    {
	printf("INFO: Copy BMP Header:FAILURE\n");
	return e_failure;
    }
    if(encode_magic_string(MAGIC_STRING, encInfo)==e_success)
    {
	printf("INFO: Magic string encoding: DONE\n");
    }
    else
    {
	printf("INFO: Magic string encoding: FAILURE\n");
	return e_failure;
    }
    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo)==e_success)
    {
	printf("INFO: Encoding of extention : DONE\n");
    }
    else
    {
	printf("INFO: Encoding of extention : FAILURE\n");
	return e_failure;
    }
    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
    {
	printf("INFO: File size encoding: DONE\n");
    }
    else
    {
	printf("INFO: File size encoding: FAILURE\n");
	return e_failure;
    }
    return e_success;
}
/*
   -> Function get_file_size with argument file *fptr.
   -> It is mainly used to check the size of the secret file.
   -> For this fseek is used with the respective file pointer passed and pointing in 0th stating position till end position.
   -> Then ftell will be used to check the current postion which will determine the size of the secret file.
   -> Return that value stored in the variable.
 */
uint get_file_size(FILE *fptr)
{
    uint total;
    fseek(fptr, 0L, SEEK_END);
    total = ftell(fptr);
    rewind(fptr);
    return total;
}
/*
   -> Function check_capacity with argument EncodeInfo *encInfo.
   -> In this function the image capacity and size of the secret file size needs to be checked.
   -> These need to be checked to determine whether the secret file can be able to encode in the source image.
   -> The size of the secret file should be lesser /equal to the source image file size as it needs to fit into the image file.
   -> In order to determine this uint get_file_size is called for getting the size of the secret file and get_image_size_for_bmp is called for getting the size of the source image.
   -> After calling the respective functions the values are stored in size_secret_file for secret file size and image_capacity for size of source file.
   -> Then calculate the length with 54 bytes included as header file of any image contains 54 bytes of data which contain the relevent information of the image.
   -> Then at the end multiply all the data except 54 with 8 as 1 bit of data takes 8 bytes to encode it into the image.
   -> Then check whether it is less/equal to image.
   -> If true then pass e_success, else e_failure.
 */
Status check_capacity(EncodeInfo *encInfo)
{
    uint length;
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    // printf("%u\n",encInfo->image_capacity);
    //printf("SIZE SECRET FILE %lu\n",encInfo->size_secret_file);
    length = 54 + (strlen(MAGIC_STRING) + sizeof(int) + sizeof(encInfo->extn_secret_file) + sizeof(long) + encInfo->size_secret_file)*8;
    //printf("%u\n",length);
    if(length <= encInfo->image_capacity)
    {
	return e_success;
    }
    else
    {
	printf("Error: %s doesnt have the capacity to encode %s\n ",encInfo->src_image_fname, encInfo->secret_fname);
	return e_failure;
    }
}
/*
   -> Function copy_bmp_header along with arguments of source and destination images.
   -> This function is used to copy 54 bytes of data to the destination image from the source image.
   -> It is done by setting the source image at first position by using fseek.
   -> Then reading 54 bytes of data from source image and writing to destination image from function fread and fwrite respectively. 
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char str[54];
    fseek(fptr_src_image,0,SEEK_SET);
    fread(str,1,54,fptr_src_image);
    fwrite(str,1,54,fptr_dest_image);
    return e_success;
}
/*
   -> Function encode_magic_string along with arguments magic string and structures.
   -> To encode magic string to image encode_data_to_image function is called along with arguments magic_string size of magic string source and destination images.
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image((char *)magic_string, strlen(MAGIC_STRING), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}
/*
   -> Function encode_data_to_image along with arguments data, size, file pointers of source and destination.
   -> Here encoding of 1 bit of data to 1 byte is done.
   -> for loop is runned size number of times and the data is read from the source and written into the destination image.
   -> Here another function encode_byte_to_lsb is called for further encoding 1 bit of data into lsb of 1 byte of source image and storing it in destination image
 */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_dest_image)
{
    int i;
    char ch[8];
    //  printf("ftell %ld\n",ftell(fptr_dest_image));

    for(i=0;i<size;i++)
    {
	fread(ch,1,8,fptr_src_image);
	encode_byte_to_lsb(data[i], ch);
	fwrite(ch,1,8,fptr_dest_image);
    }
    return e_success;
}
/*
   -> Function encode_byte_to_lsb with arguments data and image buffer.
   -> Here loop is run 8 times.
   -> First step is clearing the lsb of image_buffer.
   -> Next is getting the MSB of data and bringing it to LSB. 
   -> Next is setting the data with the image_buffer taking each bit and storing it in LSB of each byte.
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int i, ind = 0;
    char ch;
    for(i=7;i>=0;i--)
    {
	image_buffer[ind] = image_buffer[ind] & 0xFE;
	ch = (data &(1<<i))>>i;
	image_buffer[ind] = image_buffer[ind] | ch;
	ind++;
    }
    // printf("\n");
}
/*
   -> Function encode_secret_file_extn with arguments file extention and structures.
   -> Multiple functions are called in this as written below.
   -> If each of the function called returns e_success then it will print the particular information as done.
   -> Else it will return e_failure.
 */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO: Checking for %s\n",encInfo->secret_fname);
    if(encode_secret_file_size(strlen(encInfo->extn_secret_file), encInfo)==e_success)
    {
	printf("INFO: Checking secret file extn size: DONE\n");
    }
    else 
    {
	printf("INFO: Checking secret file extn size: FAILURE\n");
	return e_failure;
    }
    if(encode_data_to_image((char *)file_extn, strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
	printf("INFO: Checking complete file extention: DONE\n");
    }
    else
    {
	printf("INFO: Checking complete file extention: FAILURE\n");
	return e_failure;
    }
    if(encode_secret_file_size(encInfo->size_secret_file, encInfo)==e_success)
    {
	printf("INFO: Checking secret file size: DONE\n");
    }
    else
    {
	printf("INFO: Checking secret file size: FAILURE\n");
	return e_failure;
    }
    if(encode_secret_file_data(encInfo)==e_success)
    {
	printf("Info: Encode secret file data: DONE\n");
    }
    else
    {
	printf("INFO: Checking secret file size: FAILURE\n");
	return e_failure;
    }
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
    {
	printf("INFO: Copy remaining data: DONE\n");
    }
    else
    {
	printf("INFO: Copy remaining data: FAILURE\n");
	return e_failure;
    }
    return e_success;
}
/*
   -> Function encode_secret_file_size along with arguments file size and structure.
   -> Secret file size is encoded by using long format.
   -> 64 bytes of data is read from the source image and written in the destination by using fread and fwrite respectively.
   -> encode_file_data_size is called with image buffer, size and struture passed for further encoding 1 bit of data in 1 byte of source image and storing it in dest image.
 */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    int var,i;
    //   printf("ftell: %ld\n",ftell(encInfo->fptr_stego_image));
    char ch[64];
    fread(ch,64,1,encInfo->fptr_src_image);
    encode_file_data_size(ch, encInfo, file_size);
    fwrite(ch,64,1,encInfo->fptr_stego_image);
    return e_success;
}
/*
   -> Function encode_file_data_size with arguments data and image buffer.
   -> Here loop is run 8 times.
   -> First step is clearing the lsb of image_buffer.
   -> Next is getting the MSB of data and bringing it to LSB. 
   -> Next is setting the data with the image_buffer taking each bit and storing it in LSB of each byte.
 */
Status encode_file_data_size(char *image_buffer, EncodeInfo *encInfo, long file_size)
{
    int i,ind=0;
    char ch;
    for(i=63;i>=0;i--)
    {
	image_buffer[ind] = image_buffer[ind] & 0xFE;
	ch = (file_size & (1<<i)) >>i;
	image_buffer[ind] = image_buffer[ind] | ch;
	ind++;
    }
    return e_success;
}
/*
   -> Function encode_file_data_size with argument struture variable.
   -> Setting the secret data to 0th postion for proper encoding of data to image.
   -> Reading the secret file tp secret_data with secret file size.
   -> calling function encode_data_to_image for further encoding of data to take the MSB and setting it with LSB of the source image and writing it destinatopn image
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //  printf("ftell for stego: %ld\n",ftell(encInfo->fptr_stego_image));
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    fread(encInfo->secret_data, sizeof(char),encInfo->size_secret_file,encInfo->fptr_secret);
    //printf("sizze :%ld\n", encInfo->size_secret_file);
    //printf("secret data :%s\n", encInfo->secret_data);
    //printf("ftell for secret : %ld\n",ftell(encInfo->fptr_secret));
    encode_data_to_image(encInfo->secret_data, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}
/*
   -> Function copy_remaining_img_data with arguments source and destination image.
   -> reading the data byte by byte from sourc and writing it in destination image until the source image encounters null.
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("INFO:Copying remaining data: DONE\n");
    char ch;
    while(fread(&ch, sizeof(char),1,fptr_src)>0)
    {
	fwrite(&ch, 1,1,fptr_dest);
    }
    return e_success;
}
