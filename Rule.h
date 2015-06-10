#ifndef _RULE_H_
#define _RULE_H_
#include<vector>
#include<string>

using namespace std;

/*
 * @beief:
 * The operator overloading and the private member dou_score_ are used for rule pruning. 
 * Implementation of rule pruning is at class RuleTable. 
 *
 * @date: 2015-6-9
 */


class Rule
{
public:
	/* for the general rules*/
	Rule(string& ruleLine, vector<double> douVec_weights);	

	/* for the gule rules*/
	Rule(string& str_srcRhs, string& str_trgRhs, vector<double>& douVec_feats, vector<double> douVec_weights);

	~Rule(void);

	const vector<string> GetSrcRhs() const
		{return this->strVec_srcRhs_;}
	
	const vector<string> GetTrgRhs() const
		{return this->strVec_trgRhs_;}
	
	const vector<double> GetFeats() const
		{return this->douVec_feats_;}
		
	double GetScore() const
		{return this->dou_score_;}

	int GetType() const
		{return this->i_type_;}

	bool operator < (const Rule& r) const
		{return this->dou_score_ < r.dou_score_;}
	
	bool operator > (const Rule& r) const
		{return this->dou_score_ > r.dou_score_;}

	string SrcTrg2String();

private:
	vector<string> strVec_srcRhs_;  
	vector<string> strVec_trgRhs_;
	
	/*
		4 features : bidirectional translation probability and  bidirectional lexical translation probability
	*/
	vector<double> douVec_feats_;  
	double dou_score_;
	int i_type_;
	
	/*
 	  -1 for glue rules, -2 for OOV rules.
 	*/
	//int i_global_rule_ID;
};

#endif
