
#ifndef GLOBAL_VAL
#define GLOBAL_VAL

#include<regex>
#include<stack>
#include<map>

#define INF 99999

 // VT-camparision GRAPH
 // -1 for < , +1 for > , 0 for equal
 // 
 //		\d	 +  -   * 	 / 	 .  ( 	 )  # 	 ^	&
 //	\d	-1  +1	+1	+1	+1	-1	    +1	+1	+1	+1
 // + 	-1 	+1	+1	-1	-1 		-1 	+1	+1 	-1	-1
 //	- 	-1	+1	+1	-1	-1 		-1 	+1	+1 	-1	-1
 // * 	-1	+1	+1	+1	+1 		-1 	+1	+1 	-1	-1
 //	/ 	-1	+1	+1	+1	+1 		-1 	+1	+1 	-1	-1
 //	. 	-1							
 //	(	-1	-1	-1	-1	-1 		-1 	0 	 	-1	-1
 //	)	+1	+1	+1	+1	+1 		 	+1 	+1 	+1	+1
 //	#	-1	-1	-1	-1 	-1 		-1 	 	0 	-1	-1
 // ^	-1	+1 	+1	+1 	+1 		-1	+1 	+1 	-1	-1
 //	&	-1	+1	+1	+1	+1		-1	+1	+1	+1	-1

extern int cmp_prio[11][11];


//extern std::regex pattern_num;
//extern std::regex pattern_ops;

extern std::stack<float> N_stack;
extern std::map<char,int> sigmap;




#endif