#!/usr/bin/perl
#--------------------------------------------------------------
# This programs finsd pulses above a given threshold at give DM.
# At present it is used to find pulsed above a given threshold at DM=0.
# Pulsed parameters are as follows: starting and ending times of pulse,
# mean pulse location, number of data points in the pulse, SNR. 

if($#ARGV <0){
    exit;
} else {
    $i=0; 
    if ($ARGV[$i]=~m/-inf/){
	$inputfile=$ARGV[++$i];
	$i++;
    } if($ARGV[$i]=~m/-thr/){
	$cut=$ARGV[++$i]; 
	$i++;
    } if($ARGV[$i]=~m/-sig/){
	$sigma=$ARGV[++$i];
    }	
    
}


open(INP,$inputfile) ||  die 'could not open file';

$i=0;
while (<INP>){
    chomp($_);
    $data[$i]=$_;
    $i++;
}

$j=0;
$k=0;

for($i=0; $i <$#data+1; $i++){
    @tmp_a=split(/ /,$data[$i]); 
    @tmp_b=split(/ /,$data[$i+1]);
    
    if($tmp_a[1] <=  $cut && $tmp_b[1]  > $cut){
	$begin[$j] = $i+1;
	$tbegin[$j]= $tmp_b[0]; 
	$j++; 
    } 
    
    if($tmp_a[1] > $cut && $tmp_b[1]  <= $cut){
	$end[$k] = $i;
	$tend[$k]=$tmp_a[0];
	$k++;
    }
    
}


@amp=(); 

for($j=0; $j < $#begin+1; $j++){
    $amp[$j]=0;
    $amptime=0;
    
    for($i1=$begin[$j]; $i1 < $end[$j]+1; $i1++){
	@tmp=split(/ /,$data[$i1]);
	$amp[$j]=$amp[$j]+$tmp[1];
	$amptime=$amptime+$tmp[0]*$tmp[1];  
    }
    
    $time=$amptime/$amp[$j];  
    $snr=$amp[$j]/$sigma;   
    $npulse=$end[$j]-$begin[$j] ;   
    
    printf("%2.6f %2.6f %2.6f %2.6f %d\n",$time,$snr,$tbegin[$j],$tend[$j],$npulse);
}







