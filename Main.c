#include<stdio.h>
#include<stdlib.h>
#include "bmp.h"

int main (int argc,char * argv[])
{
if(argc != 3)
{
return EXIT_FAILURE;
}
  FILE * fptr;
  FILE * optr;
  fptr= fopen (argv[1],"r");
  if(fptr == NULL)
    {
      
      fprintf(stderr, "Can't open input file\n");
      return EXIT_FAILURE;
    }


   BMP_Image * image=Read_BMP_Image(fptr);
  

if(image == NULL)
{
fclose(fptr);
return EXIT_FAILURE;
}
optr = fopen(argv[2],"w");
  if(optr == NULL)
    {
      fprintf(stderr, "Can't open file for output\n");
fclose(fptr);
      return EXIT_FAILURE;
    }
if(image->header.bits == 16)
{
   image = Convert_16_to_24_BMP_Image(image);
}
else if(image->header.bits == 24)
{
   image = Convert_24_to_16_BMP_Image_with_Dithering(image);
}
else
{

fclose(fptr);
fclose(optr);
Free_BMP_Image(image);
return EXIT_FAILURE;
}

   Write_BMP_Image(optr,image);
   fclose(fptr);
   fclose(optr);
Free_BMP_Image(image);
return EXIT_SUCCESS;
}
