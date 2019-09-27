#include<stdio.h>
#include<stdlib.h>
#include<math.h>



int main(int argc, char* argv[]){

	float *f, *power, *f1, *power1, tmp1, tmp2, thrs;
	int i, j, k, i1, i2, nchans, nchans1, ichans, *icount;
	FILE *inpf, *outf;
	char outfname[100];

	if( argc < 4){
         printf("./broadbandperiodicrfi <input file> <threshold>  <inchans> \n");
	 return(-1); 
	}

	inpf = fopen(argv[1],"r");
        thrs = atof(argv[2]);
	ichans = atoi(argv[3]); 


	f=(float *)malloc(sizeof(float)); 
	power=(float *)malloc(sizeof(float)); 


        i =0; 	
	while(!feof(inpf)) {
         fscanf(inpf,"%f %f\n",&tmp1,&tmp2); 
          f[i] = tmp1;
          power[i] = tmp2; 
    	  i++;
          f =  realloc(f, (i+1)*sizeof(float));
          power  = realloc(power, (i+1)*sizeof(float));
	}

	nchans = i;

	nchans1 = (int) ((float) nchans/ichans); 

	f1 = (float *)malloc(nchans1 * sizeof(float)); 
	power1 = (float *)malloc(nchans1 * sizeof(float)); 



	for(i=0; i < nchans1 ; i++){
          f1[i] = 0.0; 
	  power1[i] = 0.0;
	  for(j=i * ichans; j < (i+1) * ichans ; j++){
            power1[i]+=power[j];
	    f1[i] +=f[j];  
	  } 
          power1[i] /=ichans;
	  f1[i] /= ichans; 
          printf("%2.6f %2.6f\n",f1[i],power1[i]); 
	}

	return(0); 


}
