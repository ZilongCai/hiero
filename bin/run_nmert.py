#!/usr/bin/python

import sys
import os
import re
import time
import string

#Revision history
#   2010-03-12 added count_weights() and update_ini() functions by xxy; 

# function delegatation.
run = os.system

# global variables to save the best iteration and its corresponding bleu score
opt_ite = 1
opt_bleu = 0
feat_num = 9

s_bleu_is_case = ""
s_is_post_deal = True
s_is_to_char = False
s_is_src_to_low = True
bin_dir = ""
decoder = ""

post_deal = "/home/xwang/2.hiero/0.bin/post_deal_v0.2"

def update_bleu(ite):
    "Update the highest bleu score"

    global opt_bleu
    global opt_ite

    bleu = get_bleu("run%d.eva" %ite)
    print "iteration", ite, "bleu = ", bleu
    if bleu <= opt_bleu:
        return
    opt_bleu, opt_ite = bleu, ite
    
    #copy the best run to current directory
    run("cp run%d.ini best.ini" %ite) 
    run("cp run%d.eva best.eva" %ite)

def get_bleu(fe):
    "Get the bleu score from result file printed by mteval"

    c = file(fe, "rU").read()
    reg = re.compile(r"BLEU score =\s+(.*?)\s+")
    r = reg.findall(c)
    assert len(r) == 1
    return float(r[0])

def sgm2plain(src_sgm, src_plain):
    "Tunning the $src_sgm to $src_plain"
    
    fr_sgm = file(src_sgm, "r")
    fw_plain = file(src_plain, "w")

    while True:
        line = fr_sgm.readline()

        if len(line) == 0:
            break
        
        if not line.startswith("<seg "):
            continue

        start_pos = line.find(">")+1
        end_pos = line.rfind("<")-1

        out_line = line[start_pos : end_pos].strip()
        
        if s_is_src_to_low:
            out_line = string.lower(out_line)
        
        #print >> fw_plain, line[start_pos : end_pos].strip()
        print >> fw_plain, out_line
    fr_sgm.close()   
    fw_plain.close()
    if s_is_to_char:
        run("%s/split2char %s %s" %(bin_dir, src_plain, src_plain+"_split"))
        run("mv %s %s" %(src_plain+"_split", src_plain))

     

def split_seg(line):
    p1 = line.find(">")+1
    p2 = line.rfind("<")
    return [ line[:p1], line[p1:p2], line[p2:] ]


def plain2sgm(trg_plain, src_sgm, trg_sgm):
    "Converse plain format to sgm format"
    fin_trg_plain = file(trg_plain , "r")
    fin_src_sgm = file(src_sgm, "r")
    fout = file(trg_sgm, "w")
    
    #head
    doc_head = fin_src_sgm.readline().rstrip().replace("srcset", "tstset")
    if doc_head.find("trglang") == -1 :
        doc_head = doc_head.replace(">", " trglang=\"en\">")
    print >> fout, doc_head

    for line in fin_src_sgm:
        line = line.rstrip()
        #process doc tag
        if "<doc" in line or "<Doc" in line or "<DOC" in line:
            p1 = line.find('"')
            p2 = line.find('"' , p1+1)
            id = line[p1+1 : p2]
            print >> fout, '''<doc docid="%s" sysid="hiero">''' %id
        elif line.startswith("<seg"):
            head, body , tail  = split_seg(line)
            print >> fout, head, fin_trg_plain.readline().rstrip(), tail
        elif line.strip() == "</srcset>":
            print >> fout, "</tstset>"
        else:
            print >> fout, line
    
    fout.close()
    fin_src_sgm.close()
    if s_is_post_deal:
        run("%s -o %s -n %s" %(post_deal, trg_sgm, trg_sgm+".new" ))
        run("mv %s %s" %(trg_sgm+".new", trg_sgm))
     

def count_weights(ini):
    "Count the weights number in ini"
    
    in_ini = file( ini , "r")
    count = 0
    
    while True:
        line = in_ini.readline();
        if len(line) == 0:
            break
        line = line.rstrip()
        
        if( line.find("[weight-") == 0 and line.find("[weight-file") != 0 ):
            line = in_ini.readline().strip();
            while( line != ""):
                line = in_ini.readline().strip();
                count = count + 1
    in_ini.close()
    return count

#if we change the feature used in the decoder, we need to modify this function 
#order: rule; rule_count; glue_count; wp; lm;
#       seg_lm; seg_me; src_len; oov_char_count; oov_len_count
def update_ini(lambda_file, old_ini, new_ini, is_skip):
    "Update the configuration file"
    
    in_lamb = file(lambda_file, "r")
    in_ini = file(old_ini, "r")
    out = file(new_ini, "w")
    
    #skip first line
    if is_skip:
        in_lamb.readline()
    
    line_lamb = in_lamb.readline()
    weights = line_lamb.split();
    feat_idx = 0
    
    while True:
        line = in_ini.readline();
        if len(line) == 0:
            break
        line = line.rstrip()

        print >> out, line

        if( line.find("[weights]") == 0 ):
            count = 0
            line = in_ini.readline().strip();
            while( line != ""):
                line = in_ini.readline().strip();
                count += 1

            for i in range(0, feat_num):
                print >> out, weights[feat_idx]
                feat_idx += 1
            print >> out,""
    out.close()
    
def run_new_mert(feat_score_file, cand_file, weight_file, ite) :
    init_file = "init.opt"
    feat_file = "feature.data"
    score_file = "score.data"
    default_weight_file = "weights.txt"
    
    #fout = file(init_file, "w")
    #print >> fout, "1 " * feat_num 
    #fout.close()

    #adapted to moses init
    #moses.ini.opt
    	#0.173656 -0.364714 0.00899781 0.0863148 0.0800198 0.0804651 0.0835555 0.122277
	#0 0 0 0 0 0 0 0
	#1 1 1 1 1 1 1 1
    
    fin = open("run%d.ini" %ite, "rU")
    weight_line = ""
   
    while True:
        try:
            line = fin.next().strip()
            #print line
            #'''
            if( line.find("[weights]") == 0 ):
                i = 0
                while i < feat_num:
                        line = fin.next().strip()
                        weight_line =  weight_line + " " + line
                        i = i + 1
            #'''
        except StopIteration:
                break
    weight_line  = weight_line.strip()
    fin.close()
    
    fout = file(init_file, "w") 
    print >> fout, weight_line
    print >> fout, "0 " * feat_num
    print >> fout, "1 " * feat_num
    fout.close()


    run("python %s/change_feat.py %s %s %s %s" %(bin_dir, feat_score_file, cand_file, feat_file, score_file))
    run("/home/xwang/11.Moses/mosesdecoder-master/bin.BiNeuralLM/mert --sctype BLEU --scconfig case:false --thread 10 -n 20 -S %s -F %s -d %d" %(score_file, feat_file, feat_num))
    run("mv %s %s" %(default_weight_file, weight_file))
    run("rm %s %s" %(feat_file, score_file))

#if the input paramter init_ini dev_src_sgm dev_ref_sgm is not full path, there may be some problem
#here we need an ensure_full_path to correct this problem
def mert(work_dir, max_ite, init_ini, dev_src_sgm, dev_ref_sgm):
    "Function for running mert "
    
    #make directory for working directory
    run("mkdir %s" %(mert_dir))
    run("cp %s %s/run1.ini" %(init_ini, work_dir))  
 
    dev_src_plain = "dev.plain"
    #run("cp %s  %s/%s" %(dev_src_sgm,work_dir,dev_src_plain))
    sgm2plain(dev_src_sgm, "%s/%s" %(work_dir, dev_src_plain))

   
    #change work directory to work_dir
    pre_work_dir = os.getcwd()
    os.chdir(mert_dir)
    start = time.time()
    
    #initial file about weights for mert
    for ite in xrange(1, max_ite + 1):
    	print decoder
	print dev_src_plain
        assert run("%s -f run%d.ini" %(decoder, ite)) == 0
        
	run("mv nbest.file run%d.nbest" %ite)	
	run("mv trans.output run%d.trans.plain" %ite)
        ### (2) change the kbest file format to fit funtion of creat_feature
        run("%s/change_nbest_format run%d.nbest tmp.nbest" %(bin_dir,ite))
        run("mv tmp.nbest run%d.nbest" %(ite)) 

        ### (3) score the bleu of result
        plain2sgm("run%d.trans.plain" %ite, dev_src_sgm, "run%d.trans.sgm" %ite )
        run("%s/mteval-v11b.pl %s -s %s -r %s -t run%d.trans.sgm > run%d.eva" %(bin_dir, s_bleu_is_case, dev_src_sgm, dev_ref_sgm, ite, ite))
        update_bleu(ite) 

        ### (4) create or update the feature file
        if ite == 1:
            run("%s/create_feature run1.nbest %s run1.cand run1.feat 1" %(bin_dir,dev_ref_sgm))
        else:
            cmd = "python %s/merge_feature.py \
                    --o_kbest run%d.nbest \
                    --o_cand run%d.cand \
                    --o_feat run%d.feat \
                    --c_kbest run%d.nbest \
                    --n_kbest merge.nbest \
                    --n_cand run%d.cand \
                    --n_feat run%d.feat \
                    --ref %s \
                    --createFeature %s/create_feature" \
                    %(bin_dir, ite - 1, ite - 1, ite - 1, ite, ite, ite, dev_ref_sgm, bin_dir)
            run(cmd)
            run("mv merge.nbest run%d.nbest" %ite)
        
        ### (5) run mert to get the tuning weights of every features       
        if ite < max_ite:
            run_new_mert("run%d.feat" %ite, "run%d.cand" %ite, "run%d.weights" %ite, ite)
            update_ini("run%d.weights" %ite, "run%d.ini" %ite, "run%d.ini" %(ite+1), False)

    finish = time.time()
    m = time.gmtime(finish - start)
    print "time: %d hour %d min %d second" %(m.tm_hour, m.tm_min, m.tm_sec)
    print "MERT is over, optimal (%f, %d)" %(opt_bleu, opt_ite)
    os.chdir(pre_work_dir)

def test(work_dir, ini, src_sgm, ref_sgm):
    "Runing the decoder on the test dataset"
    
    run("mkdir %s" %work_dir)
    sgm2plain(src_sgm, "%s/src.plain" %work_dir)
    #run("cp %s  %s/src.plain" %(src_sgm,work_dir));
    run("cp %s %s/test.ini" %(ini, work_dir) )
    pre_work_dir = os.getcwd()
    os.chdir(work_dir)

    #runing dataset
    assert run("%s -f test.ini" \
                 %(decoder) ) == 0
    
    #score the bleu of result
    plain2sgm("trans.output", src_sgm, "result.sgm")
    run("%s/mteval-v11b.pl %s -s %s -r %s -t result.sgm > eval2" \
            %(bin_dir, s_bleu_is_case, src_sgm, ref_sgm) )

    os.chdir(pre_work_dir)

def generate_test_config_file(test_file, rule_table):
	fin = open('./mert/best.ini', 'rU')
	ret = './test.ini'
	fout = open(ret, 'w')
	
	while True:
		try:
			line = fin.next().strip()
			if '[input-file]' == line:
				fout.write(line + '\n')
				fout.write(test_file + '\n')
				line = fin.next().strip()
			elif '[rule-table-file]' == line:
				fout.write(line + '\n')
				fout.write(rule_table + '\n')
				line = fin.next().strip()
			elif '[stack-limit]' == line:
				fout.write(line + '\n')
				fout.write('300\n')
				line = fin.next().strip()
			else:
				fout.write(line + '\n')
		except StopIteration:
			break
	fin.close()
	fout.close()
	return ret	


if __name__ == "__main__":
    #there are 3 steps to runing hiero decoder
    ### (1) extract hpb rule from word-alignment corpus
    ### (2) runing mert on devlepment dataset to tune the feature weights
    ### (3) runing decoder on test dataset
    
    ###!!! set the test condition
    s_bleu_is_case = ""
    s_is_post_deal = True
    s_is_to_char = False
    s_is_src_to_low = True

    #modifcation: the full path where the executable programs store, here "hiero" is name of the executable program of decoder
    bin_dir = "/home/xwang/2.hiero/0.bin"
    decoder = "/home/xwang/23.easy_hiero/2lan-easy-hiero-multithread"
    
    ### NOTE for the input data,please use full path 
    first_step = 1
    last_step = 2
    cur_step = 1
    
    ### (2) mert
   
    mert_dir="./mert"
    
   
    if (cur_step >= first_step) and (cur_step <= last_step):
        #modifcation: data_dir stores the development set and it's reference. here nist02* are the development sets and hpb.ini is the initial configure file.
        data_dir ="/home/xwang/2.hiero/1.corpus/nist_test"
      	year = "03"
        dev_src_sgm = data_dir + "/nist" + year + "/src.sgm"
        dev_ref_sgm = data_dir + "/nist" + year + "/ref_lc.sgm"
        
        init_ini = "config.ini"
        feat_num = 9
        max_ite = 15
	
        mert(mert_dir, max_ite, init_ini, dev_src_sgm, dev_ref_sgm)
       
    cur_step += 1
   

    if (cur_step >= first_step) and (cur_step <= last_step):
        years = ["02", "04", "05", "06", "08"]

        for year in years:
	    test_file = '/home/xwang/2.hiero/1.corpus/nist_test/nist' + year + '/src.plain'
	    rule_table = '/home/xwang/2.hiero/1.corpus/doc.table.orign.rule_len_5.src_len_10.src02-08'          
	    
            test_dir = "./eval_%s" %(year)
            pwd = os.path.abspath(".")
  
            #modifcation: data_dir stores the test set and it's reference.
            data_dir ="/home/xwang/2.hiero/1.corpus/nist_test"
            test_src_sgm = data_dir + "/nist" + year + "/src.sgm"
            test_ref_sgm = data_dir + "/nist" + year + "/ref.sgm"

	    config_file = generate_test_config_file(test_file, rule_table)


	    test(test_dir, config_file, test_src_sgm, test_ref_sgm)

    cur_step += 1
