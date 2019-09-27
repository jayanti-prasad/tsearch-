#!/usr/bin/perl
#-------------------------------------------------------#
# -This program finds pules above some threshold (3 sigma)
#  from GMRT filter bank data at DM 0 and compares the SNR 
#  of thses pulses at variois DMs.
# -In this program dedispersion is done using 'dedesperse'
#  of SIGPROC. 
#  Once pulses above threshold are identifed, DM vs SNR can 
#  be plotted using 'plot_dm_snr' program which uses PGPLOT.
#               - Jayanti Prasad, August 18, 2008.
#-------------------------------------------------------#


$nbm=3;
$nthrs=3;
$npulse=100; 
$nthreads=2; 
$prefix="dm";

if ($#ARGV <=0){
    help ();
    exit;
}else{
    $i=0;
    if($ARGV[$i]=~m/-inpdir/){
	$inputdir=$ARGV[++$i];
	$i++;
    } if($ARGV[$i]=~m/-sigdir/){
	$sigprocdir=$ARGV[++$i];
	$i++; 
     } if($ARGV[$i]=~m/-outdir/){
	 $resultdir=$ARGV[++$i]; 
	 $i++;  
     }
    if($ARGV[$i]=~m/-dmf/){
	$dmfilename=$ARGV[++$i];
    }
    
    print"---------INPUT PARAMETERS---------------\n\n\n\n"; 
    print"(1). Input data directory: [$inputdir]\n";
    print"(2). Sigproc directory:  [$sigprocdir]\n";
    print"(3). Result directory: [$resultdir]\n";
    print"(4). dmlist filename:  [$dmfilename]\n";
    print"---------------------------------------------\n\n\n";
     

    print"------------OUTPUT----------------------------\n\n"; 
    print"(1). dedispersed data will be put in:\n";
    print"[$resultdir/dedisp]\n\n";
    print"---------------------------------------------\n\n";
   
    $tmp=`ls $inputdir`;
    @filelist=split(/\n/,$tmp);

    if (!-e "$resultdir"){
          system "mkdir $resultdir";
    } if (!-e "$resultdir/dedisp"){
         system "mkdir $resultdir/dedisp";
    }
    
    foreach $file(@filelist){
        @fnr=split(/[\_,\.]/,$file);
	$beam="$fnr[1]"; 
	
	if (!-e "$resultdir/dedisp/$beam"){
	    system "mkdir $resultdir/dedisp/$beam";
        }
	

	# dedisperse the filterbank data file 
	
        $prefix=$file; 
     	
        $prefix =  $file;
        $prefix =~ s/\.dat//; 

	print "dedispersing for file $file \n"; 

        system"./dedisp_parallel  $file  $dmfilename  $nthreads $prefix"; 
	
        print "file $file has been dedispersed now doing search \n";	

	search();
	
	print "search has been done now matching pulses \n";
	
	print"matching pulses at various dms for beam $beam\n"; 
	
	system"./match_pulse.pl  -inpdir $resultdir/dedisp/$beam -np $npulse  -pfile $beam\_CAND"; 

	print "pulse have been matched \n"; 
	
    }
}

#------------------------------------------------------------


sub search(){
    @dmlist = ();
    open(DMS, "$dmfilename") || die "COULD NOT OPEN DM LIST FILE: $dmfilename\n\n\tExiting...\n\n";
    
    while (<DMS>){
	 chomp($_);
	 @temp = split(/\n/,$_);
	 @temp1 =split(/\t/,$_);
	 
	 foreach $dm(@temp1){
	     
	     $tmfilename="$prefix_DM$dm"; 
	     

	     print "writing $tmpfile for $dm \n";

	     system "$sigprocdir/reader  dedisp.out > dedisp.out.ascii ";
	     
	     $dedispdata=`cat dedisp.out.ascii`;

	     @dedispdata_array=split(/\n/,$dedispdata); 
	     
	     compute_sigma();
	     
	     # removing pulses lower than 3 sigma.
	     
	     if($dm==0){
                 $cut= $nthrs * $sigma;
		 system "./pulse_find.pl -inf dedisp.out.ascii -thr $cut -sig $sigma > $beam\_CAND \n";
	     }  
	     
             system "mv dedisp.out.ascii $dm\_$sigma\_dedisp";
	     
	     system "mv $dm\_$sigma\_dedisp  $resultdir/dedisp/$beam \n";
	     
	     system "rm dedisp.out \n";
	     
	 }
    }
    
}		

#---------------------------------------------------#
# Note that for computing sigma I have used 'meadian' 
# in place of mean. 


sub compute_sigma(){
    
    for($i=0; $i < $#dedispdata_array; $i++){
	@tmp1=split(/ /,$dedispdata_array[$i]);
	$amp[$i]=$tmp1[1];
    }		   
    
    $half=int($#amp/2+0.5);
    
    @amp_1= sort by_number (@amp); 
    $median = $amp_1[$half]; 
    
    for($i=0; $i < $#amp; $i++){
	$amp_2[$i]=abs($amp_1[$i]-$median); 
    }
    
    @amp_3= sort by_number (@amp_2);
    $sigma=$amp_3[$half];
    
}

#----------------------------------------------------#

sub by_number(){
    if($a < $b){
	return -1;
    }elsif($a == $b){
	return 0;
    }elsif($a > $b){
	return 1;
    }
}

  #---------------------------------------------#



sub help(){

    print "----------USE THE FOLLOWING OPTIONS---------- \n\n"; 	
    print " -inpdir: Input data directory  \n";	
    print " -sigdir: SIGPROC directory \n";
    print " -outdir: Results directory \n";
    print " -dmf   : Name of dmlist files\n";
    print "Example:  time perl main.pl -inpdir /data/jayanti/transient_search/inputdata/  -sigdir /usr/local/bin/ -outdir /data/jayanti/tsearch_data/results  -dmf dmlist.dat\n\n";
    print "---------------------------------------------\n\n";
    
}



