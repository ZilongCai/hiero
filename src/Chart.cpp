#include"Chart.h"
#include<iterator>

Chart::Chart()
{

}

Chart::~Chart()
{
	//for(size_t i = 0 ; i < this->hyVec_hypo_.size(); i++)
	//{
	//	delete this->hyVec_hypo_[i];
	//}

	for(map<Span, Cell*>::iterator ite = this->map_chart_cell_.begin(); ite != this->map_chart_cell_.end(); ite++)
	{
		delete ite->second;
	}
	this->map_chart_cell_.clear();
}


//void Chart::UpdateHypoVec(Hypothesis* p_hypo)
//{
//	this->hyVec_hypo_.push_back(p_hypo);
//}

void Chart::UpdateCell(const Span& sp, Cell * p_cl)
{
	this->map_chart_cell_.insert(pair<Span, Cell*>(sp, p_cl));
}


int Chart::GetCellSize(Span& sp)
{
	map<Span, Cell*>::iterator ite = this->map_chart_cell_.find(sp);
		
	if(ite == this->map_chart_cell_.end())
	{	
		return -1;
	}
	else
	{
		return ite->second->GetSize();
	}
}


bool Chart::CellIsEmpty(Span& sp)
{
	 map<Span, Cell*>::iterator ite = this->map_chart_cell_.find(sp);

        if(ite == this->map_chart_cell_.end())
        {
                return true;
        }
        else
        {
                
		return ite->second->IsEmpty();
        }
}



/*
void Chart::UpdateCell(const Span& sp, const Hypothesis& hypo)
{
        PRINT("UpdateCell...\n");
	//put hypo into hypo_vec
        this->hyVec_hypo_.push_back(hypo);
        
        //put hypo into cell
        this->map_chart_cell_.insert(pair<Span, >)
        this->v2_cell_[sp.first][sp.second].GetHypoID((int)this->hyVec_hypo_.size() -1);
}
        
void Chart::UpdateCell(const Span& sp, const vector<Hypothesis>& vec_hypo)
{
	for(size_t i = 0; i< vec_hypo.size(); i++)
	{
		UpdateCell(sp, vec_hypo[i]);
	}
}*/
