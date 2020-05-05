/*********************************************************************
    Abhinav Thirupathi
    Computer Project #7
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
	unsigned short Vbit = 0;			// V bit
	unsigned short Mbit = 0;			// M bit 
	unsigned int tagBits = 0;			// tag bits (13 bits wide)
	std::array<unsigned int, 16> dataBlock;	 // data block (16 bits wide)
};

std::array<Record, 8> dataCache; // Array of records making up a data cache


/*-------------------------------------------------------------------- 
	Name: displayContents
    Purpose:  Displays the contents of the data cache
    Receive:  None
    Return:   None
---------------------------------------------------------------------*/
void displayContents()
{
	// Prints the header
	printf("     %s %s %s  %s\n", "V", "M", "Tag", "Block Contents");
	printf("     -------- -----------------------------------------------\n");
	
	// Prints the contents of the data cache
	for(int z = 0; z < 8; z++)
	{
		// Retrieves the data from the data cache and prints it out
		Record recOut = dataCache[z] ; 	
		printf("[%1x]: %1x %1x %04x ", z,recOut.Vbit,recOut.Mbit,recOut.tagBits);
		
		// Prints the data block
		for(int y = 0; y < 16; y++)
		{
			printf("%02x ",recOut.dataBlock[y]);
		}
		printf("\n");
	}
}


/*-------------------------------------------------------------------- 
	Name: debugMode
    Purpose:  Processes the file in debug mode
    Receive:  The filename to be processed
    Return:   None
---------------------------------------------------------------------*/
void debugMode(char* arg)
{
	ifstream file;		// File to be read
	file.open(arg);
		
	// If the file cannot be read, error printed
	if (!file.is_open())
	{
		cout << "ERROR: Cannot open " <<arg<< endl;
		return;
	}
			
	// Displays the contents of the data cache BEFORE execution
	displayContents();
			
	string line;		// Line in the file
			
	// Loops through of the file
	while (std::getline(file, line))
    {			
		// Iterates through each word in the line
		stringstream s(line);
		string word;

		vector <string> wordVec; // Vector of the words
		
		unsigned int address; 	//Physical address being referenced
		
		unsigned int tag;		//Tag bits in the address
		unsigned int cacheLineAccessed;		//Cache Line Accessed
		unsigned int offset;	//Offset bits in the address (Rightmost hex digit)
		unsigned int nonOffset; //Rightmost 4 hexadecimal digits
		
		int count = 0;
		
		// Makes a vector from a line string
		while (s >> word)
		{	
			// Converts the physical address into a hex and stores it
			if(count == 0)
			{
				std::stringstream temp; 
				temp << std::hex << word;
				temp >> address;
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
			offset = (address & 0x0000F);			
			nonOffset = (address & 0xFFFF0) >> 4; 
			
			tag = nonOffset / 8;
			cacheLineAccessed = nonOffset % 8;
			
			printf( "%05x %s %04x %01x %01x\n", address, wordVec[1].c_str(), tag, cacheLineAccessed, offset);
			
				// Displays the contents of the data cache
	displayContents();
		}
	}
}

/*-------------------------------------------------------------------- 
	Name: notDebugMode
    Purpose:  Processes the file in non debug mode
    Receive:  The filename to be processed
    Return:   None
---------------------------------------------------------------------*/
void notDebugMode(char* arg)
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
	
	string line;		// Line in the file
			
	// Loops through of the file
	while (std::getline(file, line))
    {			
		// Iterates through each word in the line
		stringstream s(line);
		string word;

		vector <string> wordVec; // Vector of the words
		
		unsigned int address; 	//Physical address being referenced
		
		unsigned int tag;		//Tag bits in the address
		unsigned int cacheLineAccessed;		//Cache Line Accessed
		unsigned int offset;	//Offset bits in the address (Rightmost hex digit)
		unsigned int nonOffset; //Rightmost 4 hexadecimal digits
		
		int count = 0;
		
		// Makes a vector from a line string
		while (s >> word)
		{	
			// Converts the physical address into a hex and stores it
			if(count == 0)
			{
				std::stringstream temp; 
				temp << std::hex << word;
				temp >> address;
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
			offset = (address & 0x0000F);			
			nonOffset = (address & 0xFFFF0) >> 4; 
			
			tag = nonOffset / 8;
			cacheLineAccessed = nonOffset % 8;
			
			printf( "%05x %s %04x %01x %01x\n", address, wordVec[1].c_str(), tag, cacheLineAccessed, offset);
		}
	}
	
	// Creates a record object and inserts it into the data cache
	for(int z = 0; z < 8; z++)
	{
		Record recIn{ 0, 0, 12, 0 };
		dataCache[z] = recIn; 		
	}
	
	// Displays the contents of the data cache AFTER execution
	displayContents();
	
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
	// Calls debugMode function if it is debug
	else if (isDebug == true && refs == true)
	{
		debugMode(file);
	}
	// Calls notDebugMode function if it is not debug
	else if (isDebug == false && refs == true)
	{
		notDebugMode(file);
	}
}

