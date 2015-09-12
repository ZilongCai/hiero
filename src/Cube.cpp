#include"Cube.h"
#include"Rule.h"
#include"DataSingleton.h"


Cube::~Cube()
{
	this->iVec_apply_ruleID_.clear();
	this->vec_unvisited_point_.clear();
	this->douVec_weights_.clear();
}

Cube::Cube(const vector<int>& iVec_apply_ruleid, const Span& sp1, const Span& sp2)
{
	this->iVec_apply_ruleID_ = iVec_apply_ruleid;
}


Cube::Cube(const vector<int>& iVec_apply_ruleid, const Span& sp1, const Span& sp2, Chart* p_chart, const int& i_cube_limit)
:i_X1_visit_index_(-1), i_X2_visit_index_(-1), i_rule_visit_index_(-1), i_visited_sum_(0), dou_current_max_(MIN_FLOAT),vec_unvisited_point_(50)
{
	//PRINT("Build cube ...\n");
	//PRINT("Cube limit " << i_cube_limit << endl);

	this->douVec_weights_ = DataSingleton::GetInstance()->p_parameter->GetWeights();
	this->iVec_apply_ruleID_ = iVec_apply_ruleid;
	this->sp_X1_ = sp1;
	this->sp_X2_ = sp2;

	Cell* p_cell_X1 = p_chart->GetCell(this->sp_X1_);
        Cell* p_cell_X2 = NULL;                           
	
	//PRINT("1--\n");
	size_t i_x1_max = p_cell_X1->GetSize();
	size_t i_rule_max = this->iVec_apply_ruleID_.size();
	size_t i_sum = i_x1_max + i_rule_max -2;
	size_t i_x2_max = -1;

	//PRINT("2--\n");
        if(-1 != this->sp_X2_.first)
        {
		p_cell_X2 = p_chart->GetCell(this->sp_X2_);
		i_x2_max = p_cell_X2->GetSize();
		i_sum += (i_x2_max -1) ;
        }
	
	int i_max = 0;
	bool b_flag = true;
	int i_count = 0;	
	
	//PRINT("bulid visit list ...\n");
	for(int s = 0; s <= i_sum; s++)
	{
		for(int i = 0; i < i_rule_max && b_flag; i++)
		{
			if(-1 == this->sp_X2_.first)
			{
				int j = s - i;
				if(j >= i_x1_max || j < 0)
				{
					break;
				}
				Point p = {i, j, -1};
				//PRINT(i << " " << j << " "<< -1 << endl);
				if(s >= (int)this->vec_unvisited_point_.size())
               		        {
                        		this->vec_unvisited_point_.resize(int(this->vec_unvisited_point_.size()*1.2));
                        	}
				
				this->vec_unvisited_point_[s].push_back(p);
				i_max = i_max > s ? i_max : s;
                               	i_count ++;           
				if(i_count > i_cube_limit)      
				{
					b_flag = false;
				}
			}
			else
			{
				for(int j = 0; j < i_x1_max  && b_flag; j++)
				{
					int k = s - i - j;
					if( k >= i_x2_max || k < 0)
					{
						break;
					}
					Point p = {i, j, k};
					if(s >= (int)this->vec_unvisited_point_.size())
					{
                        	        	this->vec_unvisited_point_.resize(int(this->vec_unvisited_point_.size()*1.2));
             	               	        }	
					this->vec_unvisited_point_[s].push_back(p);
					i_max = i_max > s ? i_max : s;
                	       	        i_count ++;
                      	          	if(i_count > i_cube_limit)
                        	       	{
                                        	b_flag = false;
                                	}
					
				}
			}
			
		}
		i_max = s;
	}
	this->vec_unvisited_point_.resize(i_max+1);

}

bool Cube::GetHypothesis(Chart const* p_chart, Hypothesis* ret_hypo)
{

	//PRINT("Get Hypo from cube..\n");
	vector<double> douVec_feats;
	vector<Hypothesis*> v_hypo;
	
	//get visit point
	while(this->vec_unvisited_point_[this->i_visited_sum_].empty())
	{
			this->i_visited_sum_++;
			if(this->i_visited_sum_ >= this->vec_unvisited_point_.size())
        		{
              		  	//PRINT("return ...\n");
                		return false;
        		}
	}
	
	
	
	Point p = this->vec_unvisited_point_[this->i_visited_sum_].back();  //visit the point
	this->vec_unvisited_point_[this->i_visited_sum_].pop_back();        //delete the point
	
	this->i_rule_visit_index_ = p.i_rule_index;
	this->i_X1_visit_index_ = p.i_X1_index;
	this->i_X2_visit_index_	= p.i_X2_index;
	
	Cell* p_cell_X1 = p_chart->GetCell(this->sp_X1_);
        Cell* p_cell_X2 = NULL;                           
	
	//PRINT("visit point is " << this->i_rule_visit_index_ << " " << this->i_X1_visit_index_ << " " << this->i_X2_visit_index_ <<endl);
	//PRINT("rule id is " << this->iVec_apply_ruleID_[this->i_rule_visit_index_] << endl);
	Rule r = DataSingleton::GetInstance()->p_rule_table_->GetRule(this->iVec_apply_ruleID_[this->i_rule_visit_index_]);
	douVec_feats = r.GetFeats();
	//PRINT("get x1 hypo..\n");
	Hypothesis* p_hypo_X1 = p_cell_X1->GetHypo(this->i_X1_visit_index_);
	Hypothesis* p_hypo_X2 = NULL;
	v_hypo.push_back(p_hypo_X1);
	
	if(-1 != i_X2_visit_index_ && -1 != this->sp_X2_.first)
	{
		//PRINT("get x2 hypo..\n");
		Cell* p_cell_X2 = p_chart->GetCell(this->sp_X2_);
		p_hypo_X2 = p_cell_X2->GetHypo(this->i_X2_visit_index_);
		v_hypo.push_back(p_hypo_X2);
	}

	
	
	//PRINT("Get trg string...\n");
	//get trg string
	string str_trg("");
	vector<string> vecStr = r.GetTrgRhs();
	//PRINT(r.SrcTrg2String() + "\n");
	for(vector<string>::iterator ite = vecStr.begin(); ite != vecStr.end(); ite++)
	{
		if(X1 == *ite)
		{
			str_trg += v_hypo[0]->GetTrg() + " ";
		}
		else if(X2 == *ite)
		{
			str_trg += v_hypo[1]->GetTrg() + " ";
		}
		else
		{
			str_trg += *ite + " ";
		}
	}
	str_trg = Util::trim(str_trg);
	//PRINT(str_trg + "\n");
		

	//PRINT("Get feats...\n");
	//PRINT("1......\n");
	//get features
	for(size_t i = 0; i < douVec_feats.size(); i++)
	{
		for(size_t j = 0; j < v_hypo.size(); j++)
		{	
			douVec_feats[i] += v_hypo[j]->GetFeats()[i];
		}
	}	

	//PRINT("2......\n");
	//double dou_lm = DataSingleton::GetInstance()->p_language_model_->GetFullScore(str_trg);
	//douVec_feats.push_back(dou_lm); //lm feature
	
	double dou_lar_lm = DataSingleton::GetInstance()->p_large_language_model_->GetFullScore(str_trg);
        double dou_sml_lm = DataSingleton::GetInstance()->p_small_language_model_->GetFullScore(str_trg);
        douVec_feats.push_back(dou_lar_lm); //lm feature
        douVec_feats.push_back(dou_sml_lm);
        
	//PRINT("3......\n");
	//rule count
	if( DataSingleton::GetInstance()->p_rule_table_->GetRuleTableSize()-2 >= this->iVec_apply_ruleID_[this->i_rule_visit_index_])
	{
		douVec_feats.push_back(1);
	}
	else
	{	
		douVec_feats.push_back(0);
	}
	for(size_t i = 0; i < v_hypo.size(); i++)
	{
		 douVec_feats[douVec_feats.size()-1] += v_hypo[i]->GetFeats()[douVec_feats.size()-1];
	}
	
	//PRINT("4......\n");
	//glue rule count
	if( DataSingleton::GetInstance()->p_rule_table_->GetRuleTableSize()-1 == this->iVec_apply_ruleID_[this->i_rule_visit_index_])
	{
                douVec_feats.push_back(1);
        }
        else
        {
                douVec_feats.push_back(0);
        }

        for(size_t i = 0; i < v_hypo.size(); i++)
        {
                 douVec_feats[douVec_feats.size()-1] += v_hypo[i]->GetFeats()[douVec_feats.size()-1];
        }

        douVec_feats.push_back((int)Util::split(str_trg).size());
	
	//PRINT("Generate hypo...\n");	
	//form a hypo	
	Hypothesis p_hypo(str_trg, douVec_feats, this->douVec_weights_, this->iVec_apply_ruleID_[this->i_rule_visit_index_], p_hypo_X1, p_hypo_X2);
	
	this-> dou_current_max_ = p_hypo.GetScore();

	//PRINT("return ...\n");	
	*ret_hypo = p_hypo;
	return true;
}
