#include "header.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>

void  read_header(FILE *); 
void print_header(); 

unsigned long long datasize, nsamps;

char inputfilename[100]; 

int main(int argc, char *argv[]){
  FILE *inputfile,*outf; 
  struct stat stbuf;
  unsigned short s;
  int i,j,ichan ; 
  float data_point; 
  float *power,*raw_data,fr;  
   
  if(argc < 2){
    printf("./readheader.c [name of filterbank file] [outputfile name] \n");
    return(-1);
  }
  
  inputfile=fopen(argv[1],"rb");
  outf=fopen(argv[2],"w");

  if (stat(argv[1],&stbuf) >=0) {
    datasize = stbuf.st_size;
  }  else{
    printf(" File too big too handle !\n");
    return(-1);
  }

  read_header(inputfile);
 
  nbytes = nbits/8;
  nsamps = (datasize-totalbytes)/(nbytes*nchans);
       
  /* read header  */
 
  power=(float *)malloc(nchans*sizeof(float)); 
  raw_data=(float *)malloc(nchans*nsamps*sizeof(float)); 
 
  printf("\n\treading file %s\n\n",argv[1]); 

 for(i=0; i < nsamps; i++){
    for(j=0; j < nchans; j++){
      power[j] =0.0; 
         fread(&s,nbits/8,1,inputfile);
          raw_data[i+nsamps*j] = (float) s;
       } 
    }

printf("data read successfully !\n"); 

for(j=0; j < nchans; j++){
  
   fr = fch1+foff*j; 	
   power[j] =0.0; 
   for(i=0; i < nsamps; i++){
      power[j]+= raw_data[i+nsamps*j]; 
   } 
   printf("%2.6f %2.6f\n",fr,power[j]/nsamps);
   fprintf(outf,"%2.6f %2.6f\n",fr,power[j]/nsamps);
}

  return(0); 	
  
}



void print_header(){
 
  printf("............................................\n");
  printf("\n");
  printf("\tInput filename    : %s\n",inputfilename);
  printf("\tInput file size   : %lld Bytes\n",datasize);
  printf("\tInput file size   : %lld Bytes\n",datasize);
  printf("\tData  size        : %lld Bytes\n",datasize-totalbytes);
  printf("\tHeader size       : %d Bytes\n",totalbytes);
  printf("\t# of chanels      : %d\n",nchans);
  printf("\t# of samples      : %lld\n",nsamps);
  printf("\tsample size       : %d Bits\n",nbits);
  printf("\tfchan1            : %2.8f MHz\n",fch1);
  printf("\tchanel width e    : %2.8f MHz\n",foff);
  printf("\ttsamp             : %2.8f sec\n\n",tsamp);

}
