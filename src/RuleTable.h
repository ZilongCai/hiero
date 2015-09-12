#ifndef _RULETABLE_H_
#define _RULETABLE_H_
#include<vector>
#include<string>
#include<map>
#include <time.h> 
#include"Rule.h"

using namespace std;

/*
 * @beief: 
 * 1. Rule pruning is used in function UpdateRuleTable. i_rule_limit and dou_rule_threshold are used in rule pruning. Only 4 features which appears in 
 *    rule table are utilized to prune rules.
 * 2. We use a map to retrieve rules. The key is a soruce side span(termial or nontermial) and the value is a vector which contains the IDs of rules. 
 *
 * @date: 2015-6-9
 */



class RuleTable
{
public:
	RuleTable(void);
	~RuleTable(void);
	
	void Load(const string& file_name, const vector<double>& douVec_weights, const int& i_rule_limit, const double& dou_rule_threshold);
	
	string ExtractSrcFromTableItem (const string line) const;
	
	/**
	 *  @brief: Store the rules with the same source side. First, we prune rules according to their scores. Then, we store them in vec_rule_vec_ and 
	 *  collect their indexes. At last, we put the soruce side(key) and the collected indexes(value) to map_ruleID_.  
 	 *  @param[in] str_src: source side span
 	 *  @param[in] vec_rule: Rules to add
 	 *  @return: void
 	 */
	void UpdateRuleTable(const string& str_src, vector<Rule>& vec_rule);

	/**
 	*  @brief: Use oov word to generate rule. the source and target string are the oov word.
 	*  @param[in] str: oov word
 	*  @param[in] i_feat_number: number of rule features
 	*  @return: oov rule
 	*/ 

	Rule GenerateRule4OOV(const string& str, const int& i_feat_number); 
	
	Rule GetRule(const int& i_rule_ID) const;

	/**
 	*  @brief: Using string to search appropriate rules from rule table. 
 	*  @param[in] str_src: source string to search
 	*  @param[in] vec_match_ruleID: the index of matched rules.
 	*  @param[out]: true-> has find appropriate rules   false-> find nothing
 	**/	 
	bool MatchRule(const string& str_src, vector<int>& vec_match_ruleID);
	
	size_t GetRuleTableSize()
	{
		return this->vec_rule_table_.size();
	}

private:
	vector<Rule> vec_rule_table_;
	map<string, vector<int> > map_ruleID_;
	int i_rule_limit_;
	double dou_rule_threshold_;
	vector<double> douVec_weights_;
};

#endif
