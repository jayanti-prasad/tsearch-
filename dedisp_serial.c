#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include "header.h"
#include<sys/stat.h>
#include<sys/types.h>

void print_header(); 
void  read_header(FILE *); 
float dmdelay(float, float , float ); 
unsigned long long datasize, nsamps;

char inputfilename[100]; 

int main(int argc, char *argv[]){
  unsigned short s;
  struct stat stbuf;
  FILE *inputfile,*outputfile,*dmfile;
  char  outputfilename[100];
  int i,j,k,l,i1,*id,ndm,id_max; 
  float *raw_data,*DM;
  float fr_l, fr_h, fr, dfr,tmp; 
  
  if(argc < 3){
    printf("./dedisp.x <filterbank file name>  <DM file name> <output file prefix>\n"); 
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
 
  dmfile   = fopen(argv[2],"r");  
  DM = (float *)malloc(sizeof(float)); 
  i=0;
  while(!feof(dmfile)){
    fscanf(dmfile,"%f\n",&tmp);
    DM[i] = tmp;
    i++;
    DM= realloc(DM, (i+1)*sizeof(float));
  }
  
  ndm = i;


  read_header(inputfile); 
  
  nbytes = nbits/8; 
  nsamps = (datasize-totalbytes)/(nbytes*nchans);   	
 
  print_header(); 
  
  fr_h = fch1; 
  fr_l = fch1 + nchans * foff; 

  id_max =  (int) (dmdelay(fr_l,fr_h,DM[ndm-1])/tsamp); 
  
  printf("\tmaximum delay = %d samples = %2.2f sec\n",id_max,tsamp*id_max); 

  raw_data=(float *)malloc(nchans*nsamps*sizeof(float));  

  id=(int  *)malloc(nchans*sizeof(int));  
  
  printf("\n\treading file %s\n\n",argv[1]); 
  
  for(i=0; i < nsamps; i++){
    for(j=0; j < nchans; j++){
      fread(&s,nbits/8,1,inputfile);
      raw_data[nsamps*j+i] = (float) s;
    }
  }
  
  printf("input file read successfully !\n");
  
  for(l=0; l < ndm; l++){

    printf("dedispersing for DM %2.2f\n",DM[l]); 

    sprintf(outputfilename,"%s_DM_%2.2f.tim",argv[3],DM[l]);
    outputfile=fopen(outputfilename,"w");
    
    for(j=0; j < nchans; j++){
       fr=fr_l + j*foff;
       id[j] = (int) (dmdelay(fr,fr_h,DM[l])/tsamp);
    }
      
    for(i=0; i < nsamps - id_max; i++){
      tmp  = 0.0; 
      for(j=0; j < nchans; j++){
	i1 = i + id[j]; 
	tmp+=raw_data[nsamps*j+i1];   
      } //j-loop close 
     fprintf(outputfile,"%f %f\n",i*tsamp,tmp);
  }  //i-loop close 
   
 } // l-loop close
  
  return(0); 
}


void print_header(){

	printf("......................................................\n");
        printf("\n");
        printf("\tInput filename    : %s\n",inputfilename);
        printf("\tInput file size   : %lld Bytes\n",datasize-totalbytes);
        printf("\tHeader size       : %d Bytes\n",totalbytes);
        printf("\t# of chanels      : %d\n",nchans);
        printf("\t# of samples      : %lld\n",nsamps);
        printf("\tsample size       : %d Bits\n",nbits);
        printf("\tfchan1            : %2.8f MHz\n",fch1);
        printf("\tchanel width      : %2.8f MHz\n",foff);
        printf("\ttsamp             : %2.8f sec\n\n",tsamp);
        printf("......................................................\n");

}


