#!/bin/bash/perl 

$machine = `uname -p`;

print " Your machine is $machine \n";

$x=3;
$y=5;
$z=$x+$y;

printf("z=%2.5f\n",$z); 

$mydata=`ls -l`;

@array=split(/\n/,$mydata); 

 for($i=0; $i < $#array ; $i++){
     @array2 =split(/ /,$array[$i]); 
     printf("%s %s %s\n",$array2[3],$array2[4],$array2[7]);   

 }






