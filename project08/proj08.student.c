/*********************************************************************
    Abhinav Thirupathi
    Computer Project #8
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

std::array<Record, 8> dataCache; // Array of records for data cache (upper level)
std::array<unsigned int, 1048576> RAM; // Array for the RAM (lower level)

/*-------------------------------------------------------------------- 
	Name: displayDataCache
    Purpose:  Displays the contents of the data cache
    Receive:  None
    Return:   None
---------------------------------------------------------------------*/
void displayDataCache()
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
	printf("\n");
}

/*-------------------------------------------------------------------- 
	Name: displayRAM
    Purpose:  Displays the contents of the RAM
    Receive:  None
    Return:   None
---------------------------------------------------------------------*/
void displayRAM()
{
	// Prints the contents of the RAM starting at address 20000 to 20080
	for (unsigned int z = 0x20000; z < 0x20080; z+=16) 
	{
		// Prints the address
		printf("%5x: ", z);
			
		// Prints the RAM contents
		for(int y = 0; y < 15; y++)
		{
			printf("%02x ", RAM[z+y]);
		}
		printf("\n");	
	}
}


/*-------------------------------------------------------------------- 
	Name: fileProcessor
    Purpose:  Processes the file to compelete the operations
    Receive:  The filename to be processed, and if it is in debug mode
    Return:   None
---------------------------------------------------------------------*/
void fileProcessor(char* arg, bool isDebug)
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
	displayDataCache();
			
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
		unsigned int index;		//Cache Line Accessed
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
			index = nonOffset % 8;
			
			unsigned int ramAddress = (tag << 7) + (index << 4);
			
			// Data Cache Operations
		
			string missHit = "M";	// Default for result of access is Miss
			
			// If V bit is 1 and tags match then it is Hit
			if (dataCache[index].Vbit == 1 && dataCache[index].tagBits == tag)
			{
				missHit = "H";
			}

			//If not ( cache line accessed V bit is 1 and the line's tag is tag)
			if(!(dataCache[index].Vbit == 1 && dataCache[index].tagBits == tag))
			{
				//If cache line accessed V bit is 1 and Mbit is 1
				if (dataCache[index].Vbit == 1 && dataCache[index].Mbit == 1)
				{
					//Copies old block from cache[index] to lower level (RAM)
				
					// Old block RAM address
					unsigned int oldBlockRamAddress = (dataCache[index].tagBits << 7) + (index << 4);
				
					for(int a = 0; a<16; a++)
					{
						RAM[oldBlockRamAddress+a] = dataCache[index].dataBlock[a];
					}
				}
			
				//Copies new block from lower level (RAM) to cache[index]
				for(int a = 0; a<16; a++)
				{
					dataCache[index].dataBlock[a] = RAM[ramAddress+a];
				}
				
				dataCache[index].Mbit = 0;
				dataCache[index].tagBits = tag;
				dataCache[index].Vbit = 1;
			}
			
			if (wordVec[1] == "R")
			{	
				//Copies item from cache[index] data block to CPU register (output)
			
				auto cacheBlock = dataCache[index].dataBlock;

				printf( "%05x %s %04x %01x %01x %s %02x %02x %02x %02x\n\n", address, wordVec[1].c_str(), tag, index, offset, missHit.c_str(), cacheBlock[offset],cacheBlock[offset+1], cacheBlock[offset+2], cacheBlock[offset+3]);
			}
			else if (wordVec[1] == "W")
			{
			
			vector <unsigned int> CPURegisterHex; 
			
			// Converts the four bytes of data (CPU register) into hex values
			for (int b = 0; b<4; b++)
			{
				unsigned int q1;
				std::stringstream w1; 
				w1 << std::hex << wordVec[2+b];
				w1 >> q1;
				CPURegisterHex.push_back(q1);
			}
			
				//Copies item from CPU register to cache[index] data block 
				for (int c = 0; c<4; c++)
				{
					dataCache[index].dataBlock[offset+c] = CPURegisterHex[c];
				}
				dataCache[index].Mbit = 1;
				
				// Prints out the memory reference info after processing
				printf( "%05x %s %04x %01x %01x %s %02x %02x %02x %02x\n\n", address, wordVec[1].c_str(), tag, index, offset, missHit.c_str(), CPURegisterHex[0], CPURegisterHex[1], CPURegisterHex[2], CPURegisterHex[3]);
			}	
			
			// Displays the contents of the data cache every time if debug mode is truw
			if (isDebug == true)
			{	
				displayDataCache();
			}
		}
	}
	
	// Displays the contents of the data cache ONCE if debug mode is false
	if (isDebug == false)
	{	
		displayDataCache();
	}
	
	// Displays the RAM	
	displayRAM();
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
		fileProcessor(file, true);
	}
	// Calls fileProcessor function in not debug mode
	else if (isDebug == false && refs == true)
	{
		fileProcessor(file, false);
	}
}

