/****************************************************************************
    Abhinav Thirupathi
    Computer Project #3
  ****************************************************************************/

#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

/*--------------------------------------------------------------------------- 
	Name: main
	
    Purpose:  Copy the contents of the source file to the destination file
    Receive:  Command argument
    Return:   The program successfully copy, else returns 0 if error found
---------------------------------------------------------------------------*/


int main(int argc, char* argv[]) {

	bool a = false;			// If a in argument then true, else false
	bool b = false;			// If b in argument then true, else false
	bool t = false;			// If t in argument then true, else false
	bool file1Exists = false;	// If source file in argument then true, else false
	bool file2Exists = false;	// If destination file in argument then true, else false
	
	char* file1;			// Stores source file
	char* file2;			// Stores destination file
	long bufferSize = 256;		// Default buffer size is 256, else needs to be greater than 0

	// Loops through the command line argument left to right
	for (int i = 0; i < argc; ++i) 
	{
		// If argument is '-a', bool a is set to true
		if (strcmp(argv[i],"-a") == 0 && a == false)
		{	
			a = true;
		} 
		// If argument '-a' appears again, error is printed
		else if (strcmp(argv[i],"-a") == 0 && a == true) 
		{
			cout << "ERROR: Repetition of option '-a'" << endl;
			return 0;
		}
		// If argument is '-b', bool b is set to true
		else if (strcmp(argv[i],"-b") == 0 && b == false) 
		{
			b = true;
		} 
		// If argument '-b' appears again, error is printed
		else if (strcmp(argv[i],"-b") == 0 && b == true) 
		{
			cout << "ERROR: Repetition of option '-b'" << endl;
			return 0;
		}
		// If argument is '-t', bool t is set to true
		else if (strcmp(argv[i],"-t") == 0 && t == false) 
		{
			t = true;
		}
		// If argument '-t' appears again, error is printed
		else if (strcmp(argv[i],"-t") == 0 && t == true) 
		{
			cout << "ERROR: Repetition of option '-t'" << endl;
			return 0;
		}
		else 
		{
			// Converts the argument to a long, if it fails it is checked to see if its a file
			char * temp;
			long to_integer = strtol(argv[i], &temp, 10);
			
			// Argument is an integer
			if (to_string(to_integer) == argv[i])
			{
					bufferSize = to_integer;		
			}
			// Argument is not an integer or option
			else
			{
				// If file1Exists is false, and argv is not "proj03", then its file1
				if (file1Exists == false && strcmp(argv[i],"proj03") != 0) 
				{
					file1Exists = true;
					file1 = argv[i];
				} 
				// If file2Exists is false, and argv is not "proj03", then its file2
				else if (file2Exists == false && strcmp(argv[i],"proj03") != 0 )
				{
					file2Exists = true;
					file2 = argv[i];
				}
				// If more than two files are inputted, error printed
				else if(file1Exists == true && file2Exists == true)
				{
					cout << "ERROR: More than two files inputted" << endl;
					return 0;
				}
			}
		}

	}
	
	// If '-a' and '-t' are both entered, error printed
	if (a == true && t == true) 
	{
		cout << "ERROR: '-a' and '-t' can't coexist" << endl;
		return 0;
	}	
	// If one of the files is false, then error printed
	if (file1Exists == false || file2Exists == false)
	{
		cout << "ERROR: Less than two files inputted" << endl;
		return 0;
	}
	// If argument '-b' inputted, but buffer size is <1, print error
	if (b == true) 
	{
		if (bufferSize < 1) 
		{
			cout << "ERROR: Buffer size is less than 1" << endl;
			return 0;
		}
	}	
	// If argument '-b' is not inputted, then buffer size is 256
	else if (b == false) 
	{
		bufferSize = 256;
	}

	// Exactly two files inputted
	if (file1Exists == true && file2Exists == true) 
	{
		char * buf[bufferSize];
		int fd[10];

		// Opens the source file
		fd[0] = open(file1, O_RDWR);

		// If source file does not exist in the directory, error is printed
		if (fd[0] == -1)
		{
			cout << "ERROR: Unable to open source file" << endl;
			return 0;
			
		}
		// Source file exists
		else
		{
			// If source and destination files are the same
			if (strcmp(file1, file2) == 0)
			{	
				// If '-a' or '-t' are true, then print error
				if (a == true || t == true)
				{
					cout << "ERROR: Cannot append or truncate a file to itself" << endl;
					return 0;
				}
			}
			// Source and destination files are different
			else
			{	
				// Destination file is opened
				fd[1] = open(file2, O_RDWR);
				
				// If destination file does not exist before execution
				if(fd[1] == -1)
				{	
					// Destination file created with read and write permissions
					fd[1] = open(file2, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);	
									
					// Entire source file is read and wrote to the destination file
					fd[2] = read(fd[0], buf , bufferSize);
					while(fd[2] !=0)
					{
						// If bytes < buffer size then byte count is written
						if (fd[2] < bufferSize)
						{
							write(fd[1],buf,fd[2]);
						}
						// Else the buzzer size is written to the destination file
						else
						{
							write(fd[1],buf,bufferSize);
						}
						fd[2] = read(fd[0], buf , bufferSize);
					}
				}
				// The destination file exists before execution
				else 
				{
					// '-a' is entered
					if (a == true)
					{
						fd[1] = open(file2, O_RDWR | O_APPEND);
						// Entire source file is read and appened to the destination file
						fd[2] = read(fd[0], buf , bufferSize);
						while(fd[2] !=0)
						{
							// If bytes < buffer size then byte count is written
							if (fd[2] < bufferSize)
							{
								write(fd[1],buf,fd[2]);
							}
							// Else the buzzer size is written to the destination file
							else
							{
								write(fd[1],buf,bufferSize);
							}
							fd[2] = read(fd[0], buf , bufferSize);
						}
					}
					// '-t' is entered
					else if(t==true)
					{
						fd[1] = open(file2, O_RDWR | O_TRUNC);
						// Entire source file is read and truncated to the destination file
						fd[2] = read(fd[0], buf , bufferSize);
						while(fd[2] !=0)
						{
							// If bytes < buffer size then byte count is written
							if (fd[2] < bufferSize)
							{
								write(fd[1],buf,fd[2]);
							}
							// Else the buzzer size is written to the destination file
							else
							{
								write(fd[1],buf,bufferSize);
							}
							fd[2] = read(fd[0], buf , bufferSize);
						}
					}
				}
			}
		}
		close(fd[0]);
		close(fd[1]);
	}
}


