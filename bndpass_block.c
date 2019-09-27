#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>
#include "header.h"
#include<sys/stat.h>
#include<sys/types.h>

void  read_header(FILE *); 
void print_header(); 
float dmdelay(float, float , float ); 

long long datasize_max  = 800000000;
unsigned long long datasize, nsamps,block_size;
int num_blocks,extra; 
char  inputfname[200],outfname[200];

int main(int argc, char *argv[]){
   unsigned short s;
   struct stat stbuf;
   FILE *inputfile,*outf;
   float *raw_data,*chan_data,*power; 
   unsigned long long im,jm; 
   int i,j,l; 
   float fr_l, fr_h, fr, dfr; 
   
   if(argc < 3){
     printf("./bndpass_block  [filterbank file name] [outputfilename]\n"); 
     return(-1); 
   }

   sprintf(inputfname,"%s",argv[1]); 
   inputfile=fopen(inputfname,"rb");
   
   if (stat(argv[1],&stbuf) >=0) {
     datasize = stbuf.st_size;
   }  else{
     printf(" File too big too handle !\n");
     return(-1);
   }
   
  read_header(inputfile); 
  
  nbytes = nbits/8; 
  nsamps = (datasize-totalbytes)/(nbytes*nchans);   	
  
  if(datasize > datasize_max){
    block_size = datasize_max/(nchans*nbytes); 
    num_blocks = (int) (nsamps/block_size); 
  }else{ 
    block_size  = (datasize-totalbytes)/(nchans*nbytes);
    num_blocks  =1;
  }
  
  fr_h = fch1;
  fr_l = fch1 + nchans * foff;
  
  
  print_header(); 

  extra = nsamps- block_size * num_blocks;

  chan_data=(float  *)malloc(nchans*num_blocks*sizeof(float));
  power=(float  *)malloc(nchans*sizeof(float));
   
  if(chan_data == NULL){
    printf("Not enough memory for the raw_data \n");
    return(-1);
  }

  for(l=0; l < num_blocks; l++){
      for(j=0; j < nchans; j++){
         chan_data[l+num_blocks*j] =0.0; 
    }
  }
   
  im = totalbytes; 

  outf=fopen(argv[2],"w"); 
  printf("writing output in %s file \n",argv[2]);  
 
  for(l=0; l <num_blocks ; l++) {

    raw_data=(float *)malloc(nchans*block_size*sizeof(float));
    
    fseek(inputfile, im, SEEK_SET);

    printf(" reading  block %d which starts at  %lld \n",l,im);
    
    for(i=0; i <block_size;  i++){
      for(j=0; j < nchans; j++){
	fread(&s,nbits/8,1,inputfile);
	raw_data[i+block_size*j] = (float) s;
      }
    }

    im = ftell(inputfile); 
    
    printf("block %d ends  at  %lld \n",l,im);
    printf("block %d of input file read successfully !\n",l);
   
   for(j=0; j < nchans; j++){
          for(i=0; i < block_size; i++){
             chan_data[l+num_blocks*j]+= raw_data[i+block_size*j];
       }
     }
   
   free(raw_data); 

}

  printf("chan_data computed !\n"); 


  for(i=0; i < nchans; i++){
   power[i] = 0.0;
    for(l=0; l < num_blocks; l++){
      power[i]+=chan_data[l+num_blocks*i]; 
    }
  fprintf(outf,"%2.6f  %2.6f\n",fch1+i*foff,power[i]/(block_size*num_blocks)); 
  printf("%2.6f  %2.6f\n",fch1+i*foff,power[i]/(block_size*num_blocks)); 
 }

 return(0); 
}


void print_header(){

  printf("\t......................................................\n");
  printf("\n");
  printf("\tInput filename    : %s\n",inputfname);
  printf("\tInput file size   : %lld Bytes\n",datasize);
  printf("\tData  size   :    : %lld Bytes\n",datasize-totalbytes);
  printf("\tHeader size       : %d Bytes\n",totalbytes);
  printf("\t# of chanels      : %d\n",nchans);
  printf("\t# of samples      : %lld\n",nsamps);
  printf("\tblock size        : %lld\n",block_size);
  printf("\tsample size       : %d Bits\n",nbits);
  printf("\tfchan1            : %2.8f MHz\n",fch1);
  printf("\tchannel width     : %2.8f MHz\n",foff);
  printf("\ttsamp             : %2.8f sec\n\n",tsamp);
  printf("\t......................................................\n");
  printf("\tblock size        : %lld\n",block_size);
  printf("\tnum_blocks        : %d\n",num_blocks);
  printf("\textra             : %d\n",nsamps- block_size * num_blocks); 
  printf("\t......................................................\n");





}
