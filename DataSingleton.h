#ifndef _DATASINGLETON_H
#define _DATASINGLETON_H

#include"RuleTable.h"
#include"LanguageModel.h"
#include"Util.h"
#include"Parameter.h"

class DataSingleton
{
public:
	~DataSingleton();
	static DataSingleton* GetInstance()
	{
		if(m_pInstance == NULL)    //important
		{
            		m_pInstance = new DataSingleton(); 
		}
		return m_pInstance;
	}

	
	static void LoadStaticModel(const string& rule_table_filename, const string& language_model_filename, const vector<double>& douVec_weights, const int& i_rule_limit, const double& dou_rule_threshold, const int& i_language_model_order)
	{
		m_pInstance->LoadModel(rule_table_filename, language_model_filename, douVec_weights,  i_rule_limit, dou_rule_threshold, i_language_model_order);
	}

	void LoadModel(const string& rule_table_filename, const string& language_model_filename, const vector<double>& douVec_weights, const int& i_rule_limit, const double& dou_rule_threshold, const int& i_language_model_order);


	RuleTable *p_rule_table_;
	LanguageModel *p_language_model_;
	Parameter *p_parameter;

private:
	DataSingleton();
	static DataSingleton* m_pInstance;
};

#endif


