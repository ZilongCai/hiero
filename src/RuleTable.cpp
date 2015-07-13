
#include<fstream>
#include<algorithm>
#include <cmath>
#include"RuleTable.h"
#include"Util.h"

RuleTable::RuleTable(void)
:i_rule_limit_(100),dou_rule_threshold_(-1)
{
}

RuleTable::~RuleTable(void)
{
	this->vec_rule_table_.clear();
	this->map_ruleID_.clear();
}


string RuleTable::ExtractSrcFromTableItem (const string line) const
{
	string str_delim(" ||| ");
	size_t last = 0;
        size_t index= line.find(str_delim, last);	
	if(index == std::string::npos)
	{
		PRINT_ERR("Can't extract src part from [" << line << "]\n Check error in RuleTable.cpp\n");
	}
	else
	{
		return Util::trim(line.substr(last,index-last));
	}
}
 
Rule RuleTable::GetRule(const int& i_rule_ID) const
{
	if(i_rule_ID < (int)this->vec_rule_table_.size())
	{
		return this->vec_rule_table_[i_rule_ID];
	}	
	else
	{
		PRINT_ERR("invalid rule ID ["  << i_rule_ID << "]\n Check error in RuleTable.cpp\n");
	}
}



void RuleTable::UpdateRuleTable(const string& str_src, vector<Rule>& vec_rule)
{
	/* for debug  2015-6-9
	PRINT("Updata Rule Table --\n");
	PRINT(str_src + "\n");
	PRINT(Util::int2string(vec_rule.size()) + "raw . \n");
	*/
	sort(vec_rule.begin(), vec_rule.end(), greater<Rule>());

	/* for debug  2015-6-9
	PRINT(Util::double2string(vec_rule[0].GetScore()) + "\n");
	PRINT(Util::double2string(vec_rule[1].GetScore()) + "\n");
	*/
	
	double dou_min_score = vec_rule[0].GetScore() + log(this->dou_rule_threshold_);
	vector<int> vec_ruleID_tmp(this->i_rule_limit_, -1);

	int i_rule_ID = (int)this->vec_rule_table_.size();
	int i_count = 0;
	for(int i = 0; i < ((int)vec_rule.size() < this->i_rule_limit_?(int)vec_rule.size():this->i_rule_limit_); i++)  //rule_limit_
	{
		if(dou_min_score <= vec_rule[i].GetScore()) //rule_threshold
		{
			this->vec_rule_table_.push_back(vec_rule[i]);
			vec_ruleID_tmp[i] = i_rule_ID + i;
			i_count++;
		}
		else
		{	
			break;
		}
	}
	
	vec_ruleID_tmp.resize(i_count); //remove -1 
	
	/* for debug  2015-6-9
	PRINT(Util::int2string(i_count) + "\n");	
	PRINT(Util::int2string(this->vec_rule_table_.size()) + " total \n");*/

	this->map_ruleID_.insert(pair<string, vector<int> >(str_src, vec_ruleID_tmp));

}

void RuleTable::Load(const string& file_name, const vector<double>& douVec_weights, const int& i_rule_limit, const double& dou_rule_threshold)
{
	PRINT("Loading rule table [" << file_name << "]...\n");
	this->i_rule_limit_ = i_rule_limit;
	this->dou_rule_threshold_ = dou_rule_threshold;

	clock_t start, finish;
	start = clock();

	ifstream fin;
	fin.open(file_name.c_str(), ios::in);

	string line;
	int i_ruleID = 0; //from 0
	
	string str_last_src, str_current_src;
	vector<Rule> vec_tmp_Rule;

	while(getline(fin,line))
	{
		str_current_src = ExtractSrcFromTableItem(line);
		Rule r = Rule(line, douVec_weights);

		if(str_last_src.empty())
		{
			str_last_src = str_current_src;
			vec_tmp_Rule.push_back(r);
		}
		else
		{
			if(str_last_src == str_current_src)
			{
				vec_tmp_Rule.push_back(r);
			}
			else
			{
				UpdateRuleTable(str_last_src, vec_tmp_Rule);
				vec_tmp_Rule.clear();
				str_last_src = str_current_src;
				vec_tmp_Rule.push_back(r);
			}
		}

	}

	if(!vec_tmp_Rule.empty())
	{
		UpdateRuleTable(str_current_src, vec_tmp_Rule);
		vec_tmp_Rule.clear();
	}
	
	fin.close();

	/* for glue rules 
 	 * glue rule ID is vec_rule_table_.size()-1.
 	 */
	string str_src;
	vector<string> strVec_glue_srcRhs;
	vector<string> strVec_glue_trgRhs;
	vector<Rule> vec_rule;

	//glue rules for head and tail. we do this as moses
	//for head <s>
	strVec_glue_srcRhs.push_back("<s>"); strVec_glue_srcRhs.push_back(X1);
	strVec_glue_trgRhs.push_back("<s>"); strVec_glue_trgRhs.push_back(X1);
	vector<double> douVec_feats(douVec_weights.size(), 0);
        Rule r1 = Rule(strVec_glue_srcRhs, strVec_glue_trgRhs, douVec_feats, douVec_weights);	
	str_src = "<s> $X_1";
	vec_rule.push_back(r1);
	UpdateRuleTable(str_src, vec_rule);
	//this->vec_rule_table_.push_back(r1);
	strVec_glue_srcRhs.clear(); strVec_glue_trgRhs.clear(); vec_rule.clear();
	cout << r1.SrcTrg2String() << endl;
	

	//for tail </s>
	strVec_glue_srcRhs.push_back(X1); strVec_glue_srcRhs.push_back("</s>");
        strVec_glue_trgRhs.push_back(X1); strVec_glue_trgRhs.push_back("</s>");
        Rule r2 = Rule(strVec_glue_srcRhs, strVec_glue_trgRhs, douVec_feats, douVec_weights);	
	str_src = "$X_1 </s>";	
	vec_rule.push_back(r2);
	UpdateRuleTable(str_src, vec_rule);
	//this->vec_rule_table_.push_back(r2);
	strVec_glue_srcRhs.clear(); strVec_glue_trgRhs.clear(); vec_rule.clear();
	cout << r2.SrcTrg2String() << endl;


	strVec_glue_srcRhs.push_back(X1); strVec_glue_srcRhs.push_back(X2);
	strVec_glue_trgRhs.push_back(X1); strVec_glue_trgRhs.push_back(X2);
	Rule r3 = Rule(strVec_glue_srcRhs, strVec_glue_trgRhs, douVec_feats, douVec_weights);
	this->vec_rule_table_.push_back(r3);
	cout << r3.SrcTrg2String() << endl;

	finish = clock(); 
	double dou_duration = (finish - start)/CLOCKS_PER_SEC;
	PRINT("Loading rule table is done \n time is [" << dou_duration << "] secods\n"
	        << "loaded rules size is [" << (int)this->vec_rule_table_.size() << "]\n");
}


bool RuleTable::MatchRule(const string& str_src, vector<int>& vec_match_ruleID)
{
	map<string, vector<int> >::iterator ite = this->map_ruleID_.find(str_src);
	
	if(ite != this->map_ruleID_.end())
	{
		vec_match_ruleID = ite->second;
		return true;
	}
	else
	{
		return false;
	}
}

Rule  RuleTable::GenerateRule4OOV(const string& str, const int& i_feat_number)
{
	vector<string> strVec_strRhs(1, str);
	vector<string> strVec_trgRhs(1, str);
	
	//PRINT("OOV feature number is " << i_feat_number << endl);
	//we penalizie the oov rules with scores -4
	vector<double> douVec_feats(i_feat_number, -1);
	vector<double> douVec_weights(i_feat_number, 1);

	Rule r = Rule(strVec_strRhs, strVec_trgRhs, douVec_feats, douVec_weights);
	
	return r;
	
}
