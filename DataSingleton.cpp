#include"DataSingleton.h"

DataSingleton* DataSingleton::m_pInstance;

DataSingleton::DataSingleton()
{
}

DataSingleton::~DataSingleton()
{
	delete this->p_rule_table_;
	delete this->p_language_model_;
}

void DataSingleton::LoadModel(const string& rule_table_filename, const string& language_model_filename, const vector<double>& douVec_weights, 
			const int& i_rule_limit, const double& dou_rule_threshold, const int& i_language_model_order)
{
	PRINT("DataSingleton is loading data now........\n");
	this->p_rule_table_ = new RuleTable();
	this->p_rule_table_->Load(rule_table_filename, douVec_weights, i_rule_limit, dou_rule_threshold);
	
	this->p_language_model_ = new LanguageModel(i_language_model_order, language_model_filename);
	this->p_language_model_->Load();
}
