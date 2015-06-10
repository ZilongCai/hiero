
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
	}
	
	vec_ruleID_tmp.resize(i_count); //remove -1 
	
	/* for debug  2015-6-9
	PRINT(Util::int2string(i_count) + "\n");	
	PRINT(Util::int2string(this->vec_rule_table_.size()) + " total \n");*/

	this->map_ruleID_.insert(pair<string, vector<int> >(str_src, vec_ruleID_tmp));

}

void RuleTable::Load(const string file_name, vector<double>& douVec_weights, int i_rule_limit, double dou_rule_threshold)
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
		//PRINT(line);
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

	/* for glue rules */
	

	finish = clock(); 
	double dou_duration = (finish - start)/CLOCKS_PER_SEC;
	PRINT("Loading rule table is done \n time is [" << dou_duration << "] secods\n"
	        << "loaded rules size is [" << (int)this->vec_rule_table_.size() << "]\n");
}
