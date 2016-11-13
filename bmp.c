#include <stdlib.h>
#include <stdio.h>

#include "bmp.h"



int Is_BMP_Header_Valid(BMP_Header* header, FILE *fptr) {
  // Make sure this is a BMP file
  if (header->type != 0x4d42) {
	 
    return FALSE;
  }
  
  if (header->offset != BMP_HEADER_SIZE) {
	   
    return FALSE;
  }
	      

  if (header->DIB_header_size != DIB_HEADER_SIZE) {
	    
    return FALSE;
  }

  // Make sure there is only one image plane
  if (header->planes != 1) {
	    
    return FALSE;
  }
  // Make sure there is no compression
  if (header->compression != 0) {
    return FALSE;
  }

  // skip the test for xresolution, yresolution

  // ncolours and importantcolours should be 0
  if (header->ncolours != 0) {
    return FALSE;
  }
  if (header->importantcolours != 0) {
  }
	  
  // Make sure we are getting 24 bits per pixel
  // or 16 bits per pixel

  if (header->bits != 24 && header->bits != 16) {
    return FALSE;
  }


  if( header->width * header->height <= 0)
    {
      return FALSE;
    }
  if( (header->size - 54)<=0)
    {
      return FALSE;
    }
  int check = 0;
  while ((((header->bits/8)*header->width)+check)%4 != 0)
    { 
      check ++;
    }
  if(((header->bits/8)*header->width+check)*header->height != header->imagesize)
    {
	     
      return FALSE;
    }



  return TRUE;
}


BMP_Image *Read_BMP_Image(FILE* fptr) {

  // go to the beginning of the file
	  
  BMP_Image *bmp_image = malloc(sizeof(BMP_Image));

  if(bmp_image == NULL)
    {
      fprintf(stderr, "Error allocating memory\n");
      return NULL;
    }
 




  

  int check;
  check= fread(&bmp_image->header,sizeof(BMP_Header),1,fptr);
  if(check != 1)
    {
      fprintf(stderr, "Error reading input file\n");
      return NULL;
    }


  // if read successful, check validity of header

  if(bmp_image != NULL)
    {
	      
      check =   Is_BMP_Header_Valid(&bmp_image->header, fptr);
      if (check == FALSE)
	{
	  fprintf(stderr, "Input file not in expected format\n");
	  free(bmp_image);
	  return NULL;
	}
    }



  // Allocate memory for image data

  bmp_image -> data = malloc(bmp_image->header.imagesize);
  if(bmp_image -> data == NULL)
    {
      fprintf(stderr, "Error allocating memory\n");
      free(bmp_image);
      return NULL;
    }


	 
  // read in the image data

  check=  fread(bmp_image->data,bmp_image->header.imagesize,1,fptr);
  if(check != 1)
    {
      fprintf(stderr, "Error reading input file\n");
      return NULL;
    }

  return bmp_image;
}


int Write_BMP_Image(FILE* fptr, BMP_Image* image) 
{
  // go to the beginning of the file

  fseek(fptr,0,SEEK_SET);
  // write header
  int check= fwrite(&(image->header),sizeof(image->header),1,fptr);
  if(check!= 1)
    { fprintf(stderr, "Error writing to output file\n");
      return FALSE;
    }
  // write image data
  check=fwrite(image->data,image->header.imagesize,1,fptr);
  if(check!=1)
    {
      fprintf(stderr, "Error writing to output file\n");
      return FALSE;
    }
  return TRUE;
}

/* The input argument is the BMP_Image pointer. The function frees memory of 
 * the BMP_Image.
 */
void Free_BMP_Image(BMP_Image* image) {

  free(image->data);
  free(image);
}

// Given a BMP_Image, create a new image that is a reflection 
// of the given image
// It could be a horizontal reflection (with the vertical mirror 
// being placed at the center of the image) 
// It could be a vertical reflection (with the horizontal mirror
// being placed at the center of the image)
// It could be a horizontal reflection followed by a vertical
// reflection (or equivalently, a vertical reflection followed by
// horizontal reflection).
// hrefl == 1 implies that a horizontal reflection should take place
// hrefl == 0 implies that a horizontal reflection should not take place
// vrefl == 1 implies that a vertical reflection should take place
// vrefl == 0 implies that a vertical reflection should not take place

BMP_Image *Reflect_BMP_Image(BMP_Image *image, int hrefl, int vrefl)
{
  // create a new image reflected from the given image
  /* BMP_Image *t_image = malloc(sizeof(BMP_Image));
     t_image->data = malloc(image->header.imagesize);
     t_image->header = image->header;*/
  int width,limit;
  if(image->header.bits == 24)
    {
      width = image->header.width * 3;
      limit = 3;
    }
  else
    {
      width = image->header.width * 2;
      limit = 2;
    }int push = 0;

  while(width % 4 != 0)
    { push++;
      width++;
    }
	 
  char hold [image->header.height][width];
  int i,j,count=0;

  if(hrefl == 0 && vrefl==0)
    {
      fprintf(stderr, "Can't reflect the image\n");
      return image;
    }
  //VERTICAL

  if(vrefl ==1)
    {
      for(i=0;i<image->header.height;i++)
	{
	  for(j=0;j<width;j++)
	    {
	      hold[i][j] = image->data[count];
	      count++;
	    }
	}
      if(vrefl == 1)
	{ count = 0;
	  for(i=image->header.height-1;i>=0;i--)
	    {

	      for(j=0;j<width;j++)
		{
		  image->data[count] = hold[i][j];
		  count++;
		}
	    }
	}
    }
	      

  //HORIZONTAL
	      
  if(hrefl == 1)
    { int k =0;
      count = 0;

      char hold2[image->header.height][image->header.width][3];
      for(i=0;i<image->header.height;i++)
	{
	  for(j=0;j<image->header.width;j++)
	    {
	      for(k=0;k<limit;k++)
		{
		  hold2[i][j][k] = image->data[count];
		  count++;
		}
	    }count = count + push;
	}count=0;
      for(i=0;i<image->header.height;i++)
	{
	  for(j=image->header.width-1;j>=0;j--)
	    {
	      for(k=0;k<limit;k++)
		{
		  image->data[count] = hold2[i][j][k];
		  count++;
		}
	    }count = count +push;
	}

    }


  // Free_BMP_Image(image);

  return image;
}


// 24 to 16 BIT CONVERSION


void print_char_in_bits(char number)
{
  int i;
  unsigned char mask = 1 ;
  for (i = 0; i < 8; i++) {
    // need to form a mask
    mask = 1;
    mask <<= (8 - i - 1);
    int result = mask & number;
    if (result == 0) {
      printf("0");
    } else {
      printf("1");
    }
    mask = 1;
  }
}
void print_char_in_bits16(uint16_t number)
{
  int i;
  unsigned short mask = 1 ;
  for (i = 0; i < 16; i++) {
    // need to form a mask
    mask = 1;
    mask <<= (16 - i - 1);
    int result = mask & number;
    if (result == 0) {
      printf("0");
    } else {
      printf("1");
    }
    mask = 1;
  }
}

BMP_Image *Convert_24_to_16_BMP_Image(BMP_Image *image)
{
  if(image->header.bits!=24)
    {
      fprintf(stderr, "Not a 24-bit BMP format, can't convert\n");
      return image;
    }
  int i,j,count=0,width;
  if(image->header.bits == 24)
    {
      width = image->header.width * 3;
	      
    }
  int push = 0;

  while(width % 4 != 0)
    { push++;
      width++;
    }
	   
  uint8_t holder1,holder2,holder3;
  uint16_t hold[image->header.height][image->header.width],final=0; int wn=0;
  for(i=0;i<image->header.height;i++)
    { 
      for(j=0;j<image->header.width;j++)
	{ 
	
	  final = 0x0000;
	  final = final>>16;
	
	  holder1 = image->data[count];

	  count++;

	  holder2 = image->data[count];

	  count++;
	  
	  holder3 = image->data[count];

	  holder1 = holder1>>3;holder2=holder2>>3;holder3=holder3>>3;
	
	  holder3 = holder3 <<2;
	  holder3 =holder3 | (holder2>>3);
	 
	  final = final | ((uint16_t)holder3);
	  holder2 = holder2<<5;holder2 = holder2 | holder1;

	  final = final|((uint16_t)holder2<<8);


	  hold[i][j] = final;
\
	  count++;
	}count = count + push;
      //  new = 0;
    }
  int check = 0;
  BMP_Image * new = malloc(sizeof(BMP_Image));
  new->header=image->header; 
  new->header.bits = 16;
  if((image->header.width*2) % 4 != 0)
    {
      check = 1;
      new->data = malloc(sizeof(unsigned char)*(image->header.width+2)*2*image->header.height);
    }
  else
    {
      new->data = malloc(sizeof(unsigned char)*image->header.width*2*image->header.height);
    }

  count = 0;push = 0;
  wn = image->header.width * 2;//int k;
  while(wn%4 != 0)
    {
      push++;
      wn++;
    }
  for(i=0;i<image->header.height;i++)
    {
      for(j=0;j<image->header.width;j++)
	{
	  new->data[count]=(hold[i][j]>>8);
	  count++;
	  new->data[count]=hold[i][j];
	  count++;
	}
      if(check == 1)
      	{// printf("hello");
      	  new->data[count]=0x00;
      	  count++;
      	  new->data[count]=0x00;
      	  count++;
      	}else count = count + push;
      
    }
  new->header.imagesize = new->header.height * wn ;
  new->header.size=new->header.imagesize + 54;
  //  printf("\n size %d",new->header.imagesize);
  Free_BMP_Image(image);
  return new;
}

BMP_Image *Convert_16_to_24_BMP_Image(BMP_Image *image)
{
  if(image->header.bits != 16)
    {
      fprintf(stderr, "Not a 16-bit BMP format, can't convert\n");
      return image;
    }
  int i,j,k,count=0,width,push=0,push2=0,count2=0;
  width = image->header.width * 2;
  while(width % 4 !=0)
    {
      push++;
      width++;
    }
  width = image->header.width  * 3;
  while(width%4!=0)
    {
      push2++;
      width++;
    }

  uint8_t hold1,hold2;
  BMP_Image * new = malloc(sizeof(BMP_Image));
  new->header = image->header;
  new->data = malloc(sizeof(unsigned char)*image->header.height*width);
  new->header.bits = 24;
  new->header.imagesize = width*image->header.height;
  new->header.size = new->header.imagesize + 54;

  for(i=0;i<image->header.height;i++)
    {
      for(j=0;j<image->header.width;j++)
	{
	  hold1 = image->data[count];
	  count++;
	  hold2 = image->data[count];
	  count++;
	  new->data[count2] = (hold1<<3);
	
	  new->data[count2] = ((unsigned int)new->data[count2]>>3)*255/31;
	 
	  count2++;
	  new->data[count2] = hold2 << 6;
	  new->data[count2] =new->data[count2]| hold1 >> 2;new->data[count2] = (new->data[count2] & 0xf8);

	  new->data[count2] = ((unsigned int)new->data[count2]>>3)*255/31;
	  
	  count2 ++;
	  new->data[count2] = hold2>>2; new->data[count2] = (new->data[count2] << 3);

	  new->data[count2] = ((unsigned int)new->data[count2]>>3)*255/31;
	  
	  count2++;
	}count = count + push;
      for(k=0;k<push2;k++)
	{
	  new->data[count2] = 0x00;
	  count2++;
	}
    }
  Free_BMP_Image(image);
  return new;
}

//  *****PA05******

BMP_Image *Convert_24_to_16_BMP_Image_with_Dithering(BMP_Image *image)
{
  if(image->header.bits!=24)
    {
      fprintf(stderr, "Not a 24-bit BMP format, can't convert\n");
      return image;
    }
  int i,j,k,count=0,width;
  if(image->header.bits == 24)
    {
      width = image->header.width * 3;
	      
    }
  int push = 0;

  while(width % 4 != 0)
    { push++;
      width++;
    }
	   

  //  unsigned char orig[image->header.height][image->header.width][3]; 
  // malloc error array. because possibly less space in stack
  int ***error = (int***)malloc(image->header.height*sizeof(int**));
 unsigned char ***orig = (unsigned char***)malloc(image->header.height*sizeof(unsigned char**));

  for (i = 0; i < image->header.height; i++)
    {
	  		 
      error[i] = (int**)malloc(image->header.width*sizeof(int*));
      for (j = 0; j < image->header.width; j++)
	{
	  error[i][j] = (int*)malloc(3*sizeof(int));
	}
    }

  for (i = 0; i < image->header.height; i++)
    {
	  		 
      orig[i] = (unsigned char**)malloc(image->header.width*sizeof(unsigned char*));
      for (j = 0; j < image->header.width; j++)
	{
	  orig[i][j] = (unsigned char*)malloc(3*sizeof(unsigned char));
	}
    }
  unsigned int x;int val;
  for(i=0;i<image->header.height;i++)
    {
      for(j=0;j<image->header.width;j++)
	{
	  for(k=0;k<3;k++)
	    {
	      orig[i][j][k] = image->data[count];
	      count++;
	      error[i][j][k] = 0;
	     
	    }
	}
      count = count + push;
    }
  for(i=image->header.height-1;i>=0;i--)
    {
      for(j=0;j<image->header.width;j++)
	{
	  for(k=0;k<3;k++)
	    {
	      x = (unsigned int)orig[i][j][k] + (int)(error[i][j][k]/16);

	      if(x>255){x=255;}
	      if(x<0){x=0;}

	      orig[i][j][k] = x;
		                                                     
	   
	  
		          

	      val = orig[i][j][k] - ((orig[i][j][k]>>3)*255)/31;
	      if(j<image->header.width-1)
		error[i][j+1][k] = error[i][j+1][k] + ((val*7)); 
	      if(i>0 && j>=1)
		error[i-1][j-1][k] = error[i-1][j-1][k] + ((val*3));
	      if(i>0)
		error[i-1][j][k] = error[i-1][j][k] + ((val*5));
	      if(i>0 && j<image->header.width-1)
		error[i-1][j+1][k] = error[i-1][j+1][k] + ((val*1));

	    }
	}
    }
  for(i=0;i<image->header.height;i++)
    {
      for(j=0;j<image->header.width;j++)
	{
	  for(k=0;k<3;k++)
	    {
	      if(orig[i][j][k] <0)
		orig[i][j][k] = 0;
	      else if(orig[i][j][k] > 255)
		orig[i][j][k] = 255;
	    }
	}
    }
  count = 0;
  for(i=0;i<image->header.height;i++)
    {
      for(j=0;j<image->header.width;j++)
	{
	  for(k=0;k<3;k++)
	    {
	      image->data[count] = orig[i][j][k];
	      count++;
	    }
	}count = count +push;
    }
  image = Convert_24_to_16_BMP_Image(image);
  for (i = 0; i < image->header.height; i++) 
    {
  		 

      for (j = 0; j < image->header.width; j++) 
	{
	  free(error[i][j]);
	}
      free(error[i]);
    }
  free(error);
  for (i = 0; i < image->header.height; i++) 
    {
  		 

      for (j = 0; j < image->header.width; j++) 
	{
	  free(orig[i][j]);
	}
      free(orig[i]);
    }
  free(orig);
 
  return image;

}
