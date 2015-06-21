
#include <ctime>

#include "LanguageModel.h"
#include "Util.h"
#include "lmsri.h"
#include <stdlib.h>
using namespace std;

LanguageModel::LanguageModel(size_t order,
							 const std::string lmFile)
{
	m_active = false;
	m_order = order;
	m_lmFile = lmFile;
}

LanguageModel::~LanguageModel(void)
{

	if(m_active)
	{
        	exit(0);
		sriUnloadLM(this->m_lmPtr);
	}

}

bool LanguageModel::Load()
{
	PRINT("Read LanguageModel from : \""<<m_lmFile<<"\" ...\n");
	
	clock_t start = clock();
	this->m_lmPtr = sriLoadLM(m_lmFile.c_str(),1,m_order,1,0);
	clock_t finish = clock();

	if(m_lmPtr)
	{
		m_active = true;
		PRINT("Load in "<< double(finish - start) / CLOCKS_PER_SEC <<" seconds\n");
		return true;
	}else{
		PRINT_ERR("failed in loading language model \n");
		return false;
	}
}
	

float LanguageModel::GetFullScore(const std::string &str)const
{
	float prob = 0;
	int pos = 1;
	size_t order = this->m_order;

	vector<int> wordPosVec;
	int spp1 = 0,
		spp2 = (int)str.find(' ', spp1);
	wordPosVec.push_back(spp1);

	while (spp2 != string::npos)
	{
		spp1 = spp2 + 1;
		wordPosVec.push_back(spp1);
		spp2 = (int)str.find(' ', spp1);
	}

	for (int i = pos; i <= (int)wordPosVec.size(); i++)
	{
		string word = i < (int)wordPosVec.size() ? 
			          str.substr(wordPosVec[i - 1], wordPosVec[i] - 1 - wordPosVec[i - 1]) :
		              str.substr(wordPosVec[i - 1], str.size() - wordPosVec[i - 1]),
			   context = "";

		if (i > 1)
		{
			int start = (i < order) ? 0 : wordPosVec[i - order];
			context = str.substr(start, wordPosVec[i - 1] - 1 - start);
		}

		float p = GetProb(word, context);

		prob += p;

	}

	return prob;
}

float LanguageModel::GetProb(const std::string &word, const std::string &context)const
{
	float ret = sriWordProb(this->m_lmPtr,word.c_str(),context.c_str())*LOG_E_10 ;
    //return ret;
	return ret>-100? ret:-100;
}

//TODO
/*
float LanguageModel::GetProb(const std::string &str, size_t order, size_t position){
	
}*/

