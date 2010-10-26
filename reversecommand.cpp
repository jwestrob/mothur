/*
 *  reversecommand.cpp
 *  Mothur
 *
 *  Created by Pat Schloss on 6/6/09.
 *  Copyright 2009 Patrick D. Schloss. All rights reserved.
 *
 */

#include "reversecommand.h"
#include "sequence.hpp"

//**********************************************************************************************************************
vector<string> ReverseSeqsCommand::getValidParameters(){	
	try {
		string Array[] =  {"fasta", "outputdir","inputdir"};
		vector<string> myArray (Array, Array+(sizeof(Array)/sizeof(string)));
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "ReverseSeqsCommand", "getValidParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
ReverseSeqsCommand::ReverseSeqsCommand(){	
	try {
		abort = true;
		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
	}
	catch(exception& e) {
		m->errorOut(e, "ReverseSeqsCommand", "ReverseSeqsCommand");
		exit(1);
	}
}
//**********************************************************************************************************************
vector<string> ReverseSeqsCommand::getRequiredParameters(){	
	try {
		string Array[] =  {"fasta"};
		vector<string> myArray (Array, Array+(sizeof(Array)/sizeof(string)));
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "ReverseSeqsCommand", "getRequiredParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
vector<string> ReverseSeqsCommand::getRequiredFiles(){	
	try {
		vector<string> myArray;
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "ReverseSeqsCommand", "getRequiredFiles");
		exit(1);
	}
}
//***************************************************************************************************************

ReverseSeqsCommand::ReverseSeqsCommand(string option)  {
	try {
		abort = false;
		
		//allow user to run help
		if(option == "help") { help(); abort = true; }
		
		else {
			//valid paramters for this command
			string Array[] =  {"fasta", "outputdir","inputdir"};
			vector<string> myArray (Array, Array+(sizeof(Array)/sizeof(string)));
			
			OptionParser parser(option);
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			map<string,string>::iterator it;
			
			//check to make sure all parameters are valid for command
			for (it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			
			//initialize outputTypes
			vector<string> tempOutNames;
			outputTypes["fasta"] = tempOutNames;
			
			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);		
			if (inputDir == "not found"){	inputDir = "";		}
			else {
				string path;
				it = parameters.find("fasta");
				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["fasta"] = inputDir + it->second;		}
				}
			}

			//check for required parameters
			fasta = validParameter.validFile(parameters, "fasta", true);
			if (fasta == "not open") { abort = true; }
			else if (fasta == "not found") { fasta = ""; m->mothurOut("fasta is a required parameter for the reverse.seqs command."); m->mothurOutEndLine(); abort = true;  }	
			
			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found"){	
				outputDir = "";	
				outputDir += m->hasPath(fasta); //if user entered a file with a path then preserve it	
			}

		}
	}
	catch(exception& e) {
		m->errorOut(e, "ReverseSeqsCommand", "ReverseSeqsCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

void ReverseSeqsCommand::help(){
	try {
		m->mothurOut("The reverse.seqs command reads a fastafile and outputs a fasta file containing the reverse compliment.\n");
		m->mothurOut("The reverse.seqs command parameter is fasta and it is required.\n");
		m->mothurOut("The reverse.seqs command should be in the following format: \n");
		m->mothurOut("reverse.seqs(fasta=yourFastaFile) \n");	
	}
	catch(exception& e) {
		m->errorOut(e, "ReverseSeqsCommand", "help");
		exit(1);
	}
}

//***************************************************************************************************************

ReverseSeqsCommand::~ReverseSeqsCommand(){	/*	do nothing	*/	}

//***************************************************************************************************************


int ReverseSeqsCommand::execute(){
	try{
		
		if (abort == true) { return 0; }
		
		ifstream inFASTA;
		m->openInputFile(fasta, inFASTA);
		
		ofstream outFASTA;
		string reverseFile = outputDir + m->getRootName(m->getSimpleName(fasta)) + "rc" + m->getExtension(fasta);
		m->openOutputFile(reverseFile, outFASTA);
		
		while(!inFASTA.eof()){
			if (m->control_pressed) {  inFASTA.close();  outFASTA.close(); remove(reverseFile.c_str()); return 0; }
			 
			Sequence currSeq(inFASTA);  m->gobble(inFASTA);
			if (currSeq.getName() != "") {
				currSeq.reverseComplement();
				currSeq.printSequence(outFASTA);
			}
		}
		inFASTA.close();
		outFASTA.close();
		
		if (m->control_pressed) {  remove(reverseFile.c_str()); return 0; }
		
		m->mothurOutEndLine();
		m->mothurOut("Output File Name: "); m->mothurOutEndLine();
		m->mothurOut(reverseFile); m->mothurOutEndLine();	outputNames.push_back(reverseFile); outputTypes["fasta"].push_back(reverseFile);
		m->mothurOutEndLine();

		
		return 0;
		
	}
	catch(exception& e) {
		m->errorOut(e, "ReverseSeqsCommand", "execute");
		exit(1);
	}
}

//***************************************************************************************************************
