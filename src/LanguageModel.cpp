
#include <ctime>

#include "LanguageModel.h"
#include "Util.h"
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
		PRINT("Exit SRI Language Model...");
        	cerr<<"directly used exit()"<<endl;
		exit(0);
		delete this->m_vbPtr;
		delete this->m_lmPtr;
		PRINT("done. \n");
	}

}

bool LanguageModel::Load()
{
	PRINT("Read LanguageModel from : \""<<m_lmFile<<"\" ...");
	
	clock_t start = clock();
	this->m_vbPtr = new Vocab();
	this->m_vbPtr->unkIsWord() = true;
	this->m_lmPtr = new Ngram(*m_vbPtr, this->m_order);

    	File file(m_lmFile.c_str(), "r");
    	if(file.error()) {
        	fprintf(stderr,"Error:: Could not open file %s\n", m_lmFile.c_str());
        	return 0;
    	}
    	else
        this->m_lmPtr->read(file);

	clock_t finish = clock();

	if(m_lmPtr)
	{
		m_active = true;

		PRINT("Load in "<< double(finish - start) / CLOCKS_PER_SEC
				<<" seconds\n");
	
		return true;
	}else{
		PRINT("failed in loading language model \n");
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

	while ((size_t)spp2 != string::npos)
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
			int start = (i < (int)order) ? 0 : wordPosVec[i - order];
			context = str.substr(start, wordPosVec[i - 1] - 1 - start);
		}

		float p = GetProb(word, context);

		prob += p;

	}	
	return prob;
}

float LanguageModel::GetProb(const std::string &word, const std::string &context)const
{
	VocabString v_contexts[7];
	VocabString v_word = word.c_str();

	Vocab::parseWords((char *)context.c_str(), (VocabString *) v_contexts, 7);
	Vocab::reverse(v_contexts);

	LM* lmPtr = this->m_lmPtr;
	LogP prob = lmPtr->wordProb(v_word, v_contexts) * LOG_E_10;

	if(prob == LogP_Zero) {
		prob = -100;
	}

	return prob>-100? prob:-100;
}

//TODO
/*
float LanguageModel::GetProb(const std::string &str, size_t order, size_t position){
	
}*/

