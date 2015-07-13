#include<cmath>
#include <queue>
#include<cmath>
#include"Parser.h"
#include"Util.h"

Parser::Parser(const string& str_sentence)
{
	//add head and tail for the input sentence
	string str  = "<s> " + str_sentence + " </s>";   
	this->strVec_src_ = Util::split(str);
	this->p_chart_ = new Chart();	
	this->douVec_weights_ = DataSingleton::GetInstance()->p_parameter->GetWeights();
		
	//PRINT("Parser is built....\n");
	//PRINT(this->strVec_src_.size() << endl);
	//PRINT(&this->strVec_src_  << endl);
}

Parser::~Parser()
{
	//PRINT("~Parser..\n");
	delete this->p_chart_;
	this->strVec_src_.clear();  
	this->douVec_weights_.clear();
}


vector<Hypothesis *> Parser::TranslateAsTerminal(const string& str_src) //unsort
{
	vector<Hypothesis*> vp_hypo;
	vector<int> vec_match_ruleID;
	//PRINT(str_src << "|" <<endl);
	if(DataSingleton::GetInstance()->p_rule_table_->MatchRule(str_src, vec_match_ruleID))
	{
		for(int j = 0; j < (int)vec_match_ruleID.size(); j++)  
		{
				Rule r = DataSingleton::GetInstance()->p_rule_table_->GetRule(vec_match_ruleID[j]); 
				Hypothesis* p_hypo = Rule2Hypo(r, vec_match_ruleID[j]);
				vp_hypo.push_back(p_hypo);
				
		}
	}
			
	else if(string::npos == str_src.find(" "))
	{
		Rule r = DataSingleton::GetInstance()->p_rule_table_->GenerateRule4OOV(str_src, 
					DataSingleton::GetInstance()->p_rule_table_->GetRule(0).GetFeats().size());
		Hypothesis* p_hypo = Rule2Hypo(r, -1); //-1 means OOV rule
		vp_hypo.push_back(p_hypo);
	}
	return vp_hypo;

}


Hypothesis* Parser::Rule2Hypo(Rule& r, const int& i_rule_ID)
{
	//PRINT("Into Rule2Hypo....\n");
	vector<double> douVec_feats = r.GetFeats();
	
	string str_trg;
	vector<string> strVec_trg = r.GetTrgRhs();
	for(size_t i = 0; i < strVec_trg.size(); i++)
	{
		str_trg += strVec_trg[i] + " ";
	}
	str_trg = Util::trim(str_trg);

	//PRINT("Add hypo [" << str_trg << "]\n");
	double dou_lm = DataSingleton::GetInstance()->p_language_model_->GetFullScore(str_trg);
	
	douVec_feats.push_back(dou_lm); //lm feature
	douVec_feats.push_back(1);      //rule count
	douVec_feats.push_back(0);      //glue rule count
	douVec_feats.push_back((int)strVec_trg.size());
	
	
	Hypothesis *p_hypo = new Hypothesis(str_trg, douVec_feats, this->douVec_weights_, i_rule_ID, NULL, NULL);
	//PRINT("Add hypo done...\n");	

	return p_hypo;
}

struct cmp
{
        bool operator() (Hypothesis* p_c1, Hypothesis* p_c2)
        {
                return p_c1->GetScore() < p_c2->GetScore();
        }
};

void Parser::InitChart()
{
	//PRINT("InitChart....\n");
	for(int i = 0; i < (int)this->strVec_src_.size(); i++)
	{
		string str_src = strVec_src_[i];
		vector<int> vec_match_ruleID;
		//PRINT(str_src << "|" <<endl);
		Cell* p_cl = new Cell();
		
		priority_queue<Hypothesis*, vector<Hypothesis*>, cmp > heap;
		vector<Hypothesis*> vp_hypo = TranslateAsTerminal(str_src);

		//sort
		for(vector<Hypothesis*>::iterator ite = vp_hypo.begin(); ite != vp_hypo.end(); ite++)
		{
			heap.push(*ite);
		}
		while(!heap.empty())
		{
			Hypothesis* p_hypo = heap.top();
                	heap.pop();
			p_cl->AddHypo(p_hypo, false);
		}
		
		Span sp(i, i+1);
		this->p_chart_->UpdateCell(sp, p_cl);
		
	}	
	//PRINT("InitChart over.....\n");
}

void Parser::Glue(Span& sp, vector<Cube*>& v_cube) const
{
	//PRINT("Glue for [" << sp.first << " " << sp.second << ")\n");
	for(int mid = sp.first+1; mid <= sp.second-1; mid++)
	{
		//PRINT("Glue split point is" << mid << endl);
		Span sp1(sp.first, mid);       // [sp.first, mid)
		Span sp2(mid, sp.second);    // [mid, sp.second)
		
		//PRINT(this->p_chart_->GetCellSize(sp1) << " ||| " << this->p_chart_->GetCellSize(sp2) << endl); 
		if((!this->p_chart_->CellIsEmpty(sp1))&&(!this->p_chart_->CellIsEmpty(sp2)))
		{
			//PRINT("Glue sucess...\n");
			vector<int> iv_rule_id(1, DataSingleton::GetInstance()->p_rule_table_->GetRuleTableSize()-1); //Glue rule ID
			Cube* p_cube = new Cube(iv_rule_id, sp1, sp2, this->p_chart_, DataSingleton::GetInstance()->p_parameter->GetCubeLimit());
			v_cube.push_back(p_cube); 
		}
	}
}

string Parser::GenerateSrcWithOneNonterminal(Span& terminal_sp_left, Span& terminal_sp_right) const
{	
	string ret("");
	for(int i = terminal_sp_left.first; i < terminal_sp_left.second; i++)
	{
		ret += this->strVec_src_[i] + " ";
	}
	
	//add nonterminal symbol
	ret = ret + X1 + " ";

	for(int i = terminal_sp_right.first; i < terminal_sp_right.second; i++)
	{
		ret = ret + this->strVec_src_[i] +  " ";
	}
 
	ret = Util::trim(ret);

	return ret;
}

string Parser::GenerateSrcWithTwoNonterminals(Span& left_sp, Span& mid_sp, Span& right_sp) const
{
	string ret("");
        for(int i = left_sp.first; i < left_sp.second; i++)
        {
                ret += this->strVec_src_[i] + " ";
        }

	//add nonterminal symbol X1
	ret = ret + X1 + " ";
	
	for(int i = mid_sp.first; i < mid_sp.second; i++)
        {
                ret += this->strVec_src_[i] + " ";
        }
	
	//add nonterminal symbol X2
	ret = ret + X2 + " ";

	for(int i = right_sp.first; i < right_sp.second; i++)
        {
                ret += this->strVec_src_[i] + " ";
        }

	ret = Util::trim(ret);

	return ret;
}

void Parser::ConstructCubeVecWithOneNonterminal(Span& sp, vector<Cube*>& vc) const
{
	Span null_sp(-1, -1);
	//PRINT("ConstructCubeVecWithOneNonterminal for [" << sp.first << " " << sp.second << ")"<< endl);
	
	for(int i_left_end = sp.first; i_left_end < sp.second; i_left_end++)
		for(int i_right_start = i_left_end + 1; i_right_start <= sp.second; i_right_start++)
			{
				Span teminal_sp_left(sp.first, i_left_end);
				Span x_sp(i_left_end, i_right_start);
				Span teminal_sp_right(i_right_start, sp.second);

				string str = GenerateSrcWithOneNonterminal(teminal_sp_left, teminal_sp_right);
				
				if(DataSingleton::GetInstance()->p_parameter->GetRuleLenLimit() >= i_left_end - sp.first + sp.second - i_right_start + 1 && (sp.second-i_right_start + i_left_end - sp.first > 0))//rule_len and span_len
				{
					if((!this->p_chart_->CellIsEmpty(x_sp)))
					{
						string str_src = GenerateSrcWithOneNonterminal(teminal_sp_left, teminal_sp_right);
						vector<int> iv_rule_id;
						if(DataSingleton::GetInstance()->p_rule_table_->MatchRule(str_src, iv_rule_id))
						{
								Cube* p_cube = new Cube(iv_rule_id, x_sp, null_sp, this->p_chart_, DataSingleton::GetInstance()->p_parameter->GetCubeLimit());
								vc.push_back(p_cube);
						}
					}
				}
			}


}

void Parser::ConstructCubeVecWithTwoNonterminals(Span& sp, vector<Cube*>& vc) const
{
	//PRINT("ConstructCubeVecWithTwoNonterminals for [" << sp.first << " " << sp.second << ")" << endl);
	int x_len_min = sp.second - sp.first - DataSingleton::GetInstance()->p_parameter->GetRuleLenLimit();
	
	//left_x --> [span] <-- rigth_x   
	// X1aX2 X1aX2b aX1bX2 aX1bX2c


	for(int left_x_start = sp.first; left_x_start < sp.second-2; left_x_start++) //left_x_start 
	{ 
		for(int left_x_len = 1; left_x_start + left_x_len <= sp.second-2; left_x_len++) //left_x_len
		{
			for(int right_x_start = left_x_start + left_x_len + 1; right_x_start < sp.second; right_x_start++) //right_start
			{
				for(int right_x_len =( 1 > x_len_min - left_x_len ? 1 : x_len_min - left_x_len) ; right_x_start + right_x_len <= sp.second; right_x_len++) //right_x_len
				{
						
						
						Span left_x_span(left_x_start, left_x_start + left_x_len);
                                                Span right_x_span(right_x_start, right_x_start + right_x_len);
						
						if((!this->p_chart_->CellIsEmpty(left_x_span)) && (!this->p_chart_->CellIsEmpty(right_x_span)))
						{
							Span left_terminal_span(-1, -1);
							Span mid_terminal_span(left_x_start + left_x_len, right_x_start);
							Span right_terminal_span(-1, -1);
							if(sp.first < left_x_span.first)
							{
								left_terminal_span.first = sp.first;
								left_terminal_span.second = left_x_span.first;
							}
							
							if(right_x_start + right_x_len < sp.second)
							{
								right_terminal_span.first = right_x_start + right_x_len;
								right_terminal_span.second = sp.second;
							}
						
							string str_src = GenerateSrcWithTwoNonterminals(left_terminal_span, mid_terminal_span, right_terminal_span);
							vector<int> iv_rule_id;
							DataSingleton::GetInstance()->p_rule_table_->MatchRule(str_src, iv_rule_id);
							if(!iv_rule_id.empty())
							{
								Cube* p_cube = new Cube(iv_rule_id, left_x_span, right_x_span, this->p_chart_, DataSingleton::GetInstance()->p_parameter->GetCubeLimit());
								vc.push_back(p_cube);
							}
						}		
				}
			}
		}
		
	}

	
}

void Parser::CubePruning(Span& sp, vector<Cube*> vc)
{
	//PRINT("CubePruning for [" << sp.first << " " <<sp.second << ") size " << vc.size() << endl);
	
	
	priority_queue<Hypothesis*, vector<Hypothesis*>, cmp > heap;
	
	//translate as a whole span
	string str_src;
	for(int i  = sp.first; i < sp.second; i++)
	{
		str_src = str_src + this->strVec_src_[i] + " ";
	}
	str_src = Util::trim(str_src);
	

	vector<Hypothesis*> vp_hypo = TranslateAsTerminal(str_src);
	for(vector<Hypothesis*>::iterator ite = vp_hypo.begin(); ite != vp_hypo.end(); ite++)
	{
		heap.push(*ite);
	}

	for(size_t i = 0 ; i < vc.size() && i <  DataSingleton::GetInstance()->p_parameter->GetCubeLimit(); i++)
	{
		Hypothesis* p_hypo= new Hypothesis();
		if(vc[i]->GetHypothesis(this->p_chart_, p_hypo))
		{
			//PRINT("Add hypo....\n");
			heap.push(p_hypo);
		}
		else
		{
			//delete cube
			vc.erase(vc.begin()+i);
		}
	}

	//PRINT("......................Init cube over....\n");	
	
	while(heap.size() <=  DataSingleton::GetInstance()->p_parameter->GetCubeLimit())
	{
		int i_max_score_index = 0;
		double dou_max_score = MIN_FLOAT;
		
		if(vc.empty())
		{
			break;
		}

		for(size_t i = 0 ; i < vc.size(); i++)
		{
			if(dou_max_score < vc[i]->GetCurrentMaxScore())
			{
				dou_max_score = vc[i]->GetCurrentMaxScore();	
				i_max_score_index = i;
			}
		}
		
		Hypothesis* p_hypo = new Hypothesis();
		if(i_max_score_index < vc.size() && vc[i_max_score_index]->GetHypothesis(this->p_chart_, p_hypo))
		{
			//PRINT("Cube ID is " << i_max_score_index << endl);
			//PRINT("Add hypo....\n");
			heap.push(p_hypo);
		}
		else
		{
			//delete cube
			//PRINT("erase...\n");
			vc.erase(vc.begin()+i_max_score_index);
		}
	}

	for(vector<Cube*>::iterator cite = vc.begin(); cite != vc.end(); cite++)
	{
		delete *cite;
	}
	
	//PRINT("Get cube hypo over....\n");
	//PRINT("Heap size is " << heap.size() << endl);
	
	if(heap.empty())
	{
		return;
	}


	Cell* p_cl = new Cell();
	double dou_min_score = heap.top()->GetScore() + log(DataSingleton::GetInstance()->p_parameter->GetStackThre());  //stack threshold
	for(size_t i = 0; i <  DataSingleton::GetInstance()->p_parameter->GetStackLimit() && !heap.empty(); i++)      //stack limit
	{
		Hypothesis* p_hypo = heap.top();
		heap.pop();
		if(p_hypo->GetScore() < dou_min_score)                                         
		{
			break;
		}
		bool b_hypo_recombination = true;
		if(0 == sp.first && (int)this->strVec_src_.size() == sp.second)
		{
			//not
			p_cl->AddHypo(p_hypo,!b_hypo_recombination);
		}
		else
		{
			if(!p_cl->AddHypo(p_hypo, b_hypo_recombination))  
			{
				i--;
			}		
		}
	}	

	this->p_chart_->UpdateCell(sp, p_cl);
	//PRINT("Cell [" << sp.first << " " << sp.second << ") Cell size is " << this->p_chart_->GetCellSize(sp) << endl);
}


string Parser::ParseSentence()
{
	/* Search procedure 
 	*  Figure4 @ http://www.aclweb.org/anthology/J/J07/J07-2003.pdf
	*/
	InitChart(); //1 == span length
	
	for(int  len = 2; len <= (int)this->strVec_src_.size(); len++)  // 2 <= span length
	{
		for(int start = 0; start <= (int)this->strVec_src_.size()-len; start++) 
		{
			Span sp(start, start+len);   //[start, start+len)
			//PRINT("Span [ " << start << " " << start + len << "]\n"); 
			vector<Cube*> v_cube;
			if((0 == start && len <=  DataSingleton::GetInstance()->p_parameter->GetRuleSpanLimit() + 1) ||   //head <s>
				(0 != start && len <=  DataSingleton::GetInstance()->p_parameter->GetRuleSpanLimit()))   // cube   
			{
				ConstructCubeVecWithOneNonterminal(sp, v_cube);
				if(3 <= len)
				{		
					ConstructCubeVecWithTwoNonterminals(sp, v_cube);
				}	
			}
		
			if(0 == start)  //glue 
			{
				Glue(sp, v_cube);		
			}
			
			CubePruning(sp, v_cube);
		}
	}
	
	Span sp(0, this->strVec_src_.size());
	//PRINT("Parse over...\n");
	//PRINT(this->p_chart_->GetCellSize(sp)<<endl);
	return Util::CutHeadTailTag(this->p_chart_->GetCell(sp)->GetHypo(0)->GetTrg());
}


string Parser::GetNbestTranslation(const int& i)
{
	Span sp(0, this->strVec_src_.size());
	if(this->p_chart_->GetCellSize(sp) <= i)
	{
		return "Out of K-best size.";
	}
	else
	{
		return this->p_chart_->GetCell(sp)->GetHypo(i)->GetTrg();
	}
}

string Parser::GetNbestDerivation(const int& i)
{
	Span sp(0, this->strVec_src_.size());
        if(this->p_chart_->GetCellSize(sp) <= i)
        {
                return "Out of K-best size.";
        }
	else
        {
               return GetHypoDerivation(this->p_chart_->GetCell(sp)->GetHypo(i));
        }
}

string Parser::GetNbest(const int& i)
{
	Span sp(0, this->strVec_src_.size());
	Hypothesis* p_hypo = this->p_chart_->GetCell(sp)->GetHypo(i);

	std::ostringstream  ostr;
        ostr << p_hypo->GetTrg() << " ||| ";
     
	vector<double> douVec =  p_hypo->GetFeats();
	for(size_t i = 0; i < douVec.size(); i++)
	{
		ostr << douVec[i] << " ";
	}
	ostr <<  " ||| " << p_hypo->GetScore();

	string ret(ostr.str());
	return ret;
	
}



string Parser::GetHypoDerivation(const Hypothesis* p_hypo)
{
	string ret("");
	if(NULL != p_hypo)
	{
		ret += GetHypoDerivation(p_hypo->GetHypoX1());
		ret += GetHypoDerivation(p_hypo->GetHypoX2());
		ret += DataSingleton::GetInstance()->p_rule_table_->GetRule(p_hypo->GetAppliedRuleID()).SrcTrg2String() + "\n";
		return ret;
	}
	else
	{
		return ret;
	}	
	

}
