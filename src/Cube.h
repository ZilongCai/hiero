#ifndef _CUBE_H
#define _CUBE_H

#include<vector>
#include<string>
#include"Util.h"
#include"Chart.h"

using namespace std;

struct Point
{
	int i_rule_index;
	int i_X1_index;
	int i_X2_index;
};

class Cube
{
public:
		
	Cube(const vector<int>& iVec_apply_ruleID_, const Span& sp1, const Span& sp2);
	Cube(const vector<int>& iVec_apply_ruleID_, const Span& sp1, const Span& sp2, Chart * p_chart, const int& i_cube_limt);
	
	~Cube();
	
	/**
 	* @brief: Get current pseudo best hypothesis from the Cube
 	* @param[in] p_chart: the chart pointer.
 	* @param[in] ret_ypo: the generated hypothesis pointer
 	* @param[out]: return true if hypothesis is generated successfully.
 	*/ 	
	bool GetHypothesis(Chart const* p_chart, Hypothesis* ret_hypo);
	


	double GetCurrentMaxScore()
		{return this->dou_current_max_;}	

private:
	
	vector<double> douVec_weights_;
	Span sp_X1_;
	Span sp_X2_; // -1 == sp_X2_.first means that the cube has no X2 member
		
	vector<int> iVec_apply_ruleID_;

	int i_X1_visit_index_;
	int i_X2_visit_index_;
	int i_rule_visit_index_;
		
	int i_visited_sum_;
	double dou_current_max_;
	vector<vector<Point> > vec_unvisited_point_;
};

#endif

