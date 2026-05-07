#include <stdio.h>
#include<string.h>
#include "common.h"
#include "decode.h"
#include "types.h"

     /*Read and validate decode arquments*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
 {
     // Check if input image(dest image) is provided
    if (argv[2] == NULL)
    {
        printf(RED"ERROR: No input image provided\n"RESET);
        return e_failure;
    }
     // Check if the input file has .bmp extension
    char *ptr = strstr(argv[2], ".bmp");
    if (ptr == NULL || strcmp(ptr, ".bmp") != 0)
    {
        printf(RED"ERROR: Please provide a valid .bmp file\n"RESET);
        return e_failure;
    }
     //store input image file name
    decInfo->dest_image_fname = argv[2];

    //Check output file name is provided or not
    if(argv[3]!=NULL)//if file name is provided ,not equal to NULL
    {
    if(strstr(argv[3],".")!=NULL)//check there is any extension or not .
    {
        char* extn=strtok(argv[3],".");// Remove extension if present
        strcpy(argv[3],extn);//copy the filename to argv[3] without extension.
        strcpy(decInfo->output_fname,argv[3]);//copy that it into output file name
    }
    }
    else //if output file not provided
      strcpy(decInfo->output_fname,"output"); //default output file name
      return e_success;
    
}

Status do_open_files(DecodeInfo *decInfo)
{

    // dest Image file
    decInfo->fptr_dest_image = fopen(decInfo->dest_image_fname, "rb");
   // Do Error handling
    if (decInfo->fptr_dest_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->dest_image_fname);

        return e_failure;
    }
      printf("INFO:Opened dest.bmp\n");
   
      return e_success;
}

    /*decode_magic_string*/
Status decode_magic_string(const char* magic_string,DecodeInfo *decInfo)
{
  unsigned char image_buffer[8];
  char data;
  int len=strlen(magic_string);
  fseek(decInfo->fptr_dest_image,54,SEEK_SET);//Skip BMP header (54 bytes)

  // Decode each character of magic string
  for(int i=0;i<len;i++)
  {
     if(fread(image_buffer,1,8,decInfo->fptr_dest_image)!=8)
        return e_failure;

     decode_byte_from_lsb(&data,image_buffer);

 // Compare decoded character with expected magic string
     if(data!=magic_string[i])
     {
        return e_failure;
      }
    }
  return e_success;
 }

     /* Decode secret file extension size */ 
Status decode_secret_file_extn_size(DecodeInfo* decInfo)
{
    unsigned char image_buffer[32];
     
     if(fread(image_buffer,1,32,decInfo->fptr_dest_image)!=32)
        return e_failure;
     decode_size_from_lsb(&decInfo->extn_secret_file_size,image_buffer);
      return e_success;
}
     /* Decode secret file extension */
Status decode_secret_file_extn( char* file_extn,DecodeInfo *decInfo)
{
     unsigned  char image_buffer[8];

    for(int i=0;i<decInfo->extn_secret_file_size;i++)
    {
    
         if((fread(image_buffer,1,8,decInfo->fptr_dest_image))!=8)
         {
          return e_failure;
         }
        decode_byte_from_lsb(&file_extn[i],image_buffer);
     }
          file_extn[decInfo->extn_secret_file_size]='\0';
          
          return e_success;
    }
    
    /* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
   unsigned char image_buffer[32];

   if(fread(image_buffer,1,32,decInfo->fptr_dest_image)!=32)
        return e_failure;
   decode_size_from_lsb(&decInfo->size_secret_file,image_buffer);
       return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
  
  char secret_file_data;
  unsigned char image_buffer[8];
  for(int i=0;i<decInfo->size_secret_file;i++)
  {
   
     if(fread(image_buffer,1,8,decInfo->fptr_dest_image)!=8)
         return e_failure;

    decode_byte_from_lsb(&secret_file_data,image_buffer);
    fputc(secret_file_data,decInfo->fptr_output);
  }
    return e_success;
}
      /* Extracts one byte of data from the LSBs of 8 image bytes. */
void  decode_byte_from_lsb(char *data, unsigned char *image_buffer)
{
    *data=0;
   for(int i=0;i<8;i++)
   {
    
      *data= *data | ((image_buffer[i] & 1)<<i);//to clear bits
   }
    
}
     /*  Extracts a 32-bit size value from the LSBs of 32 image bytes.*/
void decode_size_from_lsb(long int* size,unsigned char *imageBuffer)
{
   *size=0;
 for(int i=0;i<32;i++)
 {
   
    *size=*size | ((imageBuffer[i] & 1)<<i);
 }
}

Status do_decoding(DecodeInfo *decInfo)
{
     //  Open destination image file
    if (do_open_files(decInfo) != e_success)
        return e_failure;

     // Decode and verify magic string
    if (decode_magic_string(MAGIC_STRING, decInfo) != e_success)
    {
        printf(RED"ERROR: Magic string not decoded successfully!\n"RESET);
        fclose(decInfo->fptr_dest_image);
        return e_failure;
    }
    printf("INFO: Magic string decoded successfully!\n");

     // Decode secret file extension size
    if (decode_secret_file_extn_size(decInfo) != e_success)
    {
        printf(RED"ERROR: Could not decode secret file extension size\n"RESET);
        fclose(decInfo->fptr_dest_image);
        return e_failure;
    }

    //  Decode secret file extension
    if (decode_secret_file_extn(decInfo->extn_secret_file, decInfo) != e_success)
    {
        printf(RED"ERROR: Could not decode secret file extension\n"RESET);
        fclose(decInfo->fptr_dest_image);
        return e_failure;
    }

     // Create output file with decoded extension
     strcat(decInfo->output_fname,decInfo->extn_secret_file);
     decInfo->fptr_output = fopen(decInfo->output_fname, "wb");

    if(decInfo->fptr_output == NULL)
    {
        perror("fopen");
        printf("ERROR: Could not create output file %s\n", decInfo->output_fname);
        fclose(decInfo->fptr_dest_image);
        return e_failure;
    }
    printf("INFO: Output file created: %s\n",decInfo->output_fname);

    // Decode secret file size
    if (decode_secret_file_size(decInfo) != e_success)
    {
        printf(RED"ERROR: Could not decode secret file size\n"RESET);
        fclose(decInfo->fptr_dest_image);
        fclose(decInfo->fptr_output);
        return e_failure;
    }

    //Decode secret file data
    if (decode_secret_file_data(decInfo) != e_success)
    {
        printf(RED"ERROR: Could not decode secret file data\n"RESET);
        fclose(decInfo->fptr_dest_image);
        fclose(decInfo->fptr_output);
        return e_failure;
    }

    // Close files
    fclose(decInfo->fptr_dest_image);
    fclose(decInfo->fptr_output);

    printf("INFO: Decoding Done Successfully!\n");
    return e_success;
}
