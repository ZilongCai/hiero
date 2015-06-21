#ifndef _PARAMETER_H
#define _PARAMETER_H

#include<string>
#include<vector>

using namespace std;

class Parameter
{
public:
	Parameter();
	~Parameter();

	void Load(const string& str_config_file);
	
	vector<double> GetWeights() const
		{return this->douVec_weights_;}
	
	//for rule selection
	int GetRuleLimit() const
		{return this->i_rule_limit_;}

	double GetRuleThre() const
                {return this->dou_rule_threshold_;}
	
	
	//for X span generation
	int GetRuleLenLimit()
		{return i_rule_len_limit_;}
	
	int GetRuleSpanLimit() const
                {return this->i_rule_span_limit_;}
	

	
	//for cube pruning
	int GetCubeLimit()
		{return (int)(this->i_stack_limit_*(1+this->dou_stack_margin_));}	

	int GetStackLimit() const
		{return this->i_stack_limit_;}

	double GetStackThre() const
		{return this->dou_stack_threshold_;}

	double GetStackMargin() const
		{return this->dou_stack_margin_;}
	
	//file 
	string GetRuleTableFile()
		{return this->str_rule_table_file_;}

	string GetLanguadeModelFile()
		{return this->str_language_model_file_;}	

	int GetLanguageModelOrder()
		{return this->i_language_model_order_;}

	string GetInputFile()
		{return this->str_input_file_;}
	
	string GetOutputFile()
		{return this->str_output_file_;}

	string GetNbestFile()
		{return this->str_nbest_file_;}

	
private:
	vector<double> douVec_weights_;
	int i_weigths_number_;	

	int i_rule_span_limit_; 
	int i_rule_limit_;
	int i_rule_len_limit_;
	double dou_rule_threshold_;
	int i_stack_limit_;
       	double dou_stack_threshold_;
	double dou_stack_margin_;
	

	string str_nbest_file_;
	string str_input_file_;
	string str_output_file_;

	int i_nbest_size_;
	string str_rule_table_file_;	
	string str_language_model_file_;
	int i_language_model_order_;
};
#endif
