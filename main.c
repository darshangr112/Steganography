#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>
OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
   if (argc == 1)
    {
        printf("ERROR: Required arguments are missing.\n");
        printf("Usage:\n");
        printf("For Encode: %s -e <source.bmp> <secret.txt> [output.bmp]\n", argv[0]);
        printf("For Decode: %s -d <stego.bmp> [output_file]\n", argv[0]);
        return e_failure;
    }
   OperationType op = check_operation_type(argv[1]);

   if(op==e_encode)
   {
     if (argc < 4)
     {
        printf("ERROR: Required arguments are missing.\n");
        printf("Usage: %s -e <source.bmp> <secret.txt> [output.bmp]\n", argv[0]);
         return e_failure;
     }

        EncodeInfo encInfo;

      if(read_and_validate_encode_args(argv,&encInfo)==e_success)
      {
         if(do_encoding(&encInfo) == e_success)
         {
            printf("\t:::Encoding successfully:::\t\n");
               return e_success;
         }
         else
         {
               printf("Encoding failed\n");
               return e_failure;
         }
      }
      else
      {
            printf("Invalid arguments\n");
            return e_failure;
      }
   }
   else if(op==e_decode)
   {
      if(argc < 3)
      {
            printf("ERROR:Required arguments are missing.\n");
            printf("Usage:%s -d <stego.bmp> [output_file]\n", argv[0]);
            return d_failure;
      }
       
        DecodeInfo decInfo;
      if(read_and_validate_decode_args(argv,&decInfo)==d_success)
      {
         if(do_decoding(&decInfo) == d_success)
         {
            printf("\t:::Decoding successfully:::\t\n");
               return d_success;
         }
         else
         {
               printf("Decoding failed\n");
               return d_failure;
         }
      }
      else
      {
            printf("Invalid arguments\n");
            return d_failure;
      }
   }
   else
   {
      printf("ERROR: Invalid operation.\n");
      printf("Use -e for encoding or -d for decoding.\n");
      return e_failure;
   }

}
OperationType check_operation_type(char *symbol)
{
   if(strcmp(symbol,"-e")==0)
   {
    return e_encode;
   }
   else if(strcmp(symbol,"-d")==0)
   {
    return e_decode;
   }
   else
   {
    return e_unsupported;
   }
}
