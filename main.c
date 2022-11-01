#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;													//defining struture for encoding and decoding
    DecodeInfo decInfo;

    if(argc<3)
    {
	printf("Error: lsb_steg: Encoding -e <bmp file> <txt file> [output file]\n");
	printf("Error: lsb_steg: Decoding -d <bmp file> [output file]\n");
	return 1;
    }
    if(check_operation_type(argv)==e_encode)										//checking for encoding if all return e_success then it will encode 
    {
	if(read_and_validate_encode_args(argv, &encInfo)==e_success)
	{   
	    if(do_encoding(&encInfo)==e_success)
	    {
		printf("INFO: ##Encoding done successfully##\n");
	    }
	    else
	    {
		printf("INFO: ##Encoding Failed##\n");									//if returned e_failure it will not encode
	    }
	    fclose(encInfo.fptr_src_image);
	    fclose(encInfo.fptr_secret);
	    fclose(encInfo.fptr_stego_image);
	}
    }
    else if(check_operation_type(argv)==e_decode)									//same procedure for decoding
    {
	if(read_and_validate_decode_args(argv, &decInfo) == d_success)
	{
	    if(do_decoding(&decInfo)==d_success)
	    {
		printf("INFO: ##Decoding done successfully##\n");							//if returned d_failure it will not decode
	    }
	    else
	    {
		printf("INFO: ##Decoding failed\n");

	    }
	}
    }
    else														//else it will show error
    {
	printf("Error: Invalid Input\n");
	printf("./lsb.steg: Encoding: ./lsb.steg -e <bmp file> <.txt file> [Output file]\n");
	printf("./lsb.steg: Decoding: ./lsb.steg -d <bmp file> [Output file]\n");
	return e_failure;
    }
}
