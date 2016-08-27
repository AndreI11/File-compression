#include <iostream>
#include <fstream>
#include <iterator>
#include <cstdlib>
#include <unordered_map>
#include <queue>
#include <time.h>


struct params
{
    unsigned char N;
    unsigned char L;
    unsigned char S;
    int W;
    int F;
    int maxLit;
};

unsigned int getNBits(int n,std::istream_iterator<unsigned char>* eos, std::istream_iterator<unsigned char> * iit){
	static unsigned int buffer;
	static int buffSize =0;
	
	unsigned int returnVal = buffer;
	unsigned int mask;
	if (buffSize>=n){
		mask = ((1 << n)-1) << 16-n;
		returnVal &= mask; 
		returnVal = returnVal >> (16-n);
		buffer = buffer << n; 
		buffSize -= n;
	}
	else{
		unsigned int nextByte =*((*iit)++);
		unsigned int nextByte2 = *((*iit)++);
		unsigned int temp = buffer;
		temp = temp >> 16-n;
		nextByte = nextByte << 8;
		buffer = nextByte;
		buffer |= nextByte2;
		mask = ((1 << (n-buffSize))-1)<< 16-(n-buffSize);
		returnVal = buffer;
		returnVal &= mask;
		returnVal = returnVal >> 16-n+buffSize;
		returnVal |= temp; 
		buffer = buffer << 16-n+buffSize;
		buffSize = 16-n-buffSize;
	}
	return returnVal;

}



int main(int argc, const char * argv[]){
	std::string s = "";
    int i;
    static std::ifstream input(argv[1]);
    static std::istream_iterator<unsigned char> eos;
    static std::istream_iterator<unsigned char> iit(input);

    struct params p;
    p.N = *(iit++);
    p.L = *(iit++);
    p.S = *(iit++);
    p.F = (1 << p.L) - 1;
    p.W = 1 << p.N;
    p.maxLit = (1 << p.S) - 1; 

unsigned char mask= 0;
    unsigned char buffer = 0;
    int buffPos = 0;
    int offSet = 0;
    int offPos = 0;
    int state =0;


	char window[p.W];    
    int startLB = 0; // index of the first char of the lookahead buffer

int lengthOfMatch, readyToRemove = 0, splitString = 0;
    
    int numMatched = 0;

     while (1) {
		int len = getNBits(p.L,&eos,&iit);
		int off;
		if (!len){
			off = getNBits(p.N, &eos,&iit);
			
			for (int i =0; i< len+1;i++){
				std::cout << window[off+i];
				window[startLB++ % p.W] = window[off+i];
			}
		}
		else{
			int numChars = getNBits(p.S, &eos,&iit);
			if (numChars == 0){
				break;
			}
			for (int i =0; i< numChars; i++){
				char c = getNBits(8, &eos, &iit);
				std::cout << c;
				window[startLB++ % p.W] = c;
			}

		}




}
}
*/
    /*
    while(iit != eos){
  
    	unsigned char tempOffSet;
    	if (state ==0){
	    	mask = ((1 << p.L)-1) << 8-p.L;
	    	unsigned char len = mask & nextByte;
	    	len = len >> (8-p.L);
	    	if (len != 0){
	    		mask = (1 << (8-p.L))-1;
	    		tempOffSet = mask & nextByte;
	    		offSet = offSet | tempOffSet;
	    		offSet = offSet << p.N-(8-p.L);
	    		offPos += 8-p.L;
	        	state = 1;

	    	}
	    	else{
	    		state = 2;

			}

	    }
	    else if (state == 1){
	    	if (p.N-offPos <= 8){
	    		mask = ((1 << p.N-offPos)-1) << 8-(p.N-offPos);
				tempOffSet = mask & nextByte;
				tempOffSet = tempOffSet >> 8-(p.N-offPos);
				offSet |= tempOffSet;				
	    		mask = (1 << 8-( p.N-offPos))-1;
	    		nextByte &= mask;
	    		buffer = nextByte;
	    		buffPos = 8-(p.N-offPos);
	    		state =0;
	    		offPos = 0;
	    	}
	    	else{
	    		int tempInt = nextByte;
	    		tempInt = tempInt << p.N-offPos-8;
	    		offSet |= tempInt;
	    		offPos += 8;
	    	}

	    }

    

    }
    
