/*
  This program reads the time series of a channel of  a 
  filterbank (sigproc readable) data file.
  
              ---- Jayanti Prasad, Feb 23, 2009
*/

#include "header.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>

void  read_header(FILE *); 
void print_header(); 
char inputfilename[100], outputfilename[100]; 

long long datasize, nsamps;

int main(int argc, char *argv[]){
  FILE *inputfile,*outf; 
  struct stat stbuf;
  unsigned short s;
  int i,j,ichan ; 
  float data_point; 
   
  if(argc < 3){
    printf("./readheader.c [name of filterbank file] [chanel number]\n");
    return(-1);
  }
  
  sprintf(inputfilename,"%s",argv[1]); 
  inputfile=fopen(inputfilename,"rb");
  
  if (stat(argv[1],&stbuf) >=0) {
    datasize = stbuf.st_size;
  }  else{
    printf(" File too big too handle !\n");
    return(-1);
  }
  
  inputfile=fopen(argv[1],"rb");
  ichan  = atoi(argv[2]);

  read_header(inputfile);
 
  nbytes = nbits/8;
  nsamps = (datasize-totalbytes)/(nbytes*nchans);
  

  sprintf(outputfilename,"chan_%d.out",ichan); 
 
  outf=fopen(outputfilename,"w");  
    
  /* read header  */
  
  printf("\n\treading file %s\n\n",argv[1]); 
  
  for(i=0; i < nsamps; i++){
    for(j=0; j < nchans; j++){
      fread(&s,nbits/8,1,inputfile);
      data_point = (float) s;
      if (j==ichan-1){
	fprintf(outf,"%f %2.0f\n",i*tsamp,data_point);
      }
    }
  }
  return(0); 	
  
}



void print_header(){

  printf("...............................................\n"); 
  printf("\n");
  printf("\tInput filename    : %s\n",inputfilename);
  printf("\tInput file size   : %lld Bytes\n",datasize);
  printf("\tData  size        : %lld Bytes\n",datasize-totalbytes);
  printf("\tHeader size       : %d Bytes\n",totalbytes);
  printf("\t# of chanels      : %d\n",nchans);
  printf("\t# of samples      : %lld\n",nsamps);
  printf("\tsample size       : %d Bits\n",nbits);
  printf("\tfchan1            : %2.8f MHz\n",fch1);
  printf("\tchanel width e    : %2.8f MHz\n",foff);
  printf("\ttsamp             : %2.8f sec\n\n",tsamp);
  printf(".................................................\n");


}

