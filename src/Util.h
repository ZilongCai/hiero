#ifndef _UTIL_H_
#define _UTIL_H_

#include<vector>
#include<string>
#include<stdlib.h>
#include<sstream>
#include<iostream>
#include<limits>


#define PRINT_ERR(str) std::cerr << str
#define PRINT(str) std::cout << str


const std::string X1 = "$X_1";
const std::string X2 = "$X_2";

const float LOG_E_10 = 2.30258509299405f;

const float MIN_FLOAT = -std::numeric_limits<float>::max();


/*
 * Span(start, end) means [start, end).
 */
typedef std::pair<int,int> Span;

namespace Util
{

	inline std::vector<std::string> split(const std::string& s, std::string delim = " ")
	{
		std::vector<std::string> ret;

		size_t last = 0;
		size_t index = s.find(delim, last);
		while (index!=std::string::npos)
		{
			ret.push_back(s.substr(last, index-last));
			last=index + delim.size();
			index=s.find(delim, last);
		}
		
		if (index-last>0)
		{
			ret.push_back(s.substr(last,index-last));							
		}
	
		return ret;
	}
	
	inline std::vector<double> split2double(const std::string& s, std::string delim = " ")
        {
                std::vector<double> ret;

                size_t last = 0;
                size_t index=s.find(delim,last);
                while (index!=std::string::npos)
                {
                        ret.push_back(atof(s.substr(last,index-last).c_str()));
                        last=index + delim.size();
                        index=s.find(delim,last);
                }

                if (index-last>0)
                {
                        ret.push_back(atof(s.substr(last,index-last).c_str()));
                }

                return ret;
        }


	inline std::string int2string (const int& i)
	{
		std::ostringstream  ostr;
  		ostr << i ;
 		std::string ret(ostr.str());
		ostr.clear();
		return ret;
		
	}
	
	inline std::string double2string (const double& i)
        {
                std::ostringstream  ostr;
                ostr << i ;
                std::string ret(ostr.str());
		ostr.clear();
                return ret;

        }	

	inline std::string trim(std::string str) 
	{
    		std::string::size_type pos = str.find_first_not_of(' ');
    		if (pos == std::string::npos)
    		{
        		return str;
    		}
    
		std::string::size_type pos2 = str.find_last_not_of(' ');
    		if (pos2 != std::string::npos)
    		{
        		return str.substr(pos, pos2 - pos + 1);
    		}
    		return str.substr(pos);
	}
}
#endif
