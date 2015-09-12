#ifndef _CHART_H
#define _CHART_H

#include<vector>
#include<string>
#include<map>
#include"Cell.h"
#include"Hypothesis.h"
#include"Util.h"

using namespace std;

class Chart
{
public:
	Chart();
	~Chart();		
	
	
	/**
 	*   @brief: Put hypothesis into hypo vector
 	*   @param[in] p_hypo: the hypothesis point to put 
 	*/	 
	//void UpdateHypoVec(Hypothesis* p_hypo);
	
	/**
 	*   @brief: Put cell into cell map
 	*   @param[in] p_cell: the cell point to put
 	*/
	void UpdateCell(const Span& sp, Cell * p_cl);

	int GetCellSize(Span& sp);

	bool CellIsEmpty(Span& sp);
	
	Cell* GetCell(Span& sp) const
	{
		map<Span, Cell*>::const_iterator ite = this->map_chart_cell_.find(sp);
		if(ite != this->map_chart_cell_.end())
		{
			return ite->second;
		}
		else
		{
			return NULL;
		}
	}

	
private:
	int i_sent_length_;
	int i_rule_span_limit_;
	//vector<Hypothesis*> hyVec_hypo_;
	map<Span, Cell*> map_chart_cell_;
};


#endif
