#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <string.h>






DStatus read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char *ptr;

    ptr = strstr(argv[2], ".bmp");
    if(ptr != NULL && strcmp(ptr, ".bmp") == 0)
    {
        decInfo->stego_image_fname=argv[2];
    }
    else
    {
        printf("Error:Source image must be a .bmp file.\n");
        return d_failure;
    }

    if(argv[3]==NULL)
    {
        decInfo->output_fname="Output";
    }
    else
    {
        decInfo->output_fname = argv[3];
    }

    return d_success;
    
}

DStatus open_decode_files(DecodeInfo *decInfo)
{
    // Src Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    // Do Error handling
    if (decInfo->fptr_stego_image== NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

        return d_failure;
    }
    return d_success;
}

DStatus decode_magic_string(DecodeInfo *decInfo)
{
    char imageBuffer[8];
    char magic_string[strlen(MAGIC_STRING) + 1];
    for(int i=0;i<strlen(MAGIC_STRING);i++)
    {
        if(fread(imageBuffer,8,1,decInfo->fptr_stego_image)!= 1)
        {
            return d_failure;
        }
        magic_string[i]=decode_byte_from_lsb(imageBuffer);

    }

    magic_string[strlen(MAGIC_STRING)]= '\0';

    if(strcmp(magic_string,MAGIC_STRING)==0)
    {
        return d_success;
    }
    return d_failure;
}

DStatus decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char imageBuffer[32];
    if(fread(imageBuffer,32,1,decInfo->fptr_stego_image)!=1)
    {
        return d_failure;
    }

    decInfo->extn_size=decode_size_from_lsb(imageBuffer);

    return d_success;
}

DStatus decode_secret_file_extn(DecodeInfo *decInfo)
{
   char imageBuffer[8];
   char output_file[50];
    for(int i=0;i<decInfo->extn_size;i++)
    {
        if(fread(imageBuffer,8,1,decInfo->fptr_stego_image)!=1)
        {
            return d_failure;
        }

        decInfo->extn_secret_file[i]=decode_byte_from_lsb(imageBuffer);
    }
    decInfo->extn_secret_file[decInfo->extn_size]='\0';

    strcpy(output_file,decInfo->output_fname);
    strcat(output_file,decInfo->extn_secret_file);

    decInfo->fptr_output=fopen(output_file,"wb");

    if(decInfo->fptr_output==NULL)
    {
        printf("ERROR: Unable to create output file\n");
        return d_failure;
    }

    return d_success;

}

DStatus decode_secret_file_size(DecodeInfo *decInfo)
{
    char imageBuffer[32];
    if(fread(imageBuffer,32,1,decInfo->fptr_stego_image)!=1)
    {
        return d_failure;
    }

    decInfo->size_secret_file=decode_size_from_lsb(imageBuffer);

    return d_success;
}

DStatus decode_secret_file_data(DecodeInfo *decInfo)
{
    char imageBuffer[8];
    char ch;
    for(int i=0;i<decInfo->size_secret_file;i++)
    {
        if(fread(imageBuffer,8,1,decInfo->fptr_stego_image)!=1)
        {
            return d_failure;
        }

        ch=decode_byte_from_lsb(imageBuffer);

        if(fwrite(&ch,1,1,decInfo->fptr_output)!=1)
        {
            return d_failure;
        }  
    }
     
    return d_success;
}

char decode_byte_from_lsb(char *image_buffer)
{
    char data=0;
   for(int i=0;i<8;i++)
   {
       data=data<<1;
       data=data | (image_buffer[i] & 1);
   }
   return data;
}
int decode_size_from_lsb(char *imageBuffer)
{
    int size=0;
   for(int i=0;i<32;i++)
   {
       size=size<<1;
       size=size | (imageBuffer[i]  & 1);
       
   }
   return size; 
}

DStatus do_decoding(DecodeInfo *decInfo)
{
    if(open_decode_files(decInfo)==d_success)
    {
        printf("File Opened successfully\n");
    }
    else
    {
        printf("File Open Failed\n");
        return d_failure;
    }

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    if(decode_magic_string(decInfo)==d_success)
    {
        printf("Decode magic string successefully\n");
    }
    else
    {
        printf("Decoding Failed\n");
        return d_failure;
    }

    DStatus ret;

    ret = decode_secret_file_extn_size(decInfo);

    if (ret == d_success)
    {
    printf("Decode secret file extension size successfully\n");
   }
   else
   {
       printf("Decoding failed\n");
        return d_failure;
    }

    if(decode_secret_file_extn(decInfo)==d_success)
    {
        printf("Decode secret file extn Successfully\n");
    }
    else
    {
        printf("Decoding failed\n");
        return d_failure;
    }

    if(decode_secret_file_size(decInfo)== d_success)
    {
        printf("Decode secret file size Successfully\n");
    }
    else
    {
        printf("Decoding failed\n");
        return d_failure;
    }

    if(decode_secret_file_data(decInfo)==d_success)
    {
         printf("Decode secret file data Successfully\n");
    }
    else
    {
        printf("Decoding failed\n");
        return d_failure;
    }

    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);

    return d_success;
}