#ifndef _HYPOTHESIS_H
#define _HYPOTHESIS_H

#include<vector>
#include<string>

using namespace std;

class Hypothesis
{
public:
	Hypothesis();
	Hypothesis(const string& str_trg, vector<double>& douVec_feats, vector<double>& douVec_weights, const int& i_rule_ID, Hypothesis* p_x1, Hypothesis* p_x2);
	~Hypothesis();
	
	string GetTrg() const
		{return this->str_trgRhs_;}

	vector<double> GetFeats() const
		{return this->douVec_feats_;}
	
	double GetScore()
		{return this->dou_score_;};
		
	string GetLeftNWords() const
		{return this->str_left_nwords_;}

	string GetRightNWords() const
		{return this->str_right_nwords_;}

	bool operator < (const Hypothesis hypo) const 
		{return this->dou_score_ < hypo.dou_score_;}	

	bool operator > (const Hypothesis hypo) const
                {return this->dou_score_ > hypo.dou_score_;}

	Hypothesis* GetHypoX1() const 
		{return this->p_apply_hypothesis_x1_;}
	
	Hypothesis* GetHypoX2() const 
		{return  this->p_apply_hypothesis_x2_;}

	int GetAppliedRuleID() const
		{return this->i_apply_ruleID_;}
	
	string GetKey() const
		{return this->str_key_;}
private:
	
	string str_trgRhs_;
	/*	
 	*  for deviations
	*/		
	int i_apply_ruleID_;
	Hypothesis* p_apply_hypothesis_x1_; 
	Hypothesis* p_apply_hypothesis_x2_; 
	
	vector<double> douVec_feats_;
	double dou_score_;

	//for hypothesis merging
	string str_key_;	
	string str_left_nwords_;
	string str_right_nwords_; 
};


#endif
