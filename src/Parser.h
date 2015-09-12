#ifndef _PARSER_H
#define _PARSER_H

#include<vector>
#include<string>
#include"Chart.h"
#include"Cell.h"
#include"DataSingleton.h"
#include"Rule.h"
#include"Util.h"
#include"Cube.h"

using namespace std;

class Parser
{
public:
	Parser(const string& str_sentence);
	~Parser();	
	
	/**
 	*  @brief: Parse a input sentence 
 	*  @parame[out]: return the 1-best translation
 	*/	  	
	string ParseSentence();

	/**
 	* @brief: Init the chart cell with span len is equal to 1.
 	*
 	*/	 
	void InitChart();
	
	/**
 	* @brief: Transfer a rule into a hyputhesis, then return the hypothesis pointer.
 	* @param[in] r: the rule wait to transfer.  
 	* @param[out]: the transfered hypothesis pointer.
 	*/ 	
	Hypothesis* Rule2Hypo(Rule& r, const int& i_rule_ID);

	/** 
 	*  @brief: Use glue rules on a given source sentence span
 	*  @param[in] sp: the span index 
 	*  @param[in] vc: cube vector to put cubes which are generated using glue rules
 	*/ 
	void Glue(Span& sp, vector<Cube*>& vc) const;
	

	/**
 	* @brief: Translate the input string as a whole
 	* @param[in]: the string to be translated
 	* @param[out]: the vector which contains generated hypothesis pointer
 	*/ 

	vector<Hypothesis*> TranslateAsTerminal(const string& str);


	/**
 	* @brief: Use general rules to form cube on a given source sentence span. These rules contain only one nontermial(aX1 or X1b)
 	* @param[in] sp: the span index
 	* @param[in] vc: cube vector to put cubes which are generated using general rules
 	*/	 
	void ConstructCubeVecWithOneNonterminal(Span& sp, vector<Cube*>& vc) const;
	
	/**
 	* @brief: Generate source string with one nonterminal on a given source sentence span, the string is used to search the appropriate rules.
 	* Rule form: aXb
 	* @param[in] terminal_sp_left: the left terminal span index
 	* @param[in] terminal_sp_right: the right terminal span index
 	* @param[in] i_x_positoin: -1 means the nonterminal X1 is on the left(X1b), other numbers mean the right side(aX1). 
 	*
 	*/ 	
	string GenerateSrcWithOneNonterminal(Span& terminal_sp_left, Span& terminal_sp_right) const; 

	/**
 	* @brief: Use general rules to form cube on a given source senten span. These rules contain two nontermials(aX1bX2c, teminal a and c can be NULL)
 	* @param[in] sp: the span index
 	* @param[in] vc: cube vector to put cubes which are generated using general rules
 	*/ 
	void ConstructCubeVecWithTwoNonterminals(Span& sp, vector<Cube*>& vc) const;

	/**
 	*  @brief: Generate source string with two nonterminals on a given source sentence span, the string is used to search the appropriate rules.
 	*  Rule form : aX1bX2c
 	*  @param[in] left_span: the left terminal span index, (-1,-1) means left terminal a is NULL
 	*  @param[in] mid_span: the middle terminal span index
 	*  @param[in] right_span: the right terminal span index (-1, -1) means right terminal c is NULL 
	*/
	string GenerateSrcWithTwoNonterminals(Span& left_span, Span& mid_sp, Span& right_span) const;

	/**
 	* @brief: Cube pruning for a given source sentence span
 	* @param[in] sp: the span index
 	* @param[in] vc: cube vector 
 	*/ 
	void CubePruning(Span& sp, vector<Cube*> vc);	

	string GetNbestTranslation(const int& i);
	string GetNbestDerivation(const int& i);
	string GetHypoDerivation(const Hypothesis* p_hypo);

	string GetNbest(const int& i);


	int GetNbestSize()
	{
		Span sp(0, this->strVec_src_.size());
        	return (int)this->p_chart_->GetCellSize(sp);
	}

private:

	vector<string> strVec_src_;
	vector<double> douVec_weights_;
	Chart *p_chart_;
	
};

#endif
