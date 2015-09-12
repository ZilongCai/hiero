#include"DataSingleton.h"

DataSingleton* DataSingleton::m_pInstance;

DataSingleton::DataSingleton()
{
}

DataSingleton::~DataSingleton()
{
	delete this->p_rule_table_;
	delete this->p_large_language_model_;
	delete this->p_small_language_model_;
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
	

	string large_language_model_filename = this->p_parameter->GetLargeLanguadeModelFile();
	int i_large_language_model_order = this->p_parameter->GetLargeLanguageModelOrder();
	this->p_large_language_model_ = new LanguageModel(i_large_language_model_order, large_language_model_filename);
	this->p_large_language_model_->Load();


	string small_language_model_filename = this->p_parameter->GetSmallLanguadeModelFile();
        int i_small_language_model_order = this->p_parameter->GetSmallLanguageModelOrder();
        this->p_small_language_model_ = new LanguageModel(i_small_language_model_order, small_language_model_filename);
        this->p_small_language_model_->Load();
}
