#ifndef __CINT__

#include "GoAT.h"
#include "TSystem.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include <time.h>

/**
 * @brief the main routine
 * @param argc number of parameters
 * @param argv the parameters as strings
 * @return exit code
 */
int main(int argc, char *argv[])
{
	
	clock_t start, end;
	start = clock();

	std::string configfile = "";
	std::string serverfile = "";
	std::string dir_in = "";
	std::string dir_out = "";
	std::string file_in = "";
	std::string file_out = "";
	std::string pre_in = "Acqu";
	std::string pre_out = "GoAT";

	Int_t length;
	std::string flag;

	if(argc == 1)
	{
		cout << "Please provide a config file" << endl;
		return 0;
	}
	else if(argc == 2) configfile = argv[1];
	else
	{
		for(int i=1; i<argc; i++)
		{
			flag = argv[i];
			if(flag.find_first_of("-") == 0)
			{
				i++;
				flag.erase(0,1);
				if(strcmp(flag.c_str(), "s") == 0) serverfile = argv[i];
				else if(strcmp(flag.c_str(), "d") == 0) dir_in = argv[i];
				else if(strcmp(flag.c_str(), "D") == 0) dir_out = argv[i];
				else if(strcmp(flag.c_str(), "f") == 0) file_in = argv[i];
				else if(strcmp(flag.c_str(), "F") == 0) file_out = argv[i];
				else if(strcmp(flag.c_str(), "p") == 0) pre_in = argv[i];
				else if(strcmp(flag.c_str(), "P") == 0) pre_out = argv[i];
				else
				{
					cout << "Unknown flag " << flag << endl;
					return 0;
				}
			}
			else configfile = argv[i];
		}
	}

	// Check that config file exists:
	ifstream cfile(configfile.c_str());
	if(!cfile)
	{
		cout << "Config file '" << configfile << "' could not be found." << endl;
		return 0;
	}

	// If server file is specified, check that it exists
	if(serverfile.length() > 0)
	{
		// Check that file exists:
		ifstream sfile(serverfile.c_str());
		if(!sfile)
		{
			cout << "Server file '" << serverfile << "' could not be found" << endl;
			return 0;
		}
	}
	// If no server file is specified, allow for checking in the config file
	else serverfile = configfile;

	// Create instance of GoAT class
	GoAT* goat = new GoAT;

	// Scan server or config file for file settings
	flag = goat->ReadConfig("Input-Directory",0,(Char_t*)serverfile.c_str());	
	flag.erase(0,flag.find_first_not_of(" "));
	if(strcmp(flag.c_str(),"nokey") != 0) dir_in = flag;

	flag = goat->ReadConfig("Output-Directory",0,(Char_t*)serverfile.c_str());	
	flag.erase(0,flag.find_first_not_of(" "));
	if(strcmp(flag.c_str(),"nokey") != 0) dir_out = flag;

	flag = goat->ReadConfig("Input-File",0,(Char_t*)serverfile.c_str());	
	flag.erase(0,flag.find_first_not_of(" "));
	if(strcmp(flag.c_str(),"nokey") != 0) file_in = flag;

	flag = goat->ReadConfig("Output-File",0,(Char_t*)serverfile.c_str());	
	flag.erase(0,flag.find_first_not_of(" "));
	if(strcmp(flag.c_str(),"nokey") != 0) file_out = flag;

	flag = goat->ReadConfig("Input-Prefix",0,(Char_t*)serverfile.c_str());	
	flag.erase(0,flag.find_first_not_of(" "));
	if(strcmp(flag.c_str(),"nokey") != 0) pre_in = flag;

	flag = goat->ReadConfig("Output-Prefix",0,(Char_t*)serverfile.c_str());	
	flag.erase(0,flag.find_first_not_of(" "));
	if(strcmp(flag.c_str(),"nokey") != 0) pre_out = flag;
	// Finished scanning for file settings

	// Fix directories to include final slash if not there
	if(dir_in.find_last_of("/") != (dir_in.length()-1)) dir_in += "/";
	if(dir_out.find_last_of("/") != (dir_out.length()-1)) dir_out += "/";

	cout << "Server file '" << serverfile << "' chosen" << endl;
	cout << "Input directory '" << dir_in << "' chosen" << endl;
	cout << "Output directory '" << dir_out << "' chosen" << endl;
	cout << "Input file '" << file_in << "' chosen" << endl;
	cout << "Output file '" << file_out << "' chosen" << endl;
	cout << "Input prefix '" << pre_in << "' chosen" << endl;
	cout << "Output prefix '" << pre_out << "' chosen" << endl;

	// Perform full initialisation 
	if(!goat->Init(configfile.c_str()))
	{
		cout << "ERROR: GoAT Init failed!" << endl;
		return 0;
	}

	std::string file;
	std::string prefix;
	std::string suffix;

	// If input file is specified, use it
	if(file_in.length() > 0)
	{
		file = file_in;
		length = file.length();
		// File should at least have '.root' at the end
		if(length >= 5)
		{
			// Add input directory to it
			file_in = dir_in+file_in;
			// If output file is specified, use it
			if(file_out.length() > 0)
			{
				file_out = dir_out+file_out;
				if(!goat->File(file_in.c_str(), file_out.c_str())) cout << "ERROR: GoAT failed on file " << file_in << "!" << endl;
			}
			// If output file is not specified, build it
			else
			{
				// If output directory is not specified, build it
				if(dir_out.length() == 0)
				{
					prefix = file.substr(0,file.find_last_of("/")+1);
					dir_out = dir_in+prefix;
				}
				// If input prefix doesn't match, simply prepend output prefix to the file name
				if(file.find(pre_in)>file.length()) suffix = ("_"+file.substr(file.find_last_of("/")+1,length-(file.find_last_of("/")+1)));
				// If input prefix does match, switch prefixes
				else suffix = file.substr(file.find_last_of("/")+1+pre_in.length(),length-(file.find_last_of("/")+1+pre_in.length()));
				// Build output file name
				file_out = dir_out+pre_out+suffix;
				// Run GoAT
				if(!goat->File(file_in.c_str(), file_out.c_str())) cout << "ERROR: GoAT failed on file " << file_in << "!" << endl;
			}
		}
	}
	// Otherwise if input directory is specified, scan it
	else if(dir_in.length() > 0)
	{
		// If output directory is not specified, use the input directory
		if(dir_out.length() == 0) dir_out = dir_in;

		// Create list of files in input directory
		TSystemFile *sys_file;
		TSystemDirectory *sys_dir = new TSystemDirectory("files",dir_in.c_str());
		TList *file_list = sys_dir->GetListOfFiles();
		file_list->Sort();
		TIter file_iter(file_list);

		// Iterate over files
		while((sys_file=(TSystemFile*)file_iter()))
		{
			file = sys_file->GetName();
			length = file.length();
			// File should at least have '.root' at the end
			if(length >= 5)
			{
				//Check that prefixes and suffixes match
				prefix = file.substr(0,pre_in.length());
				suffix = file.substr(length-5,5);
				if(((strcmp(prefix.c_str(),pre_in.c_str()) == 0)) && (strcmp(suffix.c_str(),".root") == 0))
				{
					// Build input file name
					file_in = dir_in+file;
					// Build output file name
					suffix = file.substr(pre_in.length(),length-pre_in.length());
					file_out = dir_out+pre_out+suffix;					
					// Run GoAT
					if(!goat->File(file_in.c_str(), file_out.c_str())) cout << "ERROR: GoAT failed on file " << file_in << "!" << endl;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: No AcquRoot files found!" << endl;
		return 0;
	}

	end = clock();
	cout << "Time required for execution: "
	<< (double)(end-start)/CLOCKS_PER_SEC
	<< " seconds." << "\n\n";

	return 0;
}


GoAT::GoAT() :
				UseParticleReconstruction(0)
{ 
}

GoAT::~GoAT()
{
}

Bool_t	GoAT::Init(const char* configfile)
{
	cout << endl << "Initialising GoAT analysis:" << endl;
	cout << "==========================================================" << endl;	
	SetConfigFile((Char_t*)configfile);
		
	config = ReadConfig("Period-Macro");
	if( sscanf(config.c_str(),"%d\n", &period) == 1 ) UsePeriodMacro = 1;
		
	cout << "Setting up sorting criteria:" << endl;	
	cout << "==========================================================" << endl;	
	if(!GSort::PostInit()) 
	{
		cout << "GSort Init failed!" << endl; 
		return kFALSE;
	}
	
	cout << "Setting up analysis classes:" << endl;	
	cout << "==========================================================" << endl;	
    config = ReadConfig("DO-PARTICLE-RECONSTRUCTION");

    {
        int buffer=0;
        sscanf( config.c_str(), "%d\n", &buffer);
        UseParticleReconstruction = (buffer==1);
    }

	if(UseParticleReconstruction) 
	{
		if(!GParticleReconstruction::PostInit())
		{
			cout << "GParticleReconstruction Init failed!" << endl; 
			return kFALSE;
		}
	}			
	cout << endl;	

	cout << "Initialisation complete." << endl;
	cout << "==========================================================" << endl << endl;
  
	return kTRUE;
}

Bool_t	GoAT::File(const char* file_in, const char* file_out)
{
	cout << "Opening files: " << endl;
	if(!OpenAcquFile(file_in))	return kFALSE;	
	if(!OpenGoATFile(file_out))	return kFALSE;	
	
	cout << endl;

	cout << "Setting up tree files:" << endl;
	if(!OpenTreeRawEvent())			return kFALSE;
	if(!OpenTreeTagger())			return kFALSE;
	if(!OpenTreeTrigger())			return kFALSE;
	if(!OpenTreeScaler())			return kFALSE;
	cout << endl;
	
	cout << "Checking scaler reads for valid events:" << endl;	
	if(!FindValidAcquEvents())		return kFALSE;
	cout << endl;

	cout << "Opening particle reconstruction tree: ";	
	InitTreeParticles();
	cout << endl;
	
	Analyse();
}

void	GoAT::Analyse()
{
	cout << "Analysing ..." << endl;
	TraverseAcquEntries();			
	CloseOutputFile();
//	OutputStatistics();
	cout << "File complete." << endl;
	cout << "==========================================================" << endl << endl;	
}

void	GoAT::Reconstruct()
{
	if(UsePeriodMacro == 1)
	{
		if(GetActualEvent() % period == 0) 
						printf("Event: %d\n",GetActualEvent());
	}

	if(SortAnalyseEvent())
	{
		if(UseParticleReconstruction) GParticleReconstruction::Reconstruct();
		
		if(SortFillEvent()) FillEvent();
	}
}

Bool_t 	GoAT::Write()
{
	return kTRUE;
}

#endif
