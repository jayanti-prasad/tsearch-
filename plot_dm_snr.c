#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include "/usr/local/include/cpgplot.h"

int main(int argc, char *argv[]) {
  char  *xlabel, *ylabel,*outputfilename;
  FILE *inputfilename1; 
  float *x, *y, xmin,xmax,ymin,ymax;
  int i,n;

  n=atoi(argv[1]);
  xlabel= argv[2];  
  ylabel =argv[3];
  inputfilename1=fopen(argv[4],"r");
  

  outputfilename=argv[5];

  x=(float *)malloc(n*sizeof(float));
  y=(float *)malloc(n*sizeof(float));

  for(i=0; i <n; i++){
    fscanf(inputfilename1,"%f %f\n",&x[i],&y[i]);
  }

  xmin=1000000.0;
  ymin=1000000.0; 
  ymax=-1000000.0;
  ymax=-1000000.0;

  for(i=0; i <n; i++){
      if(x[i] > xmax){
       xmax=x[i];
     } if(y[i] > ymax){
       ymax=y[i]; 
     } if(x[i] < xmin){
       xmin=x[i]; 
     } if(y[i] < ymin){
       ymin=y[i];
     }
  }



  cpgbeg(0, "pgplot.ps/VPS", 1, 1);

  cpgscf(2);
  cpgpap(6.0,1.0);
  cpgenv(xmin,xmax,ymin,1.2*ymax,2,0);
  cpglab(xlabel,ylabel,"");

  cpgslw(3);
  cpgline(n,x,y);

  cpgend();

  
}





