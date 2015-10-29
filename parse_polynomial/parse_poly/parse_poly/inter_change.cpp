

#include<string>
#include<cstdlib>

void change_x(std::string&str,float val){
	char tmp[1024]={};
	std::string x="x";
	sprintf(tmp,"(%f)",val);
	while(true)   {     
        std::string::size_type pos(0);     
        if(   (pos=str.find(x))!=std::string::npos   )     
            str.replace(pos,x.length(),std::string(tmp));     
        else   break;     
    }     
}
