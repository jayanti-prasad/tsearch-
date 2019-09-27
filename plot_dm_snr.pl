#!/usr/bin/perl 

if($#ARGV <=0){
help();
exit;
} else {
$i=0;
 if($ARGV[$i]=~m/-inpdir/){
  $datadir=$ARGV[++$i];
  $i++;
 }if($ARGV[$i]=~m/-outdir/){
  $plotsdir=$ARGV[++$i];
  $i++;  
 }if($ARGV[$i]=~m/-xlab/){
  $xlabel=$ARGV[++$i];
  $i++; 
 }if($ARGV[$i]=~m/-ylab/){
  $ylabel=$ARGV[++$i]; 
 } 

 @pp=split(/\//,$datadir);
 $beamname=pop @pp;


 if(!-e "$plotsdir"){
   system "mkdir $plotsdir";
 }if(!-e "$plotsdir/$beamname"){
  system "mkdir $plotsdir/$beamname";
 }
  
  $tmp=`ls $datadir`;
  @filearray=split(/\n/,$tmp);

  print"plots will be put in: $plotsdir/$beamname \n";

 system "LD_LIBRARY_PATH=/home/jayanti/bin/pgplot/";
 system "export LD_LIBRARY_PATH";



  for ($i=0; $i <$#filearray; $i++){
     @ar=split(/ /,`wc -l $datadir/$filearray[$i]`);
     $n=$ar[0];
  
     system "./plot_dm_snr.x   $n  $xlabel $ylabel $datadir/$filearray[$i]  pg.ps";
     
     $psfilename=$filearray[$i];
     $psfilename =~ s/PULSE/ps/; 
   
     system "mv pgplot.ps $psfilename";
     
     system "mv $psfilename $plotsdir/$beamname"; 
  }

}


sub help(){
  print"-inpdir: input directory\n";
  print"-outdir: plots directory\n";
  print"-xlab  : xlabel \n";
  print"-ylab  : ylabel \n";
  print"Example:./plot_dm_snr.pl  -inpdir /data/jayanti/transient_search/tsearch/v3.0/d0/results/dmsnr/00111/ -outdir tmp_plots   -xlab DM -ylab SNR\n"
}
