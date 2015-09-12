#pragma once
#include <string>
#include"srilm.h"
#include"Util.h"

class LanguageModel
{
public:
	/**
	* @brief : The constructor of LanguageModel
	* @param[in] order: used for load
	* @param[in] lmFile: The flie name
	*/
	LanguageModel(size_t order,	
				  const std::string lmFile);
	~LanguageModel(void);

public:	
	/**
	* @brief: Load language model
	**/
	bool Load();
	/**
	* @brief: mainly invoke sriWordProb, paramters are for this function
	*/
	float GetProb(const std::string& word, const std::string& context )const;
	/**
	* @brief: Get score for string str
	* @param[in] str: The string which need score
	* @return:	The score str
	*/
	float GetFullScore(const std::string& str) const;
	/**
	 * @brief: Get the order of language module, refer to function sriLoadLM
	 * @return: The order 
	 */	
	size_t GetOrder() { return m_order; }
	
private:
	Ngram* m_lmPtr;
	Vocab* m_vbPtr;
        std::string m_lmFile;
	size_t m_order;
	bool m_active;
      
};

