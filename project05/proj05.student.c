/*********************************************************************
    Abhinav Thirupathi
    Computer Project #5
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
#include <pthread.h> 
#include <stdlib.h>
#include <stdio.h>

using namespace std;

extern char **environ;

/*-------------------------------------------------------------------- 
	Name: env
    Purpose:  Display the users environment variables
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
	Name: set
    Purpose:  Manage the user’s set of environment variables
    Receive:  Vector of string with the tokens
    Return:   None
---------------------------------------------------------------------*/
void set(std::vector<string> wordVec)
{
	// Deletes an environment variable
	if (wordVec.size() == 2)	
	{
		int unsetenvFlag = unsetenv(wordVec[1].c_str());
					
		// Delete an environment variable fails, output error
		if (unsetenvFlag == -1)
		{
			cout << "Warning: Unable to set the environment" << endl;
		}
	} 
	// Sets or modifies an enviroment
	else if (wordVec.size() == 3)
	{
		int unsetenvFlag = setenv(wordVec[1].c_str(),wordVec[2].c_str(),1);
						
		// If set/modifies an environment fails, output error
		if (unsetenvFlag == -1)
		{
			cout << "Warning: Unable to set the environment" << endl;
		}
	}
	// Invalid number of arguments for set
	else
	{
		cout << "ERROR: Invalid number of arguments" << endl;
	}
}

/*-------------------------------------------------------------------- 
	Name: cd
    Purpose:  Manage the current working directory and update the 
    		  user's PWD environment variable
    Receive:  Vector of string with the tokens
    Return:   None
---------------------------------------------------------------------*/
void cd(std::vector<string> wordVec)
{
	vector<string> setVec{"set", "PWD"}; 
	// If 'cd', goes to the home directory and sets PWD
	if (wordVec.size() == 1)
	{
		int chdirFlag = chdir(getenv("HOME"));
		if (chdirFlag == 0)
		{
			setenv("PWD",getenv("HOME"),1);
		} 
		else
		{
			cout << "Warning: Failed to change directory" << endl;
		}
	}
	else if (wordVec.size() == 2)
	{
		// Just '~' goes to the home directory of the current user
		if(wordVec[1] == "~")
		{
			int chdirFlag = chdir(getenv("HOME"));
			if (chdirFlag == 0)
			{
				setenv("PWD",getenv("HOME"),1);
			}
			else
			{
				cout << "Warning: Failed to change directory" << endl;
			}
						
		}
		// '~/' goes to the pathname inputted
		else if((wordVec[1][0] == '~') && (wordVec[1][1] == '/'))
		{
			int chdirFlag = chdir(wordVec[1].c_str());
			if (chdirFlag == 0)
			{
				setVec.push_back(wordVec[1]);
				set(setVec);
			}
			else
			{
				cout << "Warning: Failed to change directory" << endl;
			}
		}
		// '/' goes to the pathname inputted
		else if (wordVec[1][0] == '/')
		{
			int chdirFlag = chdir(wordVec[1].c_str());
			if (chdirFlag == 0)
			{
				setVec.push_back(wordVec[1]);
				set(setVec);
			}
			else
			{
				cout << "Warning: Failed to change directory" << endl;
			}
		}
		// '~USER' goes to the home directory of the specified user
		else if (wordVec[1][0] == '~')
		{
			string temp = "/user/";
			temp += wordVec[1].erase(0,1);
			int chdirFlag = chdir(temp.c_str());
			if (chdirFlag == 0)
			{
				setVec.push_back(temp);
				set(setVec);
			}
			else
			{
				cout << "Warning: Failed to change directory" << endl;
			}
		}
		// Invalid pathname, its fails
		else
		{
			cout << "Warning: Failed to change directory" << endl;
			}
		}
	// Invalid number of commands
	else 
	{
		cout << "Warning: Invalid number of arguments" << endl;
	}
}

/*-------------------------------------------------------------------- 
	Name: runSystem
    Purpose:  Calls the system command
    Receive:  External command as a string
    Return:   None
---------------------------------------------------------------------*/
void * runSystem(void *vline)
{
	// Converts vline into string
	string line = *static_cast<string*>(vline);
	// System() call with error checking
	int flag = system(line.c_str());
	if(flag != 0)
	{
		cout << "Warning: Error with the external command" << endl;
	}
	pthread_exit(NULL);
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
				
		// Iterates through each word in the line
		stringstream s(line);
		string word;

		vector <string> wordVec; // Vector of the words
		
		// Makes a vector from a line string
		while (s >> word)
		{
			wordVec.push_back(word);
		} 
		
		// Further processing if the line is not empty
		if (wordVec.size() != 0)
		{
			// First token is 'exit', stops current file looping
			if (wordVec[0] == "exit")
			{
				return;
			}
			// First token is 'date', prints date and time
			else if (wordVec[0] == "date")
			{
				date();
			}
			// Token is 'env', displays env. variables
			else if (wordVec[0] == "env")
			{
				env();
			}
			// Token is 'path', displays current search path
			else if (wordVec[0] == "path")
			{
				path();
			}
			// Token is 'cwd', displays pathname of cwd
			else if (wordVec[0] == "cwd")
			{
				cwd();
			}
			// Token is 'cd', goes to the directory
			else if (wordVec[0] == "cd")
			{
				cd(wordVec);
			}
			// Token is 'set', sets the enivornment variable
			else if (wordVec[0] == "set")
			{
				set(wordVec);
			}
			// If an external directive, calls the system()
			else
			{
				pthread_t thread_id;
				pthread_create(&thread_id, NULL, runSystem, &line);
				pthread_join(thread_id, NULL);		
			}
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
		// Iterates through each word in the line
		stringstream s(line);
		string word;

		vector <string> wordVec; // Vector of the words
		
		// Makes a vector from a line string
		while (s >> word)
		{
			wordVec.push_back(word);
		} 
		
		// Further processing if the line is not empty
		if (wordVec.size() != 0)
		{
			// First token is 'exit', stops current file looping
			if (wordVec[0] == "exit")
			{
				return;
			}
			// First token is 'date', prints date and time
			else if (wordVec[0] == "date")
			{
				date();
			}
			// Token is 'env', displays env. variables
			else if (wordVec[0] == "env")
			{
				env();
			}
			// Token is 'path', displays current search path
			else if (wordVec[0] == "path")
			{
				path();
			}
			// Token is 'cwd', displays pathname of cwd
			else if (wordVec[0] == "cwd")
			{
				cwd();
			}
			// Token is 'cd', goes to the directory
			else if (wordVec[0] == "cd")
			{
				cd(wordVec);
			}
			// Token is 'set', sets the enivornment variable
			else if (wordVec[0] == "set")
			{
				set(wordVec);
			}
			// If an external directive, calls the system()
			else
			{
				pthread_t thread_id;
				pthread_create(&thread_id, NULL, runSystem, &line);
				pthread_join(thread_id, NULL);		
			}
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

