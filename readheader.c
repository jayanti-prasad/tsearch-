/*
    
   This program reads the header of a filterbank (sigproc readable) data file. 
                                         ---- Jayanti Prasad, Feb 23, 2009
  
*/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include <sys/stat.h>
#include "header.h"

int  read_header(FILE *); 

int main(int argc, char *argv[]){
  FILE *inputfile; 
  long long  nsamps,datasize; 
  struct stat stbuf; 

  if(argc < 2){
    printf("./readheader.c [name of filterbank file]\n"); 
    return(-1);
  }

  inputfile=fopen(argv[1],"rb"); 

 if (stat(argv[1],&stbuf) >=0) {
    datasize = stbuf.st_size;    	 
 }  else{ 
    printf(" File too big too handle !\n");
    return(-1); 
 }

  inputfile=fopen(argv[1],"rb"); 
  
  read_header(inputfile); 

  nbytes = nbits/8; 
  
  nsamps = (datasize-totalbytes)/(nbytes*nchans);   	

  printf("\n"); 
  printf("\tInput filename    : %s\n",argv[1]); 
  printf("\tInput file size   : %lld Bytes\n",datasize); 
  printf("\tData  size        : %lld Bytes\n",datasize-totalbytes);
  printf("\tHeader size       : %d Bytes\n",totalbytes); 
  printf("\t# of chanels      : %d\n",nchans); 
  printf("\t# of samples      : %lld\n",nsamps); 
  printf("\tsample size       : %d Bits\n",nbits); 
  printf("\tfchan1            : %2.8f MHz\n",fch1); 
  printf("\tchanel width e    : %2.8f MHz\n",foff); 
  printf("\ttsamp             : %2.8f sec\n\n",tsamp); 
  
  return(0); 	
  
}




