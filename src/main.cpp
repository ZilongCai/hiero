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

/**
 * @author: Wang Xing xingwsuda@gmail.com
 */


using namespace std;

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



	string line;
	int lineCount = 0;
	
	clock_t gStart = clock();
	while(getline(fin,line))
	{
		if(line.size() == 0) continue;
		clock_t start = clock();
		PRINT("Translating sent(" << lineCount+1 << "):\n" << line << "\n");
		
		cout << "start construct line....." << endl;	
		Parser* parser = new Parser(line);
		cout << "start translate sentence...." << endl;
		string str_ret = parser->ParseSentence();
		cout << "finish translate sentence...." << endl;
		fout << str_ret << endl;
		
		if(nBestOut)
		{
			for(int i = 0; i < parser->GetNbestSize(); i++)
			{
				string str = parser->GetNbest(i);
				nBestOut << lineCount << " ||| ";
				nBestOut << str;
				nBestOut << endl;
			}
		}
		
		delete parser;
		
		clock_t end = clock();
		PRINT("finished the translation of sent << (" << lineCount+1 <<") Result:\n"
				   << str_ret << "\n"
				   << "in time(s):" << (end - start)/(float)CLOCKS_PER_SEC
  				   << "\n");
		lineCount++;
			
	}
	fin.close();
	fout.close();
	nBestOut.close();
	
	clock_t  gEnd = clock();

	cerr << "translation finished! " << "total sents: " << lineCount << endl;
	cerr << "total time(s)" << (gEnd - gStart)/(float)CLOCKS_PER_SEC << endl;
	
	return 0;
}











