
#include<regex>
#include<stack>
#include<map>

#define INF 99999

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


//std::regex pattern_num("^(-?\\d*|-?\\d*.\\d*)$");
//std::regex pattern_ops("^N(+|-|*|/|\\^)N$");
std::stack<int> N_stack;
std::map<char,int> sigmap;

