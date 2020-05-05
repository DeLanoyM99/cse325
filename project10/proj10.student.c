/*********************************************************************
    Abhinav Thirupathi
    Computer Project #10
**********************************************************************/

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;


// Struct for the Record
struct Record
{
	unsigned short Vbit = 0;	// V bit ('0' for not valid, '1' valid)
	unsigned short Pbit = 0;	// P bit ('0' for not present, '1' present)
	unsigned short Rbit = 0;    // R bit ('0' for not referenced, '1' referenced)
	unsigned short Mbit = 0;	// M bit ('0' for not modified, '1' modified)
	unsigned int frameNum = 0;	// Frame number (two hex digits)
};

std::array<Record, 16> pageTable; // Array of records making up page table (upper)

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
		printf("[%1x]: %01x %01x %01x %01x  %02x\n", i,rec.Vbit,rec.Pbit,rec.Rbit,rec.Mbit,rec.frameNum);
	}
	printf("\n");
}


vector <unsigned int> freeFrameVec; // Vector of the free frame list
vector <unsigned int> validPageVec; // Vector of the valid pages

/*-------------------------------------------------------------------- 
	Name: processConfigFile
    Purpose:  Processes the config file 
    Receive:  None
    Return:   Return true, else false if open fails
---------------------------------------------------------------------*/
bool processConfigFile()
{
	string configFile = "config";
	
	ifstream file;		// File to be read
	file.open(configFile);
		
	// If the file cannot be read, error printed
	if (!file.is_open())
	{
		cout << "ERROR: Cannot open config file" << endl;
		return false;
	}
	
	string line;		// Line in the file
	int count = 0;		// Count of the lines
	
	// Loops through of the file
	while (std::getline(file, line))
    {			
		// Iterates through each word in the line
		stringstream s(line);
		string word;
	
		std::vector<string> tempVec;
			
		// Makes a vector from a line string
		while (s >> word)
		{	
			// Stores the last two line of the config in temp. vector
			if(count > 0)
			{	
				tempVec.push_back(word);
			}
		} 
		
		// Converts the values to hex and adds to the appropriate vectors
		if (count > 0)
		{
			// Initializes the V bit in the page table
			for(int a = 1; a <= stoi(tempVec[0]); a++)
			{
				std::stringstream temp;
				temp << tempVec[0+a];
				unsigned int hexNum;
				temp >> std::hex >> hexNum;

				if(count == 1)
				{
					freeFrameVec.push_back(hexNum);
				}
				else if (count == 2)
				{
					validPageVec.push_back(hexNum);
					pageTable[hexNum].Vbit = 1;
				}
			}
		}
		count++;
	}
	return true;
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
	
	int readOpsCount = 0;		// Total number of read operations
	int writeOpsCount = 0;		// Total number of write operations
	int pageFaultCount = 0; 	// Total number of page faults
	int writeBackCount = 0;		// Total number of write backs
	std::vector<unsigned int> LRU; 	// LRU for the pages used
 	
	string line;		// Line in the file
			
	// Loops through of the file
	while (std::getline(file, line))
    {			
		// Iterates through each word in the line
		stringstream s(line);
		string word;

		std::vector <string> wordVec; // Vector of the words
		
		unsigned int vAddress; 		//Virtual address being referenced
		unsigned int pAddress;		//Physical address 
		unsigned int pageNum;		//Page Number (Leftmost 1 hex digit)
		unsigned int offset;		//Offset bits (Rightmost 3 hex digits)
		string pageFaultFlag = "";	//Page Fault Flag
		string writeBackFlag = "";	//Write Fault Flag
		
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
			
			// Tries to find the LRU and if it exists moves it to the end
			auto LRU_find = std::find(LRU.begin(), LRU.end(), pageNum);
			if (LRU_find != LRU.end())
			{
				LRU.erase(LRU_find);
				LRU.push_back(pageNum);
			}
			else 
			{
				LRU.push_back(pageNum);
			}
			
			// Checks for invalid page number
			if(pageTable[pageNum].Vbit == 0)
			{
				printf( "ERROR: Invalid memory reference '%04x'\n", vAddress);
				if(wordVec[1] == "R")
				{
					readOpsCount++;
				}
				else if (wordVec[1] == "W")
				{
					writeOpsCount++;
				}
				displayContents();
				continue;
			}
			else
			{	
				// Checks for page fault
				if(pageTable[pageNum].Vbit == 1 && pageTable[pageNum].Pbit == 0)
				{
					// Sets the page fault flag and increments the count
					pageFaultFlag = "F";	
					pageFaultCount++;		
					
					// Check for write back
					if (freeFrameVec.empty() == true)
					{
						//Selects the top of the LRU as the victim page
						unsigned int victimPageNum = LRU[0];
						LRU.erase(LRU.begin());
						
						// Check for write back of the victim page
						if(pageTable[victimPageNum].Mbit == 1)
						{
							//Sets the write back flag and increments by 1
							writeBackFlag = "B";
							writeBackCount++;	
						}
						
						pageTable[victimPageNum].Pbit = 0;
						
						// Moves the victim's frame to the end of free frame list
						freeFrameVec.push_back(pageTable[victimPageNum].frameNum);
					}
					// Allocates frame from free frame list to the desired page
					unsigned int frame = freeFrameVec[0];
					freeFrameVec.erase(freeFrameVec.begin());
					
					// Sets the appropriate bits
					pageTable[pageNum].Pbit = 1;
					pageTable[pageNum].Rbit = 0;
					pageTable[pageNum].Mbit = 0;
					pageTable[pageNum].frameNum = frame;
				}
				
				// Read and write operations
				if(wordVec[1] == "R")
				{
					pageTable[pageNum].Rbit = 1;
					readOpsCount++;
				}
				else if (wordVec[1] == "W")
				{
					pageTable[pageNum].Rbit = 1;
					pageTable[pageNum].Mbit = 1;
					writeOpsCount++;
				}
				
				// Composes the 12-bit physical address from the frame number and offset
				pAddress = (pageTable[pageNum].frameNum << 12) + offset;
			}
		}
		
		// Displays the memory reference after processing 
		printf( "%04x %s %01x %03x %s %s %05x\n", vAddress, wordVec[1].c_str(), pageNum, offset, pageFaultFlag.c_str(), writeBackFlag.c_str(), pAddress);

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
	
	// Displays the counts of read and write operations, write backs, page faults
	printf( "Total number of read operations:   %d\n", readOpsCount);
	printf( "Total number of write operations:  %d\n", writeOpsCount);
	printf( "Total number of page faults: %d\n", pageFaultCount);
	printf( "Total number of write backs: %d\n\n", writeBackCount);
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
	// Calls fileProcessor function in debug mode
	else if (isDebug == true && refs == true)
	{
		bool t = processConfigFile();
		if (t == false)
		{
			return 0;
		}
		processFile(file, true);
	}
	// Calls fileProcessor function in not debug mode
	else if (isDebug == false && refs == true)
	{
		bool t = processConfigFile();
		if (t == false)
		{
			return 0;
		}
		processFile(file, false);
	}
}
