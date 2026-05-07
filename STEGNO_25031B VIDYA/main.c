

/*            Name : VIDYA T K
Name of the project: STEGANOGRAPHY
        Description: This project implements Image Steganography using the Least Significant Bit (LSB) technique to securely hide secret data within a BMP image.
                     The goal of this project is to enable confidential data transmission without revealing the presence of hidden information.
                     The encoding process embeds a magic string, secret file size, and secret data into the least significant bits of the image pixel values using bitwise operations. 
                     The decoding process extracts the embedded information from the stego-image and reconstructs the original secret file after validation.
                     The project is developed using the C programming language, focusing on file I/O operations, pointers, structures, and modular programming. 
              Date : 15/01/2026       */
            


#include <stdio.h>
#include<string.h> 
#include "common.h"
#include "encode.h"
#include "types.h"
#include "decode.h"

OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    
    if(argc<2)
    {
        printf(RED"Command line argument should contain minimum 3 for decoding and 4 arguments for encoding\n"RESET);
        return 1;
    }
         if((check_operation_type(argv[1]))==e_encode)
        {
           if(argc<4) 
           {
            printf(RED"Insufficient arguments for encoding!\n"RESET);
            return 1;
           }
              EncodeInfo encInfo;
              if(read_and_validate_encode_args(argv,&encInfo)==e_success)
              {
                printf("INFO:Opening required files\n");
                do_encoding(&encInfo);
              }
              else
              {
                printf(RED"Encoding argument validation failed\n"RESET);
                 return 1;
             }
        }
        else if((check_operation_type(argv[1]))==e_decode)
         {
            if(argc<3)
           {
            printf(RED"Insufficient arguments for decoding!\n"RESET);
            return 1;
           }
            DecodeInfo decInfo;
           if(read_and_validate_decode_args(argv,&decInfo)==e_success)
           {
            printf("INFO:Opening required files\n");
            do_decoding(&decInfo);
  
           }
           else
           {
            printf(RED"Decoding argument validation failed\n"RESET);
            return 1;
           }
         }
         else
         {
            printf(RED"Error!Unsupported\n"RESET);
            return 1;
         }
         return 0;

     }
   
OperationType check_operation_type(char *symbol)
{
    if(strcmp(symbol,"-e")==0)
      return e_encode;
    else if(strcmp(symbol,"-d")==0)
       return e_decode;
    else
      return e_unsupported;
}
