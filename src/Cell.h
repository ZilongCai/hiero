#ifndef _CELL_H
#define _CELL_H

#include<vector>
#include<string>
#include"Hypothesis.h"
#include"Util.h"
#include <string.h>
#include<set>

using namespace std;


/**
 * @brief: Cell unit. This cell is used for stote hypothesises in sort.
 *
 */



class Cell
{
public:
	Cell();
	~Cell();
	
	bool IsEmpty() const
		{return this->vp_hypothesis_.empty();}
	
	int GetSize() const
		{return (int)this->vp_hypothesis_.size();}	
	
	
	bool AddHypo(Hypothesis * p_hypo, bool b_hypo_recombinatoin);
	/*
	{
		this->vp_hypothesis_.push_back(p_hypo);
		return 0;
	}*/

	Hypothesis* GetHypo(const int& i)
	{
		if(i < this->vp_hypothesis_.size())
		{	
			return vp_hypothesis_[i];
		}
		return NULL;
	}

private:
	vector<Hypothesis*> vp_hypothesis_;
	set<string> set_str_hypo_;
};

#endif


















