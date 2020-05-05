/*********************************************************************
    Abhinav Thirupathi
    Computer Project #4
**********************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <algorithm>
#include <vector>
using namespace std;

extern char **environ;

/*-------------------------------------------------------------------- 
	Name: env
    Purpose:  Display the user’s environment variables
    Receive:  None
    Return:   None
---------------------------------------------------------------------*/
void env()
{
	int i = 0;
	while (environ[i] != NULL)
	{
		cout << environ[i] << endl;
		i++;
	}
}

/*-------------------------------------------------------------------- 
	Name: path
    Purpose:  Display the current search path in a readable format
    Receive:  None
    Return:   None
---------------------------------------------------------------------*/
void path()
{
	// Converts it to a string and replaces':' with ' '
	string path = std::string(getenv("PATH"));
	std::replace(path.begin(), path.end(), ':', ' ');
							
	string temp;
	stringstream pathStream(path);
	vector<string> pathVector;    
    // Appends the each one to the path Vector 
	while (pathStream >> temp) 
	{
		pathVector.push_back(temp);
	}
	// Prints the content of the path vector
	for (auto a : pathVector)
	{
		cout << a << endl;
	}
}

/*-------------------------------------------------------------------- 
	Name: cwd
    Purpose:  Display the absolute pathname of the current working directory
    Receive:  None
    Return:   None
---------------------------------------------------------------------*/
void cwd()
{
	char buf[FILENAME_MAX];
	getcwd(buf, FILENAME_MAX);
	string cwd(buf);
	cout << cwd << endl;
}

/*-------------------------------------------------------------------- 
	Name: date
    Purpose:  Displays current date and time in a human-readable format
    Receive:  None
    Return:   None
---------------------------------------------------------------------*/
void date()
{
	time_t rawtime;
	time (&rawtime);
	cout << ctime(&rawtime) << endl;
}

/*-------------------------------------------------------------------- 
	Name: verboseMode
    Purpose:  Processes the file in verbose mode
    Receive:  None
    Return:   None
---------------------------------------------------------------------*/
void verboseMode(char* arg)
{
	ifstream file;		// File to be read
	file.open(arg);
		
	// If the file cannot be read, error printed
	if (!file.is_open())
	{
		cout << "ERROR: Cannot open " <<arg<< endl;
	}
			
	int lineCount = 1;	// Line count of each file
	string line;		// Line in the file
			
	// Loops through of the file
	while (std::getline(file, line))
    {
		cout << "<" << lineCount << ">" << line << '\n';
				
		// Iterates through each word in the file
		stringstream s(line);
		string word;
		int wordCount = 1;	// Word count of the line
					
		while (s >> word)
		{
			// The first token in the line
			if (wordCount == 1)
			{
				// First token is 'exit', stops current file looping
				if (word == "exit")
				{
					return;
				}
				// First token is 'date', prints date and time
				else if (word == "date")
				{
					date();
				}
				// Token is 'env', displays env. variables
				else if (word == "env")
				{
					env();
				}
				// Token is 'path', displays current search path
				else if (word == "path")
				{
					path();
				}
				// Token is 'cwd', displays pathname of cwd
				else if (word == "cwd")
				{
					cwd();
				}
				// If an unknown directive, prints error
				else
				{
					cout << "Warning: Invalid directive" << endl;
				}
			}
			// If more than two tokens per line, prints warning
			else if (wordCount > 1)
			{
				cout << "Warning: More than two directives on line" << endl;
			}
			wordCount++;
		}    		
		lineCount++;	
	}
}

/*-------------------------------------------------------------------- 
	Name: silentMode
    Purpose:  Processes the file in silent mode
    Receive:  None
    Return:   None
---------------------------------------------------------------------*/
void silentMode(char* arg)
{
	ifstream file;		// File to be read
	file.open(arg);
		
	// If the file cannot be read, error printed
	if (!file.is_open())
	{
		cout << "ERROR: Cannot open " <<arg<< endl;
	}
	
	string line;		// Line in the file
			
	// Loops through of the file
	while (std::getline(file, line))
    {			
		// Iterates through each word in the file
		stringstream s(line);
		string word;
		int wordCount = 1;	// Word count of the line
					
		while (s >> word)
		{
			// The first token in the line
			if (wordCount == 1)
			{
				// First token is 'exit', stops current file looping
				if (word == "exit")
				{
					return;
				}
				// First token is 'date', prints date and time
				else if (word == "date")
				{
					date();
				}
				// Token is 'env', displays env. variables
				else if (word == "env")
				{
					env();
				}
				// Token is 'path', displays current search path
				else if (word == "path")
				{
					path();
				}
				// Token is 'cwd', displays pathname of cwd
				else if (word == "cwd")
				{
					cwd();
				}
				// If an unknown directive, prints error
				else
				{
					cout << "Warning: Invalid directive" << endl;
				}
			}
			// If more than two tokens per line, prints warning
			else if (wordCount > 1)
			{
				cout << "Warning: More than two directives on line" << endl;
			}
			wordCount++;
		}    		
	}
}

/*-------------------------------------------------------------------- 
	Name: main
    Purpose:  Processes the command line argument and calls correct functions
    Receive:  The number command line arguments, and array of the arguments
    Return:   Return 0
---------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
	bool isVerbose = true;	// Boolean for is verbose or silent
	
	// Loops through the command line argument left to right
	for (int i = 1; i < argc; ++i) 
	{
		// If option is '-v', isVerbose is true
		if (strcmp(argv[i],"-v") == 0)
		{
			isVerbose = true;
		}
		// If option is '-s', isVerbose is false for silent
		else if (strcmp(argv[i],"-s") == 0)
		{
			isVerbose = false;
		}
		// If argument doesn't start with '-', assumes file
		else if (argv[i][0] != '-')
		{
			// Calls verboseMode function if it is verbose
			if (isVerbose == true)
			{
				verboseMode(argv[i]);
			}
			// Calls silentMode function if it is not verbose
			else if (isVerbose == false)
			{
				silentMode(argv[i]);
			}
		}
		// Invalid options will warn the user
		else 
		{
			cout << "Warning: Invalid option " << argv[i] << endl;
		}
	}
}

