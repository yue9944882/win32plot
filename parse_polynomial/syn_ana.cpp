
#include"TypeDef.h"

#include<string>
#include<stack>
#include<vector>

#include<iterator>
#include<iostream>
#include<cctype>
#include<cstdlib>
#include<stack>
#include<map>


#define INF 9999

// Module for Polynomial Analysis


//** Function: Syntax Tree Building
//** Input : String type polynomial
//** Output: A binary tree describing the structure of polynomial

void sigmap_init();
int VT_cmp(char c1,char c2);
void stack_transform(char*stack,int begin,int end);
int parse_poly(std::string expr);

char str[100]="f(x)=1+x";

std::map<char,int> sigmap;
char*sptr=str;

 // VT-camparision GRAPH
 // -1 for < , +1 for > , 0 for equal
 // 
 //		\d	 +  -   * 	 / 	 .  ( 	 )  # 	 ^
 //	\d	-1  +1	+1	+1	+1	-1	-1 	+1	+1	+1 
 // + 	-1 	+1	+1	-1	-1 		-1 	+1	+1 	-1 
 //	- 	-1	+1	+1	-1	-1 		-1 	+1	+1 	-1 
 // * 	-1	+1	+1	+1	+1 		-1 	+1	+1 	-1 
 //	/ 	-1	+1	+1	+1	+1 		-1 	+1	+1 	-1 
 //	. 	-1							
 //	(	-1	-1	-1	-1	-1 		-1 	0 	 	-1 
 //	)	+1	+1	+1	+1	+1 		 	+1 	+1 	+1 
 //	#	-1	-1	-1	-1 	-1 		-1 	 	0 	-1 
 // ^	-1	+1 	+1	+1 	+1 		-1	+1 	+1 	-1 

int cmp_prio[10][10]={
{-1,+1,+1,+1,+1,-1,-1,+1,+1,+1},
{-1,+1,+1,-1,-1,INF,-1,+1,+1,-1},
{-1,+1,+1,-1,-1,INF,-1,+1,+1,-1},
{-1,+1,+1,+1,+1,INF,-1,+1,+1,-1},
{-1,+1,+1,+1,+1,INF,-1,+1,+1,-1},
{-1,INF,INF,INF,INF,INF,INF,INF,INF,INF},
{-1,-1,-1,-1,-1,INF,-1,0,INF,-1},
{+1,+1,+1,+1,+1,INF,INF,+1,+1,+1},
{-1,-1,-1,-1,-1,INF,-1,INF,0,-1},
{-1,+1,+1,+1,+1,INF,-1,+1,+1,-1}
};


int main(){
	
	while((*sptr)!='='){
		sptr++;
	}sptr++;
	///Until the assignment operation then syntax analysis begin
	
	std::string expr(sptr);
	
	sigmap_init();
	
	parse_poly(expr);
	
	std::cout<<"Expression After:"+expr<<std::endl;
	
	system("pause");
	
	return 0;
}


int parse_poly(std::string expr){
	
	char*stack_buf=new char[1024];
	
	expr=expr+"#";
	
	int stack_pos=0;
	int input_pos=0;
	
	int expr_len=0;
	const char*expr_ptr=expr.c_str();
	int expr_pos=0;
	
	//Pushing '#' into stack
	stack_buf[stack_pos]='#';
	stack_pos++;
	
	 int j;
	
	do{
		
		char a=expr_ptr[expr_pos++];
		
		if(stack_buf[stack_pos-1]!='N'){ j=stack_pos-1; }
		else{ j=stack_pos-2; }
		
		char q;
		while(1==VT_cmp(stack_buf[j],a)){
			
			do{
				q=stack_buf[j];
				if(stack_buf[j-1]=='N'){ j=j-1; }
				else{ j=j-2; }
			}while(VT_cmp(stack_buf[j],q)==-1);
			
			///	Parse into number
			
			stack_transform(stack_buf,j+1,stack_pos-1);
			
		}
		
		if(VT_cmp(stack_buf[j],a)==-1||VT_cmp(stack_buf[j],a)==0){
			stack_buf[stack_pos++]=a;
		}else{
			exit(0);
		}
		
	}while(stack_buf[stack_pos]!='#');	
	
	delete []stack_buf;

}
 
 
void stack_transform(char*stack,int begin,int end){
	for(int i=begin;i<end;i++){
		putchar(stack[i]);
	}printf("\n");
}


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
} 


/* 
void prio_init(){

}
*/ 
 
int VT_cmp(char c1,char c2){
	int i1=sigmap[c1];
	int i2=sigmap[c2];
	
	return cmp_prio[i1][i2];
}

