#include"stdafx.h"



#include"global_val.h"

#include<map>


void sigmap_init(){
	char c0='0'; 
	for(;c0<='9';c0++){
		sigmap.insert(std::pair<char,int>(c0,0));
	}
	sigmap.insert(std::pair<char,int>('+',1));
	sigmap.insert(std::pair<char,int>('-',2));
	sigmap.insert(std::pair<char,int>('*',3));
	sigmap.insert(std::pair<char,int>('/',4));
	sigmap.insert(std::pair<char,int>('.',5));
	sigmap.insert(std::pair<char,int>('(',6));
	sigmap.insert(std::pair<char,int>(')',7));
	sigmap.insert(std::pair<char,int>('#',8));
	sigmap.insert(std::pair<char,int>('^',9));
	sigmap.insert(std::pair<char,int>('&',10));
} 


 
int VT_cmp(char c1,char c2){
	int i1=sigmap[c1];
	int i2=sigmap[c2];
	
	return cmp_prio[i1][i2];
}


