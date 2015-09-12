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


	static bool LoadStaticParameter(const string& str_config_file)
	{
		m_pInstance->LoadParameter(str_config_file);
		return true;
	}
	
	static bool LoadStaticModel()
	{
		m_pInstance->LoadModel();
		return true;
	}

	void LoadModel();

	void LoadParameter(const string& str_config_file);

	RuleTable *p_rule_table_;
	LanguageModel *p_large_language_model_;
	LanguageModel *p_small_language_model_;
	Parameter *p_parameter;

private:
	DataSingleton();
	static DataSingleton* m_pInstance;
};

#endif


