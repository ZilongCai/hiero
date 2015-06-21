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


void DataSingleton::LoadParameter(const string& str_config_file)
{
	this->p_parameter = new Parameter();
	this->p_parameter->Load(str_config_file);
}

void DataSingleton::LoadModel()
{
	PRINT("DataSingleton is loading data now........\n");
	
	string rule_table_filename = this->p_parameter->GetRuleTableFile();
	vector<double> douVec_weights = this->p_parameter->GetWeights();
	douVec_weights.resize(4);          //To do: initial the weights automatically
	int i_rule_limit = this->p_parameter->GetRuleLimit();
	double dou_rule_threshold = this->p_parameter->GetRuleThre();
	
	
	this->p_rule_table_ = new RuleTable();
	this->p_rule_table_->Load(rule_table_filename, douVec_weights, i_rule_limit, dou_rule_threshold);
	

	string language_model_filename = this->p_parameter->GetLanguadeModelFile();
	int i_language_model_order = this->p_parameter->GetLanguageModelOrder();
	this->p_language_model_ = new LanguageModel(i_language_model_order, language_model_filename);
	this->p_language_model_->Load();
}
