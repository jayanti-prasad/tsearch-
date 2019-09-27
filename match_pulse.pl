#!/usr/bin/perl
# This program computes the SNR of pulses identified at DM=0 at
# other DMS. It takes starting and ending time of pulses identified 
# at DM=0 and computes their SNR at other DMS.

if($#ARGV <=0){
    help();
    exit; 
}else{
    $i=0;
    if($ARGV[$i]=~m/-inpdir/){
	$inputdir=$ARGV[++$i];
	$i++;
    }if($ARGV[$i]=~m/-outdir/){
	$outdir=$ARGV[++$i];
	$i++
    }if($ARGV[$i]=~m/-np/){
	$npulse=$ARGV[++$i]; 
     $i++; 
    } if($ARGV[$i]=~m/-pfile/){
       $pulsefile=$ARGV[++$i];
    }	    

    @tmp=split(/\_/,$pulsefile);
    $beam=$tmp[0]; 

    print"matching for beam $beam\n";
    
    @pp=split(/\//,$inputdir);
    pop @pp;
    pop @pp;
    $new="dmsnr";
    push(@pp,$new);
    $resultdir_base=join("/",@pp);
    
    if(!-e $resultdir_base){
	system"mkdir $resultdir_base";
    }
    
    push(@pp,$beam);
    $resultdir_final=join("/",@pp);
    
    if(!-e $resultdir_final){
	system "mkdir $resultdir_final";
    }

    #detect the giant pulses at dm=0.

    print"results will be put in: $resultdir_final\n"; 

    open(INP,"$pulsefile") || die ("could not open file",$pulsefile);
    @raw_data=<INP>;
    
    for($j=0; $j <=$#raw_data; $j++){
	@line=split(/ /,$raw_data[$j]);
	$raw_data_21[$j]=join(" ",$line[1],$line[0],$line[2],$line[3]);
    }
    
    @raw_data_21_sort= sort by_number @raw_data_21;
    @raw_data_21_sort_des=reverse @raw_data_21_sort;
    
    for($j=0; $j < $npulse; $j++){
	@tmp=split(/ /,$raw_data_21_sort_des[$j]);
	$pulse_start[$j]=$tmp[2];
	$pulse_end[$j] =$tmp[3];
	$pulse_time[$j]=$tmp[1];   
    }
    
    #now search the same pulses (which are characterized by the start and end time)
    #at various dm from the pulses which are above 3 sigma. 

    $data=`ls $inputdir`;
    @dataarray=split(/\n/,$data);

    for($j=0; $j < $npulse; $j++){
	open(DMF,">$pulse_time[$j].PULSE");
	
        print"matching pulse $j \n";
	
	for($i=0; $i <$#dataarray; $i++){
	    @filename=split(/\_/,$dataarray[$i]);
	    $dm=$filename[0];
	    $sigma=$filename[1];
	    
	    open(INP,"$inputdir/$dataarray[$i]") || die ("could not open file",$inputdir/$dataarray[$i]);
	    
	    @raw_data=<INP>; 
	    
	    $first=$raw_data[0];
	    chomp $first;
	    @first_array=split(/ /,$first);
            $last=$raw_data[$#raw_data]; 
            chomp $last;
            @last_array=split(/ /,$last);
	    $dt=($last_array[0]-$first_array[0])/($#raw_data);

            $j1=int(($pulse_start[$j]-$first_array[0])/$dt)-10;
            $j2=int(($pulse_end[$j]-$first_array[0])/$dt)+10;

	    $tt1=$raw_data[$j1];
	    chomp $tt1;
	    $tt2=$raw_data[$j2];
	    chomp $tt2;

	    @tt1_array=split(/ /,$tt1);
	    @tt2_array=split(/ /,$tt2);

	    $time_1=$tt1_array[0];
	   $time_2=$tt2_array[0]; 
	 
	   $t1=$pulse_start[$j];
	    $t2= $pulse_end[$j]; 
	    
	    if($time_1 > $t1 || $time_2 <  $t2){
             print"Error !SEARCH LIMITS:[$time_1,$time_2],PULSE:[$t1,$t2]\n";
	     exit;
    	    }
	    
	    $amp[$i]=0;
            for($k=$j1; $k <=$j2; $k++){
		$tt=$raw_data[$k];
		chomp $tt;
		@tt_array=split(/ /,$tt);
		$time=$tt_array[0];

		if($time > $pulse_start[$j] && $time < $pulse_end[$j]){
		    $amp[$i]=$amp[$i]+$tt_array[1]; 
		}
		
	    }
	    $snr[$i]=$amp[$i]/$sigma;
	    
	    $dm_snr[$i]=join(" ",$dm,$snr[$i]); 
	    
	}

	@dm_snr_srt=sort by_number @dm_snr; 
	
	for($i=0;$i < $#dm_snr_srt; $i++){
	    @tmp_array=split(/ /,$dm_snr_srt[$i]);
	    printf(DMF "%2.6f %2.6f\n",$tmp_array[0],$tmp_array[1]);
	}
       system"mv $pulse_time[$j].PULSE $resultdir_final \n";    
    }
    
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
    print "-inpdir: input directory\n";
    print "-np:number of giant pulese\n";
    print "-pfile: pulse file \n";  
}


