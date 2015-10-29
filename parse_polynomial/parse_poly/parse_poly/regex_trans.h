
#ifndef REGEX_TRANS
#define REGEX_TRANS

extern int stack_transform(char*stack,int begin,int end,int&stack_pos);
extern int num_transform(char*stack,int begin,int end,int&stack_pos);

extern float pop_N();
extern void push_N(int n);



#endif