CX=gcc

all:   readheader readchanel  bndpass bndpass_block rfievents poweraverage  dedisp_serial dedisp_parllel  


readheader:
	$(CX) strings_equal.c read_header.c readheader.c -o readheader 

readchanel:
	$(CX) strings_equal.c read_header.c readchanel.c -o readchanel

bndpass:
	$(CX) strings_equal.c read_header.c bndpass.c -o  bndpass  -lm

bndpass_block:
	$(CX) strings_equal.c read_header.c bndpass_block.c -o bndpass_block  -lm

rfievents :
	$(CX) strings_equal.c read_header.c rfievents.c -o rfievents -lm 

poweraverage :
	$(CX) strings_equal.c read_header.c  poweraverage.c  -o poweraverage -lm 

dedisp_serial:
	$(CX) strings_equal.c dmdelay.c   read_header.c dedisp_serial.c -o dedisp_serial  -lm

dedisp_parllel:
	$(CX) strings_equal.c  dmdelay.c read_header.c dedisp_parallel.c -o  dedisp_parallel  -lm  -fopenmp 

clean:
	rm -rf *~  readheader bndpass bndpass_block  readchanel  dedisp_serial  dedisp_parallel  rfievents poweraverage  
