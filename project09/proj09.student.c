/*********************************************************************
    Abhinav Thirupathi
    Computer Project #9
**********************************************************************/

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <iomanip>
using namespace std;


// Struct for the Record
struct Record
{
	unsigned short Vbit = 0;	// V bit ('0' for not valid, '1' valid)
	unsigned short Pbit = 0;	// P bit ('0' for not present, '1' present)
	unsigned short Rbit = 0;  // R bit ('0' for not referenced, '1' referenced)
	unsigned short Mbit = 0;	// M bit ('0' for not modified, '1' modified)
	unsigned int frameNum = 0;	// Frame number (two hex digits)
};

std::array<Record, 16> pageTable; // Array of records making up a page table


/*-------------------------------------------------------------------- 
	Name: displayContents
    Purpose:  Displays the contents of the pageTable
    Receive:  None
    Return:   None
---------------------------------------------------------------------*/
void displayContents()
{
	// Prints the header
	printf("\n     %s %s %s %s  %s\n", "V", "P", "R", "M","Frame");
	printf("     -------  -----\n");
	
	// Prints the contents of the data cache
	for(int i = 0; i < 16; i++)
	{
		// Retrieves the data from the data cache and prints it out
		Record rec = pageTable[i] ; 	
		printf("[%1x]: %01x %01x %01x %01x   %02x\n", i,rec.Vbit,rec.Pbit,rec.Rbit,rec.Mbit,rec.frameNum);
	}
	printf("\n");
}


/*-------------------------------------------------------------------- 
	Name: processFile
    Purpose:  Processes the file 
    Receive:  The filename to be processed and isDebug or not
    Return:   None
---------------------------------------------------------------------*/
void processFile(char* arg, bool isDebug)
{
	ifstream file;		// File to be read
	file.open(arg);
		
	// If the file cannot be read, error printed
	if (!file.is_open())
	{
		cout << "ERROR: Cannot open " << arg << endl;
		return;
	}

	// Displays the contents of the data cache BEFORE execution
	displayContents();
	
	int memoryRefsCount = 0;	// Total number of memory references
	int readOpsCount = 0;		// Total number of read operations
	int writeOpsCount = 0;		// Total number of write operations
	
	string line;		// Line in the file
			
	// Loops through of the file
	while (std::getline(file, line))
    {			
		// Iterates through each word in the line
		stringstream s(line);
		string word;

		vector <string> wordVec; // Vector of the words
		
		unsigned int vAddress; 	//Virtual address being referenced
		unsigned int pageNum;	//Page Number (Leftmost 1 hex digit)
		unsigned int offset;	//Offset bits (Rightmost 3 hex digits)
		
		int count = 0;
		
		// Makes a vector from a line string
		while (s >> word)
		{	
			// Converts the physical address into a hex and stores it
			if(count == 0)
			{
				std::stringstream temp; 
				temp << std::hex << word;
				temp >> vAddress;
				wordVec.push_back(word);
			}
			else
			{
				wordVec.push_back(word);
			}
			count++;
		} 
			
		// Further processing if the line is not empty
		if (wordVec.size() != 0)
		{
			// Mask and shift to get the bits
			offset = (vAddress & 0x0FFF);			
			pageNum = (vAddress & 0xF000) >> 12; 
			
			printf( "%04x %s %01x %03x\n", vAddress, wordVec[1].c_str(), pageNum, offset);
		}
		
		// Keeps count of memory references and write/read operations
		if (wordVec[1] == "R")
		{
			readOpsCount++;
		} else if (wordVec[1] == "W")
		{
			writeOpsCount++;
		}
		memoryRefsCount++;
		
		// Displays the contents of the page table every time if debug mode is true
		if(isDebug == true)
		{
			displayContents();
		}
	}
	
	//Displays the contents of the page table ONCE if debug mode is false
	if(isDebug == false)
	{
		displayContents();
	}
	
	// Displays the counts of memory references, read, and write operations
	printf( "Total number of memory references: %d\n", memoryRefsCount);
	printf( "Total number of read operations:   %d\n", readOpsCount);
	printf( "Total number of write operations:  %d\n\n", writeOpsCount);
	
}

/*-------------------------------------------------------------------- 
	Name: main
    Purpose:  Processes the command line argument and calls correct functions
    Receive:  The number command line arguments, and array of the arguments
    Return:   Return 0
---------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
	bool isDebug = false;	// Boolean for is debug or not debug
	bool refs = false;		// Boolean for refs inputted
	bool fileExists = false;// Boolean for if file exists
	char* file;				// Stores the inputted file
	
	// Loops through the command line argument left to right
	for (int i = 1; i < argc; ++i) 
	{
		// If option is '-debug', isDebug is true
		if (strcmp(argv[i],"-debug") == 0)
		{
			isDebug = true;
		}
		// If option is '-refs', refs is made true
		else if (strcmp(argv[i],"-refs") == 0)
		{
			refs = true;
		}
		// If argument doesn't start with '-', assumes file
		else if (argv[i][0] != '-')
		{
			// Filename accepted if it followed '-refs', else error
			if (refs == true)
			{
				file = argv[i];
				fileExists = true;
			}
			else
			{
				cout << "ERROR: File without '-refs'" << endl;
				return 0;
			}
		}
		// Invalid options will warn the user
		else 
		{
			cout << "Warning: Invalid option " << argv[i] << endl;
			return 0;
		}
	}
	
	// Error if '-refs' is not present in the command line
	if(refs == false)
	{
		cout << "ERROR: '-refs' is not present" << endl;
		return 0;
	}
	// Error if no file is inputted
	else if (fileExists == false)
	{
		cout << "ERROR: No file inputted" << endl;
		return 0;
	}
	// Calls processFile function in debug mode
	else if (isDebug == true && refs == true)
	{
		processFile(file, true);
	}
	// Calls processFile function in not debug mode
	else if (isDebug == false && refs == true)
	{
		processFile(file, false);
	}
}
