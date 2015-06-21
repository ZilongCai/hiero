#include"Hypothesis.h"
#include"Util.h"
#include"DataSingleton.h"

Hypothesis::Hypothesis(const string& str_trg, vector<double>& douVec_feats, vector<double>& douVec_weights, const int& i_rule_ID, Hypothesis* p_x1, Hypothesis* p_x2)
{
	this->str_trgRhs_ = str_trg;
       	this->i_apply_ruleID_ = i_rule_ID;
        this->p_apply_hypothesis_x1_ = p_x1;
        this->p_apply_hypothesis_x2_ = p_x2;

        this->douVec_feats_ = douVec_feats;
        
	double dou_sum = 0;
	for(size_t i = 0; i < douVec_feats.size(); i++)
	{
		dou_sum += douVec_feats[i] * douVec_weights[i];
	}
	this->dou_score_ = dou_sum;

        //this->str_left_nwords_;
        //this->str_right_nwords_;		
       	
	string str_tmp;
	vector<string> strVec = Util::split(str_trg);
	
	int i_bound_size = DataSingleton::GetInstance()->p_parameter->GetLanguageModelOrder();
	for(size_t i = 0; i < strVec.size() && i < i_bound_size; i++)
	{
		str_tmp += strVec[i] + " ";
	} 
	
	str_tmp += " ||| ";
	
	for(size_t i = strVec.size()-i_bound_size; i >= 0 && i < strVec.size(); i++)
	{
		str_tmp += strVec[i] + " ";
	}
	
	this->str_key_ = Util::trim(str_tmp);
	
}

Hypothesis::Hypothesis()
{
}

Hypothesis::~Hypothesis()
{
	//PRINT("delete hypo...\n");
	this->douVec_feats_.clear();
}
