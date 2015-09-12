#include<string>
#include<fstream>
#include<ctime>
#include<vector>
#include<strstream>

#include"DataSingleton.h"
#include"Parameter.h"
#include"Util.h"
#include"Parser.h"
#include<stdlib.h>
#include <pthread.h>

/**
 * @author: Wang Xing xingwsuda@gmail.com
 */


using namespace std;

int i_current_sent_index = 0;
pthread_mutex_t sum_mutex; //lock
vector<string>  str_vec_sentences; //store sentences waited to translate
vector<string>  str_vec_translog;
vector<string>  str_vec_trans;
vector<string>  str_vec_nbest;

int i_thread_live = 0;

struct mypara
{
	mypara(int i):i_thread_id(i){};
	int i_thread_id;
	
};


void* decode_procecess(void *arg)
{
	int i_sent_index = 0; //index of the sentence to be translated 	
	while(str_vec_sentences.size() > i_current_sent_index)
	{
		pthread_mutex_lock( &sum_mutex ); //lock
		string str_sentence;
		if((int)str_vec_sentences.size() > i_current_sent_index)
		{
			cerr << "sent [" << i_current_sent_index << "] is parsing .." << endl; 
			i_sent_index = i_current_sent_index;
			i_current_sent_index++;
			str_sentence = str_vec_sentences[i_sent_index];
			pthread_mutex_unlock( &sum_mutex ); //unlock
			
		}
		else
		{
			
        		pthread_mutex_unlock( &sum_mutex );//unlock
			pthread_exit(0);
			i_thread_live--;
		}
		
		
		clock_t start = clock();
		string str_translog;
		ostringstream  ostr_translog;
		ostr_translog << "start construct line....." << endl;
		Parser* parser = new Parser(str_sentence);
		ostr_translog << "start translate sentence...." << endl;
		string str_ret = parser->ParseSentence();
		ostr_translog <<"finish translate sentence...." << endl;
		
	
		
		string str_nbest;
		ostringstream  ostr_nbest;
                for(int i = 0; i < parser->GetNbestSize(); i++)
                {
                                string str = parser->GetNbest(i);
                                ostr_nbest << i_sent_index << " ||| ";
                                ostr_nbest << str;
                                ostr_nbest << endl;
				str_nbest = ostr_nbest.str();
				ostr_nbest.clear();
                }
                
		delete parser;

		
                clock_t end = clock();
		ostr_translog << "finished the translation of sent << (" << i_sent_index <<") Result:\n" 
		      	      << str_ret << "\n"
                              << "in time(s):" << (end - start)/(float)CLOCKS_PER_SEC << endl;
		str_translog = ostr_translog.str();
		ostr_translog.clear();
		
		pthread_mutex_lock( &sum_mutex ); //lock
		str_vec_translog[i_sent_index] = str_translog;
		str_vec_trans[i_sent_index] = str_ret + "\n";
		str_vec_nbest[i_sent_index] = str_nbest;
		cerr << i_sent_index << " ------------ " << endl;
		pthread_mutex_unlock( &sum_mutex ); //unlock
	}
	i_thread_live--;
	pthread_exit(0);

}



int main(int argc,char **argv)
{
	if(argc < 3 && "-f" != argv[1])
	{
		PRINT_ERR("./hiero -f config.ini\n");
		exit(0);
	}	
	
	string str_config_file = argv[2];
		
	PRINT("translation will start...\n");
	cout.setf(std::ios::fixed);
	cout.precision(5);
	cerr.setf(std::ios::fixed);
	cerr.precision(5);
	
	const DataSingleton* staticData = DataSingleton::GetInstance();
	if(!DataSingleton::LoadStaticParameter(str_config_file))
	{
		PRINT_ERR("failed to load parameter!!!\n");
		return 1;
	}
	
	if(!DataSingleton::LoadStaticModel())
	{
		PRINT_ERR("failed to load rule table or language model!!!\n");
                return 1;
	}
	
	ifstream fin;
	ofstream fout;

	fin.open(DataSingleton::GetInstance()->p_parameter->GetInputFile().c_str());
	if(!fin)
	{
		PRINT_ERR("failed to open input file!");
		exit(1);
	}
	
	fout.open(DataSingleton::GetInstance()->p_parameter->GetOutputFile().c_str());
	if(!fout)
	{
		PRINT_ERR("failed to open output file!\n");
		exit(1);
	}
	
	
	ofstream nBestOut;
	nBestOut.open(DataSingleton::GetInstance()->p_parameter->GetNbestFile().c_str());
        if(!nBestOut)
        {
                PRINT_ERR("failed to open nbest file!\n");
                exit(1);
        }
	
	clock_t gStart = clock();
	
	//prepare sentences to be translate
	string line;
	while(getline(fin,line))
	{
		str_vec_sentences.push_back(line);
	}
	str_vec_translog.resize(str_vec_sentences.size());
	str_vec_trans.resize(str_vec_sentences.size());
	str_vec_nbest.resize(str_vec_sentences.size());
	
	cerr << "str_vec_sentences str_vec_translog str_vec_trans str_vec_nbest size:" << str_vec_sentences.size() 
		<< " " <<str_vec_translog.size() << " " << str_vec_trans.size() << " " << str_vec_nbest.size() << endl; 
		
	int i_thread_num = DataSingleton::GetInstance()->p_parameter->GetThreadNum();
	i_thread_live = i_thread_num;

	vector<pthread_t> tids(i_thread_num);
	pthread_attr_t attr;
	pthread_attr_init( &attr ); 
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
	pthread_mutex_init( &sum_mutex, NULL );
	
	
	for( int i = 0; i < i_thread_num; i++ )
	{
        	mypara pa(i);
                int ret = pthread_create(&tids[i], &attr, decode_procecess, &pa);
                cerr << "Thread-"  << i << " is  starting !!!!" << endl;
		if( ret != 0 )
		{
                        cerr << "pthread_create error:error_code=" << ret << endl;
                }
         }

        //pthread_exit( NULL );
        pthread_mutex_destroy( &sum_mutex );

	while(0 != i_thread_live) {} //wait all threads die

	//write file
	for(vector<string>::iterator ite = str_vec_translog.begin(); ite != str_vec_translog.end(); ite++)
	{
		PRINT(*ite);
	}

	for(vector<string>::iterator ite = str_vec_trans.begin(); ite != str_vec_trans.end(); ite++)
	{
		fout << *ite;
	}
	
	for(vector<string>::iterator ite = str_vec_nbest.begin(); ite != str_vec_nbest.end(); ite++)
	{
		nBestOut << *ite;
	}
	
	
		
	fin.close();
	fout.close();
	nBestOut.close();
	
	clock_t  gEnd = clock();

	cerr << "translation finished! " << "total sents: " << str_vec_sentences.size() << endl;
	cerr << "total time(s)" << (gEnd - gStart)/(float)CLOCKS_PER_SEC << endl;
	
	pthread_exit( NULL );
	return 0;
}











