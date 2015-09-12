#include"Cell.h"

Cell::Cell()
{
}

Cell::~Cell()
{
	//PRINT("delete cell....\n");
	for(vector<Hypothesis*>::iterator ite = this->vp_hypothesis_.begin(); ite != vp_hypothesis_.end(); ite++)
	{
		delete *ite;
	}
	this->vp_hypothesis_.clear();
	this->set_str_hypo_.clear();	

}

bool Cell::AddHypo(Hypothesis * p_hypo, bool b_hypo_recombinatoin)
{
	string str_key = p_hypo->GetKey();
	
	if(b_hypo_recombinatoin)
	{
		set<string>::iterator ite = this->set_str_hypo_.find(str_key);
		if(ite == this->set_str_hypo_.end())
		{
			this->set_str_hypo_.insert(str_key);
			this->vp_hypothesis_.push_back(p_hypo);
			return true;
		}	
	}
	else
	{
                this->vp_hypothesis_.push_back(p_hypo);
                return true;
	}
	return false;
}
