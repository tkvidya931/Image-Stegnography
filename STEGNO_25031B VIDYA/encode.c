#include <stdio.h>
#include<string.h>
#include"common.h"
#include "encode.h"
#include "types.h"

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
    fseek(fptr, 0, SEEK_END);   // Move to end of file
    uint size = ftell(fptr);   // Get file size
    rewind(fptr);              // Reset file pointer
    return size;
    // Find the size of secret file data
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

   /* Validate command-line arguments for encoding */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char* ptr=strstr(argv[2],".bmp"); //checking source image is bmp or not
    if(ptr!=NULL)
       encInfo->src_image_fname=argv[2];
    else 
    {
       printf(RED"Error!Please provide valid .bmp file name\n"RESET);
       return e_failure;
    }
    //Checking secret file is .txt or not
    char* extn=strstr(argv[3],".");
    if(extn==NULL)
    {
      printf(RED"Secret file must have one extension!...."RESET);
      return e_failure;
    }
    else
    {
    if(strcmp(extn,".txt")==0)
        encInfo->secret_fname=argv[3];
    else if(strcmp(extn,".c")==0)
        encInfo->secret_fname=argv[3];
    else if(strcmp(extn,".sh")==0)
        encInfo->secret_fname=argv[3];
     else if(strcmp(extn,".h")==0)
        encInfo->secret_fname=argv[3];
     else
     {
         printf(RED"Error!Please provide valid  file name\n"RESET);
         return e_failure;
     }
    }
  // Checking output file argument
    if(argv[4]!=NULL)
    {
        char* dt=strstr(argv[4],".bmp");
        if(dt!=NULL)
        {
          encInfo->dest_image_fname=argv[4];
          return e_success;
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        //default destiantion file name
        printf("Output file not mentioned.Creating dest_bmp as default\n");
        encInfo->dest_image_fname="dest.bmp";
        
        return e_success;
    }  
}

/* Open all files required for encoding */
Status open_files(EncodeInfo *encInfo)
{

    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    printf("INFO:Opened beautiful.bmp\n");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    printf("Opened %s\n",encInfo->secret_fname);
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // dest Image file
    encInfo->fptr_dest_image = fopen(encInfo->dest_image_fname, "wb");
    printf("INFO:Opened destination_image\n");
    // Do Error handling
    if (encInfo->fptr_dest_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->dest_image_fname);

        return e_failure;
    }

     printf("INFO:Done\n");
    // No failure return e_success
    return e_success;
}
/* Check if source image has enough capacity to hide secret file */
Status check_capacity(EncodeInfo *encInfo)
{
   
   printf("INFO:Checking for extension size\n");
   encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
   printf("INFO:Done. Not Empty\n");
   printf("INFO:Checking for beautiful.bmp capacity to handle secret.txt\n");
   encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
   printf("Image capacity=%d\n",encInfo->image_capacity);
   // Minimum capacity required = magic string + extension size + extension + secret file data
   if((encInfo->image_capacity)>((strlen(MAGIC_STRING)*8)+32+(strlen(encInfo->extn_secret_file)*8)+(encInfo->size_secret_file*8)))
   {
      printf("INFO:Done...Found OK\n");
      return e_success;
   }
    else 
      return e_failure;

}
/* Copy first 54 bytes of BMP header from source to destination */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    unsigned char header[54];
    rewind(fptr_src_image);
    fread(header,54,1,fptr_src_image);
    fwrite(header,54,1,fptr_dest_image);
    if(ftell(fptr_src_image)==ftell(fptr_dest_image)) //check to see if file pointers match after copy
      return e_success;
    else
      return e_failure;
}
 /* Encode magic string into image */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
 unsigned char image_buffer[8];
 int len=strlen(magic_string);
 for(int i=0;i<len;i++)
 {
     fread(image_buffer,1,8,encInfo->fptr_src_image);//Read 8 bytes from source image
     encode_byte_to_lsb(magic_string[i],image_buffer);//Encode one character
     fwrite(image_buffer,1,8,encInfo->fptr_dest_image);//write encoded bytes
 }
 if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image)) //check to see if file pointers match after copy
      return e_success;
  else
      return e_failure;
    
}
  /* Encode secret file extension size */
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    unsigned char image_buffer[32];
    fread(image_buffer,1,32,encInfo->fptr_src_image);//Read 32 bytes from string
    encode_size_to_lsb(size,image_buffer);//Encode size
    fwrite(image_buffer,1,32,encInfo->fptr_dest_image);
    if((ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image)))
    {
        return e_success;
    }
    else
      return e_failure;
}

/* Encode secret file extension */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
     unsigned  char image_buffer[8];
    int file_len=strlen(file_extn);
    for(int i=0;i<file_len;i++)
    {
        fread(image_buffer,1,8,encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i],image_buffer);
        fwrite(image_buffer,1,8,encInfo->fptr_dest_image);

    }
    if((ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image)))
    {
    return e_success;
    }
   return e_failure;

}

  /* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
   unsigned char image_buffer[32];
   fread(image_buffer,1,32,encInfo->fptr_src_image);
   encode_size_to_lsb(file_size,image_buffer);
   fwrite(image_buffer,1,32,encInfo->fptr_dest_image);
   if((ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image)))
    {
    return e_success;
    }
   return e_failure;

}
   /* Encode secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
  
  char secret_file_data[encInfo->size_secret_file];
  rewind(encInfo->fptr_secret);
  fread(secret_file_data,encInfo->size_secret_file,1,encInfo->fptr_secret);
  unsigned char image_buffer[8];
  for(int i=0;i<encInfo->size_secret_file;i++)
  {
    fread(image_buffer,1,8,encInfo->fptr_src_image);//Read 8 bytes
    encode_byte_to_lsb(secret_file_data[i],image_buffer);//Encode 1 byte
    fwrite(image_buffer,1,8,encInfo->fptr_dest_image);
  }
  if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image))
  {
    return e_success;
  }
  return e_failure;

}
/* Copy remaining image data after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
   int ch;
   while((ch=fgetc(fptr_src)) !=EOF)
   {
    fputc(ch,fptr_dest);

   }
   if((ftell(fptr_src)==ftell(fptr_dest)))
      return e_success;
    else 
       return e_failure;
}
 /* Encode a byte into 8 bytes of image buffer using LSB */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
  
   //to store source image first 8 byte datas to image buffer. s
   for(int i=0;i<8;i++)
   {
    
      image_buffer[i]=image_buffer[i] & 0XFE;//to clear lsb 
      image_buffer[i]= image_buffer[i] | ((data>>i)&1);//encode lsb first.
   }
    
}
/* Encode 32-bit size to 32 bytes of image buffer using LSB */
Status encode_size_to_lsb(int size, char *imageBuffer)
{
 for(int i=0;i<32;i++)
 {
   
    imageBuffer[i]=imageBuffer[i] & 0xFE;//clear lsb of image byte
    imageBuffer[i]=imageBuffer[i]|((size>>i)&1);//replace lsb of image byte with size 
 }
}

Status do_encoding(EncodeInfo *encInfo)
{  
 if(open_files(encInfo)==e_success)
 {
  if(check_capacity(encInfo)==e_success)
  {
    printf("INFO:Copying Image Header\n");
    if((copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_dest_image))==e_success)
    {
      printf("INFO:Done\n");
      printf("INFO:## Encoding Procedure Started ##\n");
      printf("INFO:Encoding Magic String Signature\n");
     if((encode_magic_string(MAGIC_STRING,encInfo))==e_success)
     {
        printf("INFO:Done\n");
        char* ptr=strchr(encInfo->secret_fname,'.');
        if(ptr!=NULL)
        {
            strcpy(encInfo->extn_secret_file,ptr); 
        }
        int length=strlen(encInfo->extn_secret_file);
      
        printf("INFO:Encoding secret File  Extension Size\n");
       
      if((encode_secret_file_extn_size(length,encInfo))==e_success)
       {
         printf("INFO:Done");
         printf("INFO:Encoding secret File Extension\n");
       if((encode_secret_file_extn(encInfo->extn_secret_file,encInfo))==e_success)
        {
        printf("INFO:Done\n");
         encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
          printf("INFO:Encoding secret file Size\n");
        if((encode_secret_file_size(encInfo->size_secret_file,encInfo))==e_success)
        {
          printf("INFO:Done\n");
          printf("INFO:Encoding secret File Data\n");
         if((encode_secret_file_data(encInfo)==e_success))
         {
            printf("INFO:Done\n");
            printf("INFO:Copying Left Over Data\n");
           if((copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_dest_image))==e_success)
           {
            printf("INFO:Done\n");
            printf("INFO:##Encoding Done Successfully ##");
            return e_success;
           }
           printf(RED"INFO:## Encoding not completed ##"RESET);
           return e_failure;
         }
         printf(RED"INFO: Copyiyng Left Over Data Failed!\n"RESET);
         return e_failure;
        }
        printf(RED"INFO:Failure in Encoding secret file Data\n"RESET);
        return e_failure;
       }
        printf(RED"INFO:Secret file size encoding failed\n"RESET);
        return e_failure;
      }
      printf(RED"INFO:Secret.txt file extension encoding failed!"RESET);
      return e_failure;
     } 
     printf(RED"INFO:Secret file extension encoding failed!\n"RESET);
     return e_failure;
    }
    printf(RED"Magic string encoding failed!\n"RESET);
    return e_failure;
  }
  else
    
     printf(RED"Copying image header failed!\n"RESET); 
     return e_failure;
}

     //close all files
      fclose(encInfo->fptr_src_image);
      fclose(encInfo->fptr_secret);
      fclose(encInfo->fptr_dest_image);

}
