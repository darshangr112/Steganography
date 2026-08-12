#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>

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
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    uint size = ftell(fptr);
    rewind(fptr);

    return size;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char *ptr;

    ptr = strstr(argv[2], ".bmp");
    if(ptr != NULL && strcmp(ptr, ".bmp") == 0)
    {
        encInfo->src_image_fname=argv[2];
    }
    else
    {
        printf("Error:Source image must be a .bmp file.\n");
        return e_failure;
    }

    ptr = strstr(argv[3], ".txt");
    if(ptr != NULL && strcmp(ptr, ".txt") == 0)
    {
        encInfo->secret_fname=argv[3];
    }
    else
    {
        printf("ERROR: Secret file must be a .txt file.\n");
        return e_failure;
    }

    if(argv[4]==NULL)
    {
        encInfo->dest_image_fname="default.bmp";
    }
    else
    {
        ptr = strstr(argv[4], ".bmp");
        if(ptr != NULL && strcmp(ptr, ".bmp") == 0)
        {
            encInfo->dest_image_fname=argv[4];
        }
        else
        {
            printf("ERROR: Output file extension must be a .bmp file.\n");
            return e_failure;
        }
    }

    return e_success;
    
}

Status open_encode_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_dest_image = fopen(encInfo->dest_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_dest_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->dest_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);

    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);

    int required_size=(54+16+32+32+32+encInfo->size_secret_file*8);

    if(encInfo->image_capacity>=required_size)
    {
        return e_success;
    }
    return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);
    char imageBuffer[54];

    fread(imageBuffer,54,1,fptr_src_image);

    fwrite(imageBuffer,54,1,fptr_dest_image);

    if(ftell(fptr_src_image)==54 && ftell(fptr_dest_image)==54)
    {
        return e_success;
    }
    return e_failure;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char imageBuffer[8];
    for(int i=0;i<strlen(magic_string);i++)
    {
        fread(imageBuffer,8,1,encInfo->fptr_src_image);

        encode_byte_to_lsb(magic_string[i],imageBuffer);

        fwrite(imageBuffer,8,1,encInfo->fptr_dest_image);  
    }

    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image))
    {
        return e_success;
    }
    return e_failure;


    
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char imageBuffer[32];
    fread(imageBuffer,32,1,encInfo->fptr_src_image);

    encode_size_to_lsb(size,imageBuffer);

    fwrite(imageBuffer,32,1,encInfo->fptr_dest_image);

    long src = ftell(encInfo->fptr_src_image);
    long dest = ftell(encInfo->fptr_dest_image);

    if (src == dest)
    {
      return e_success;
   }
   else
   {
    printf("SRC offset = %ld\n", src);
    printf("DEST offset = %ld\n", dest);
    return e_failure;
   }
  
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
   char imageBuffer[8];
    for(int i=0;i<strlen(file_extn);i++)
    {
        fread(imageBuffer,8,1,encInfo->fptr_src_image);

        encode_byte_to_lsb(file_extn[i],imageBuffer);

        fwrite(imageBuffer,8,1,encInfo->fptr_dest_image);  
    }

    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image))
    {
        return e_success;
    }
    return e_failure;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char imageBuffer[32];
    fread(imageBuffer,32,1,encInfo->fptr_src_image);

    encode_size_to_lsb(file_size,imageBuffer);

    fwrite(imageBuffer,32,1,encInfo->fptr_dest_image);

    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image))
    {
        return e_success;
    }
    return e_failure;  
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);
    char secret_file_data[encInfo->size_secret_file];

    fread(secret_file_data,1,encInfo->size_secret_file,encInfo->fptr_secret);
    char imageBuffer[8];
    for(int i=0;i<encInfo->size_secret_file;i++)
    {
        fread(imageBuffer,8,1,encInfo->fptr_src_image);

        encode_byte_to_lsb(secret_file_data[i],imageBuffer);

        fwrite(imageBuffer,8,1,encInfo->fptr_dest_image);  
    }

    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image))
    {
        return e_success;
    }
    return e_failure;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
   for(int i=0;i<8;i++)
   {
       int bit=(data>>(7-i)&1);

       image_buffer[i] &=0xFE;

       image_buffer[i] |= bit;
   }
   return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    for(int i=0;i<32;i++)
   {
       int bit=(size>>(31-i)&1);

       imageBuffer[i] &=0xFE;

       imageBuffer[i] |= bit;
   }
   return e_success;
  
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    int ch;

    while ((ch = fgetc(fptr_src)) != EOF)
    {
        fputc(ch, fptr_dest);
    }

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_encode_files(encInfo)==e_success)
    {
        printf("File Opened successfully\n");
    }
    else
    {
        printf("File Open Failed\n");
        return e_failure;
    }

    if(check_capacity(encInfo)==e_success)
    {
        printf("Check capacity successfully\n");
    }
    else
    {
        printf("Check capacity Failed\n");
        return e_failure;
    }

    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_dest_image)==e_success)
    {
        printf("Copy bmp header successfully\n");
    }
    else
    {
        printf("Copy bmp header Failed\n");
        return e_failure;
    }

    if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
    {
        printf("Encode magic string successefully\n");
    }
    else
    {
        printf("Encoding Failed\n");
        return e_failure;
    }

    char *secret_file_extn = strrchr(encInfo->secret_fname, '.');

    if(secret_file_extn == NULL)
    {
        printf("ERROR: Secret file extension not found\n");
        return e_failure;
    }

    int extn_size = strlen(secret_file_extn);

    Status ret;

    ret = encode_secret_file_extn_size(extn_size, encInfo);

    if (ret == e_success)
    {
    printf("Encode secret file extension size successfully\n");
   }
   else
   {
       printf("Encoding failed\n");
        return e_failure;
    }

    if(encode_secret_file_extn(secret_file_extn,encInfo)==e_success)
    {
        printf("Encode secret file extn Successfully\n");
    }
    else
    {
        printf("Encoding failed\n");
        return e_failure;
    }

    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)== e_success)
    {
        printf("Encode secret file size Successfully\n");
    }
    else
    {
        printf("Encoding failed\n");
        return e_failure;
    }

    if(encode_secret_file_data(encInfo)==e_success)
    {
         printf("Encode secret file data Successfully\n");
    }
    else
    {
        printf("Encoding failed\n");
        return e_failure;
    }

    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_dest_image) == e_success)
    {
         printf("copied remaining image data Successfully\n");
         return e_success;
    }
    else
    {
        printf("copied failed\n");
        return e_failure;
    }

  
}
