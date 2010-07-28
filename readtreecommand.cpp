/*
 *  readtreecommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 1/23/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "readtreecommand.h"

//**********************************************************************************************************************
ReadTreeCommand::ReadTreeCommand(string option)  {
	try {
		globaldata = GlobalData::getInstance();
		abort = false;
				
		//allow user to run help
		if(option == "help") { help(); abort = true; }
		
		else {
			//valid paramters for this command
			string Array[] =  {"tree","group","name","outputdir","inputdir"};
			vector<string> myArray (Array, Array+(sizeof(Array)/sizeof(string)));
			
			OptionParser parser(option);
			map<string, string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			map<string, string>::iterator it;
		
			//check to make sure all parameters are valid for command
			for (it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			
			globaldata->newRead();
			
			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);		
			if (inputDir == "not found"){	inputDir = "";		}
			else {
				string path;
				it = parameters.find("tree");
				//user has given a template file
				if(it != parameters.end()){ 
					path = hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["tree"] = inputDir + it->second;		}
				}
				
				it = parameters.find("group");
				//user has given a template file
				if(it != parameters.end()){ 
					path = hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["group"] = inputDir + it->second;		}
				}
				
				it = parameters.find("name");
				//user has given a template file
				if(it != parameters.end()){ 
					path = hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["name"] = inputDir + it->second;		}
				}

			}

			
			//check for required parameters
			treefile = validParameter.validFile(parameters, "tree", true);
			if (treefile == "not open") { abort = true; }
			else if (treefile == "not found") { treefile = ""; m->mothurOut("tree is a required parameter for the read.tree command."); m->mothurOutEndLine(); abort = true;  }	
			else {  globaldata->setTreeFile(treefile);  globaldata->setFormat("tree"); 	}
			
			groupfile = validParameter.validFile(parameters, "group", true);
			if (groupfile == "not open") { abort = true; }	
			else if (groupfile == "not found") { groupfile = ""; m->mothurOut("group is a required parameter for the read.tree command."); m->mothurOutEndLine(); abort = true;	}
			else {  
				globaldata->setGroupFile(groupfile); 
				//read in group map info.
				treeMap = new TreeMap(groupfile);
				treeMap->readMap();
				globaldata->gTreemap = treeMap;
			}
			
			namefile = validParameter.validFile(parameters, "name", true);
			if (namefile == "not open") { abort = true; }
			else if (namefile == "not found") { namefile = ""; }
			else { readNamesFile(); }	
			
			if (abort == false) {
				filename = treefile;
				read = new ReadNewickTree(filename);
			}
						
		}
	}
	catch(exception& e) {
		m->errorOut(e, "ReadTreeCommand", "ReadTreeCommand");		
		exit(1);
	}
}
//**********************************************************************************************************************

void ReadTreeCommand::help(){
	try {
		m->mothurOut("The read.tree command must be run before you execute a unifrac.weighted, unifrac.unweighted. \n");
		m->mothurOut("It also must be run before using the parsimony command, unless you are using the randomtree parameter.\n");
		m->mothurOut("The read.tree command parameters are tree, group and name.\n");
		m->mothurOut("The read.tree command should be in the following format: read.tree(tree=yourTreeFile, group=yourGroupFile).\n");
		m->mothurOut("The tree and group parameters are both required.\n");
		m->mothurOut("The name parameter allows you to enter a namefile.\n");
		m->mothurOut("Note: No spaces between parameter labels (i.e. tree), '=' and parameters (i.e.yourTreefile).\n\n");
	}
	catch(exception& e) {
		m->errorOut(e, "ReadTreeCommand", "help");	
		exit(1);
	}
}

//**********************************************************************************************************************

ReadTreeCommand::~ReadTreeCommand(){
	if (abort == false) { delete read; }
}

//**********************************************************************************************************************

int ReadTreeCommand::execute(){
	try {
	
		if (abort == true) { return 0; }
		
		int readOk;
		
		readOk = read->read(); 
		
		if (readOk != 0) { m->mothurOut("Read Terminated."); m->mothurOutEndLine(); globaldata->gTree.clear(); delete globaldata->gTreemap; return 0; }
		
		vector<Tree*> T = globaldata->gTree;

		//assemble users trees
		for (int i = 0; i < T.size(); i++) {
			if (m->control_pressed) {  
				for (int i = 0; i < T.size(); i++) {  delete T[i];  }
				globaldata->gTree.clear();
				delete globaldata->gTreemap;
				return 0;
			}
	
			T[i]->assembleTree();
		}

		//output any names that are in group file but not in tree
		if (globaldata->Treenames.size() < treeMap->getNumSeqs()) {
			for (int i = 0; i < treeMap->namesOfSeqs.size(); i++) {
				//is that name in the tree?
				int count = 0;
				for (int j = 0; j < globaldata->Treenames.size(); j++) {
					if (treeMap->namesOfSeqs[i] == globaldata->Treenames[j]) { break; } //found it
					count++;
				}
				
				if (m->control_pressed) {  
					for (int i = 0; i < T.size(); i++) {  delete T[i];  }
					globaldata->gTree.clear();
					delete globaldata->gTreemap;
					return 0;
				}
				
				//then you did not find it so report it 
				if (count == globaldata->Treenames.size()) { 
					//if it is in your namefile then don't remove
					map<string, string>::iterator it = nameMap.find(treeMap->namesOfSeqs[i]);
					
					if (it == nameMap.end()) {
						m->mothurOut(treeMap->namesOfSeqs[i] + " is in your groupfile and not in your tree. It will be disregarded."); m->mothurOutEndLine();
						treeMap->removeSeq(treeMap->namesOfSeqs[i]);
						i--; //need this because removeSeq removes name from namesOfSeqs
					}
				}
			}
			
			globaldata->gTreemap = treeMap;
		}
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "ReadTreeCommand", "execute");	
		exit(1);
	}
}
/*****************************************************************/
int ReadTreeCommand::readNamesFile() {
	try {
		globaldata->names.clear();
		
		ifstream in;
		openInputFile(namefile, in);
		
		string first, second;
		map<string, string>::iterator itNames;
		
		while(!in.eof()) {
			in >> first >> second; gobble(in);
			
			itNames = globaldata->names.find(first);
			if (itNames == globaldata->names.end()) {  
				globaldata->names[first] = second; 
				
				//we need a list of names in your namefile to use above when removing extra seqs above so we don't remove them
				vector<string> dupNames;
				splitAtComma(second, dupNames);
				
				for (int i = 0; i < dupNames.size(); i++) {	nameMap[dupNames[i]] = dupNames[i];  }
			}else {  m->mothurOut(first + " has already been seen in namefile, disregarding names file."); m->mothurOutEndLine(); in.close(); globaldata->names.clear(); return 1; }			
		}
		in.close();
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "ReadTreeCommand", "readNamesFile");
		exit(1);
	}
}

//**********************************************************************************************************************
