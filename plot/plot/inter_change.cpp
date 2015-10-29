#include"stdafx.h"

#include<string>
#include<cstdlib>
#include<regex>


void change_x(std::string&str,float val){
	char tmp[1024]={};
	std::string x="x";
	sprintf_s(tmp,"(%f)",val);
	while(true)   {     
        std::string::size_type pos(0);     
        if(   (pos=str.find(x))!=std::string::npos   )     
            str.replace(pos,x.length(),std::string(tmp));     
        else   break;     
    }     
}

void change_fx(std::string&str){
	std::string sin="sin";
	std::string log="log";
	std::string cos="cos";
	std::string exp="exp";
	std::string sin_t="1&";
	std::string log_t="2&";
	std::string cos_t="3&";
	std::string exp_t="4&";
	while(true){
		std::string::size_type pos(0);     
		if((pos=str.find(sin))!=std::string::npos)     
			str.replace(pos,sin.length(),sin_t);     
		else if((pos=str.find(log))!=std::string::npos)
			str.replace(pos,log.length(),log_t);
		else if((pos=str.find(cos))!=std::string::npos)
			str.replace(pos,cos.length(),cos_t);
		else if((pos=str.find(exp))!=std::string::npos)
			str.replace(pos,exp.length(),exp_t);
		else break;
	}
}
