#include<stdio.h>
#include<math.h>
#include<omp.h>
#include<stdlib.h>
#include<string.h>
#include "header.h"
#include<sys/stat.h>
#include<sys/types.h>
#include <sys/time.h>

void  read_header(FILE *); 
void print_header(); 
float dmdelay(float, float , float ); 
float time_diff(struct timeval,struct timeval );

long long datasize_max  = 800000000;
unsigned long long datasize, nsamps,block_size;
int id_max,num_blocks; 
char  inputfname[200],outfname[200];

int main(int argc, char *argv[]){
  unsigned short s;
  FILE *inputfile,*outf[200],*dmfile,*outf1;
  float *raw_data,*DM,*common;
  unsigned long long im,jm; 
  int i,j,k,l,m,i_start,i_end,i1,*id,ndm,extra,cond,tid; 
  float fr_l, fr_h, fr, dfr,tmp; 
  struct timeval  t1,t2,t3,t4,t5,t6;
  struct timezone   tzp;
  struct stat stbuf;
  float reading_time[20],computing_time[20],total_time; 

  gettimeofday (&t1, &tzp);
  
   if(argc < 4){
     printf("./dedisp.x <filterbank file name> <DM file name> <numthreads> <prefix for output file>\n"); 
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
   
   dmfile   = fopen(argv[2],"r");  
   DM = (float *)malloc(sizeof(float)); 
   
   omp_set_num_threads(atoi(argv[3]));
   
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
   
   if(datasize > datasize_max){
     block_size = datasize_max/(nchans*nbytes); 
     num_blocks = (int) (nsamps/block_size); 
   }else{ 
     block_size  = (datasize-totalbytes)/(nchans*nbytes);
     num_blocks  =1;
   }
   
   fr_h = fch1;
   fr_l = fch1 + nchans * foff;
   id_max = (int) (dmdelay(fr_l,fr_h,DM[ndm-1])/tsamp);
   

   for(i=0; i < num_blocks;i++){
    reading_time[i] = 0.0;
    computing_time[i] = 0.0;
   }

   extra = nsamps- block_size * num_blocks; 
  
   print_header(); 
   
   im = totalbytes; 
   
   gettimeofday (&t2, &tzp);
   
   for(m=0; m <num_blocks ; m++) {
     
     if(m == (num_blocks -1))
       block_size = block_size + extra - id_max; 
     
     raw_data=(float *)malloc(nchans*(block_size+id_max)*sizeof(float));
     
     fseek(inputfile, im, SEEK_SET);
   
     gettimeofday (&t3, &tzp);  
   
     printf("reading block %d\n",m); 
     printf("block %d starts at  %lld \n",m,im);
     
     for(i=0; i <block_size;  i++){
      for(j=0; j < nchans; j++){
	fread(&s,nbits/8,1,inputfile);
	raw_data[i+block_size*j] = (float) s;
      }
     }
     
     im = ftell(inputfile); 
     
     printf("block %d ends  at  %lld \n",m,im);
     printf("block %d of input file read successfully !\n",m);
       
     if (num_blocks > 1){
       printf("reading common block !\n");   
       for(i=0; i < id_max; i++){
	 for(j=0; j < nchans; j++){
	   fread(&s,nbits/8,1,inputfile);
	   raw_data[block_size+i+block_size*j] = (float) s;
	 }
       } 
       printf("read common block !\n"); 
     }
  
     gettimeofday (&t4, &tzp);
     reading_time[m] =  time_diff(t3,t4);
  
     fprintf(stderr,"block=%d reading time=%2.6f sec\n",m,reading_time[m]); 
 
    
 #pragma omp parallel for  private(l,i,j,i1,id,tid,fr,outfname,tmp)\
   shared(raw_data,DM,ndm,fr_l,fr_h,foff,nchans,nsamps,tsamp,m,outf)
    
     for(l=0; l < ndm; l++){
  
       id=(int  *)malloc(nchans*sizeof(int));
       tid = omp_get_thread_num();
       
       if (m==0){
	 sprintf(outfname,"%s_DM_%2.2f.tim",argv[4],DM[l]);     
	 outf[l]=fopen(outfname,"w");
       } 
      
       printf("thread %d dedispersing  block %d for DM %2.2f\n",tid,m,DM[l]);
       
       for(j=0; j < nchans; j++){
	 fr=fr_l + j*foff;
	 id[j] = (int) (dmdelay(fr,fr_h,DM[l])/tsamp);
      }
       
       for(i=0; i < block_size; i++){
	 tmp= 0.0;
	 for(j=0; j < nchans; j++){
	   i1 = i + id[j];
	   if (i1 < nsamps)  
	    tmp+=raw_data[i1+block_size*j];   
	 }      
	 
	 fprintf(outf[l],"%2.6f %2.6f \n",i*(m+1)*tsamp,tmp);
	 
      }  //i-loop close 
       
       printf("block %d dedispersed for DM %2.2f by thread %d\n",m,DM[l],tid);   
       
     } // l-loop close ; end of omp loop 
     
     free(raw_data); 
    
     gettimeofday (&t5, &tzp);

     computing_time[m] =  time_diff(t4,t5);

     fprintf(stderr,"block=%d computing  time=%2.6f sec\n",m,computing_time[m]);


   } // m-loop 
  
   gettimeofday (&t6, &tzp);

   total_time = time_diff(t2,t6);

   fprintf(stderr,"total_time=%2.6f sec\n",total_time); 

   outf1 = fopen("time_estimate.dat","w");
 
   fprintf(outf1,"\tnumber of paraller threads=%d\n",atoi(argv[3]));
   fprintf(outf1,"\t total time=%2.6f sec\n",total_time); 
   for(i=0; i < num_blocks; i++){
     fprintf(outf1,"reading time = %2.6f sec, computing time=%2.6f sec\n",reading_time[i],computing_time[i]); 
   }
   close(outf);
    
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
  printf("\tmaximum delay     : %d samples =%2.6f sec \n",id_max,id_max*tsamp);
  printf("\t......................................................\n");





}



float time_diff(struct timeval t1,struct timeval t2){
  float t;
  t = (t2.tv_sec-t1.tv_sec) + (t2.tv_usec-t1.tv_usec)/1000000.0;
  return(t);
}
