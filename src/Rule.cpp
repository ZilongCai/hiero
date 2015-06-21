
#include"Rule.h"
#include"Util.h"

Rule::Rule(const string& ruleLine, const vector<double>& douVec_weights)
{
	string str_delim(" ||| ");
	
	vector<string> strVec = Util::split(ruleLine, str_delim);

	if(5 > strVec.size())
	{
		
			PRINT_ERR(" Rule [" + ruleLine + "] length is not enought \n Check eroor in Rule.cpp\n");
	
	}	

	string str_item_src = Util::trim(strVec[0]);
	this->strVec_srcRhs_ = Util::split(str_item_src);

	string str_item_trg = Util::trim(strVec[1]);
        this->strVec_trgRhs_ = Util::split(str_item_trg);

	string str_item_feats = Util::trim(strVec[3]);
        this->douVec_feats_ = Util::split2double(str_item_feats);
	
	if(douVec_weights.size() != this->douVec_feats_.size())
	{
		PRINT_ERR("weights size is [" + Util::int2string(douVec_weights.size()) + "\n");
		PRINT_ERR("the number of  rule features is [" + Util::int2string(this->douVec_feats_.size()) + "\n");
		PRINT_ERR("the number of  rule features is not equral to weights \n Check error in Rule.cpp\n");
	}
	else
	{
		double dou_tmp = 0;
		for(int i = 0; i  < douVec_weights.size(); i++)
		{
			dou_tmp += douVec_weights[i] * this->douVec_feats_[i];
		}
		this->dou_score_ = dou_tmp;
	}

}

Rule::Rule(const vector<string>& strVec_srcRhs, const vector<string>& strVec_trgRhs, const vector<double>& douVec_feats, const vector<double>& douVec_weights)
{
	this->strVec_srcRhs_ = strVec_srcRhs;
	this->strVec_trgRhs_ = strVec_trgRhs;

	this-> douVec_feats_ =  douVec_feats;
	if(douVec_weights.size() != douVec_feats.size())
	{
                PRINT_ERR("the number of  rule features is not equral to weights \n Check error in Rule.cpp\n");
        }
	else
	{
		double dou_tmp = 0;
                for(int i = 0; i  < douVec_weights.size(); i++)
                {
                        dou_tmp += douVec_weights[i] * this->douVec_feats_[i];
                }
                this->dou_score_ = dou_tmp;
	}	

	
}

Rule::~Rule()
{
	this->strVec_srcRhs_.clear();
	this->strVec_trgRhs_.clear();
	this->douVec_feats_.clear();
}

string Rule::SrcTrg2String()
{
	string ret("");

	for(vector<string>::iterator ite = this->strVec_srcRhs_.begin(); ite != this->strVec_srcRhs_.end(); ite++)
	{
		ret = ret + *ite + " ";
	}
	
	ret += " ||| ";
	for(vector<string>::iterator ite = this->strVec_trgRhs_.begin(); ite != this->strVec_trgRhs_.end(); ite++)
        {
                ret = ret + *ite + " ";
        }
	
	return ret;
}
