
float dmdelay(float f, float f_h, float DM){
      float t_f, t_f_h;
      float coeff = 4150.0;
      t_f = coeff * DM* (1.0/(f*f)-1.0/(f_h*f_h)); 
      return(t_f); 
}





