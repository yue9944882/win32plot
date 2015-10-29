
#include<string>
#include"global_val.h"
#include"regex_trans.h"
#include"VT_func.h"


float parse_poly(std::string expr){
	
	sigmap_init();

	char*stack_buf=new char[1024];
	
	memset(stack_buf,0,1024);

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

		if(sigmap[stack_buf[stack_pos-1]]==0||a=='.'){
			if(sigmap[a]!=0&&a!='.'){
				int p=stack_pos-1;
				do{
					p--;
				}while(sigmap[stack_buf[p]]==0||stack_buf[p]=='.');
			
				num_transform(stack_buf,p+1,stack_pos-1,stack_pos);
			}else{
				stack_buf[stack_pos]=a;
				stack_pos++;
				continue;
			}
		}


		if(stack_buf[stack_pos-1]!='N'){ j=stack_pos-1; }
		else{ j=stack_pos-2; }
		
		char q;
		while(1==VT_cmp(stack_buf[j],a)){

			if(stack_buf[stack_pos-1]!='N'){ j=stack_pos-1; }
			else{ j=stack_pos-2; }
			int bf=0;
			do{
				q=stack_buf[j];
			
				if(stack_buf[j-1]!='N'){ j=j-1; }
				else{ j=j-2; }
			
				if(j<0){
					j=0;
					bf=1;
					break;
				}
				if(bf)break;
			}while(VT_cmp(stack_buf[j],q)==1||VT_cmp(stack_buf[j],q)==0);
			
			///	Parse into number
			
			int r=stack_transform(stack_buf,j+1,stack_pos-1,stack_pos);
			if(r==-1)break;
		}
		
		if(VT_cmp(stack_buf[j],a)==-1||VT_cmp(stack_buf[j],a)==0){
			stack_buf[stack_pos++]=a;
		}else{
			exit(0);
		}
		
	}while(stack_buf[stack_pos-1]!='#');	
	
	delete []stack_buf;

	return pop_N();

}
 
 
