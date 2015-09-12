#include<stdlib.h>
#include<fstream>
#include"Parameter.h"
#include"Util.h"


Parameter::Parameter()
{
}

Parameter::~Parameter()
{
}

void Parameter::Load(const string& str_config_file)
{
	ifstream fin;
	fin.open(str_config_file.c_str());
	if (!fin.is_open())
	{
		cerr<<"fail to open config file\n";
		return;
	}

	PRINT("Config file is [" << str_config_file << "] " << endl);
	string line;
	while(getline(fin, line))
	{
		line = Util::trim(line);
		if("[input-file]" == line)
		{
			getline(fin, line);
			line = Util::trim(line);
			this->str_input_file_ = line;	
		}
		else if("[output-file]" == line)
		{
			getline(fin, line);
                        line = Util::trim(line);
			this->str_output_file_ = line;
		}
		else if("[nbest-file]" == line)
		{
			getline(fin, line);
                        line = Util::trim(line);
			this->str_nbest_file_ = line;
		}
		else if("[nbest-size]" ==line)
		{
			getline(fin, line);
                        line = Util::trim(line);
                        this->i_nbest_size_ = atoi(line.c_str());
		}
		else if("[rule-table-file]" == line)
		{
			getline(fin, line);
                        line = Util::trim(line);
			this->str_rule_table_file_ = line;
			PRINT("Rule table is [" << this->str_rule_table_file_ << "] "<<endl);
		}
		else if("[large-language-model-order]" == line)
		{
			getline(fin, line);
                        line = Util::trim(line);
			this->i_large_language_model_order_ = atoi(line.c_str());
		}
		else if("[large-language-model-file]" == line)
		{
			getline(fin, line);
                        line = Util::trim(line);
			this->str_large_language_model_file_ = line;
			PRINT("Large Language model is [" << this->str_large_language_model_file_ << "] "<<endl);
		}
		else if("[small-language-model-order]" == line)
                {
                        getline(fin, line);
                        line = Util::trim(line);
                        this->i_small_language_model_order_ = atoi(line.c_str());
                }
                else if("[small-language-model-file]" == line)
                {
                        getline(fin, line);
                        line = Util::trim(line);
                        this->str_small_language_model_file_ = line;
                        PRINT("Small Language model is [" << this->str_small_language_model_file_ << "] "<<endl);
                }
		else if("[thread-num]" == line)
                {
                        getline(fin, line);
                        line = Util::trim(line);
                        this->i_thread_num_ = atoi(line.c_str());
                        PRINT("Decoder will creat [" <<  this->i_thread_num_ << "] thread(s)..\n");
                }		
		else if("[rule-limit]" == line)
		{
			getline(fin, line);
                        line = Util::trim(line);
			this->i_rule_limit_ = atoi(line.c_str());
		}
		else if("[rule-table-threshold]" == line)
		{
			getline(fin, line);
                        line = Util::trim(line);
			this->dou_rule_threshold_ = atof(line.c_str());
		}
		else if("[stack-margin]" == line)
		{
			getline(fin, line);
                        line = Util::trim(line);
			this->dou_stack_margin_ = atof(line.c_str());
		}
		else if("[stack-limit]" == line)
		{
			getline(fin, line);
                        line = Util::trim(line);
			this->i_stack_limit_ = atoi(line.c_str());
		}
		else if("[stack-threshold]" == line)
		{
			getline(fin, line);
                        line = Util::trim(line);
			this->dou_stack_threshold_ = atof(line.c_str());
		}
		else if("[rule-length-limit]" == line)
		{
			getline(fin, line);
                        line = Util::trim(line);
			this->i_rule_len_limit_ = atoi(line.c_str());
		}
		else if("[rule-span-limit]" == line)
		{
			getline(fin, line);
                        line = Util::trim(line);
			this->i_rule_span_limit_ = atoi(line.c_str());
		}
		else if("[weights-number]" == line)
		{
			getline(fin, line);
                        line = Util::trim(line);
			this->i_weigths_number_ = atoi(line.c_str());
		}
		else if("[weights]" == line)
		{
			this->douVec_weights_.clear();
			int i = 0;
			while(i < this->i_weigths_number_)
			{
				getline(fin, line);
                        	line = Util::trim(line);
				this->douVec_weights_.push_back(atof(line.c_str()));
				i++;
			}
		}
	}
	PRINT("Loading parameters is over...\n");
}
