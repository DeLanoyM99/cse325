/*********************************************************************
    Abhinav Thirupathi
    Computer Project #6
**********************************************************************/

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <pthread.h> 
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <map> 
#include <vector>
#include <iomanip>
using namespace std;


int bufferSize = 5;  	// Size of bounded buffer
int in = 0;				// Next location to put item
int out = 0;			// Next location to get item

sem_t semProducer;			// Sync producer and consume (no. of empty slots)
sem_t semConsumer;			// Sync producer and consumer (no. of consumable items)
sem_t semCriticalSection;	// Enforcing mutual exclusion when accessing buffer

// Struct for the transaction 
struct Transaction
{
	int accountNumber = 0;	 // Account Number
	string type = "";		 // Type of transaction
	double amount = 0.0;	 // Transaction amount
	int thread; 			 // Thread number where the transaction originated
};

array<Transaction, 5> buffer; // Circular Buffer of size bufferSize


/*-------------------------------------------------------------------- 
	Name: pOptionHandler
    Purpose:  Handles and error checks the inputted number for p
    Receive:  The inputted number of p threads
    Return:   Return 1, if default
---------------------------------------------------------------------*/
int pOptionHandler(int p)
{
	int pResult = p;
	if (pResult > 0 && pResult < 11)
	{
		return pResult;
	}
	cout << "Warning: Invalid number of producers. Default to 1" << endl; 
	return 1;
}

/*-------------------------------------------------------------------- 
	Name: bOptionHandler
    Purpose:  Handles and error checks the inputted number for b
    Receive:  The inputted size of the buffer
    Return:   None
---------------------------------------------------------------------*/
void bOptionHandler(int b)
{
	
	int bResult = b;
	if (bResult > 0 && bResult < 21)
	{
		bufferSize = bResult;
		return;
	}
	cout << "Warning: Invalid buffer size. Default to 5" << endl;
	bufferSize = 5;
	return;
}

/*-------------------------------------------------------------------- 
	Name: producerThread
    Purpose:  Reads the transactions and adds it to the buffer
    Receive:  Path to the transaction file
    Return:   None
---------------------------------------------------------------------*/
void * producerThread(void *pt)
{
	// Converts pt into string and creates a filename
	char *path = (char*)(pt);
  	
  	ifstream file;		// Transaction file to be read
	file.open(path);
		
	// If the file cannot be read, error printed
	if (!file.is_open())
	{
		cout << "ERROR: Cannot open transaction file" << endl;
		pthread_exit( NULL );
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
		
		// Creates a Transaction object
		Transaction trans{ std::stoi(wordVec[0]), wordVec[1], std::stod(wordVec[2]), -1 };
		
		// Stores the thread number in the Transaction object
		if (path[5] == '0')
		{
			trans.thread = 0;
		} else if (path[5] == '1')
		{
			trans.thread = 1;
		} else if (path[5] == '2')
		{
			trans.thread = 2;
		} else if (path[5] == '3')
		{
			trans.thread = 3;
		} else if (path[5] == '4')
		{	
			trans.thread = 4;
		} else if (path[5] == '5')
		{
			trans.thread = 5;
		} else if (path[5] == '6')
		{
			trans.thread = 6;
		} else if (path[5] == '7')
		{
			trans.thread = 7;
		} else if (path[5] == '8')
		{
			trans.thread = 8;
		} else if (path[5] == '9')
		{
			trans.thread = 9;
		} 
		
		sem_wait( &semProducer );
		sem_wait( &semCriticalSection );
		
    	buffer[in] = trans;
    	in = (in+1) % bufferSize;

    	sem_post( &semCriticalSection );
		sem_post( &semConsumer );
	}
  	pthread_exit( NULL );
}


/*-------------------------------------------------------------------- 
	Name: consumerThread
    Purpose:  Updates the account balance and writes to 'accounts.log'
    Receive:  Map of the accounts
    Return:   None
---------------------------------------------------------------------*/
void * consumerThread(void * accountsMap)
{
	// Converts accountsMap to a map of accounts
	using T = std::map<int, float>;
   	T& accounts = *static_cast<T*>(accountsMap);
   	
   	// Create/open a new file named "accounts.log"
	ofstream newFile("accounts.log");
   	
   	newFile << "Thread" << "  Account Number" << "  Transaction Type" << "  Current Balance" << "  Transaction Amount" << "  Updated Balance" << endl;
   	
   	// Initial Balance of the account
   	float initialBalance = 0.0;
   	
	while (buffer.empty() != true)
	{
		sem_wait( &semConsumer );
		sem_wait( &semCriticalSection );
		
		// Retrieve the transaction from the buffer
		Transaction item = buffer[out];
		out = (out+1) % bufferSize;
		
		sem_post( &semCriticalSection);
		sem_post( &semProducer);
		
		// Processes the transaction
		if(item.type == "withdraw")
		{
			initialBalance = accounts[item.accountNumber];
			accounts[item.accountNumber] -= item.amount;
		}
		else if (item.type == "deposit")
		{
			initialBalance = accounts[item.accountNumber];
			accounts[item.accountNumber] += item.amount;
		}
		// Writes to the 'accounts.log' file
		newFile << "     " << item.thread << "            " << item.accountNumber << "          " << item.type << "         " << std::setprecision(2) << fixed << initialBalance << "           " << item.amount << "             " << accounts[item.accountNumber] << std::endl; 
	
	}
	pthread_exit( NULL );
}

/*-------------------------------------------------------------------- 
	Name: main
    Purpose:  Processes the command line argument and calls correct functions
    Receive:  The number command line arguments, and array of the arguments
    Return:   Return 0
---------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
	
	int p = 1;	// Number of producer threads
		
	// Loops through the command line argument left to right
	for (int i = 1; i < argc; ++i) 
	{
		// If option is '-p', producer threads passed in
		if (strcmp(argv[i],"-p") == 0)
		{
			if (i < (argc-1))
			{
				if (atoi(argv[i+1]) != 0)
				{
					p = pOptionHandler(atoi(argv[i+1]));
					i++;
				}
				else
				{
					cout << "Warning: No producer thread inputted. Default to 1" << endl;
				}
			} 
			else
			{
				cout << "Warning: No producer thread inputted. Default to 1" << endl;
			}
		}
		// If option is '-s', size of bounded buffer passed in
		else if (strcmp(argv[i],"-b") == 0)
		{
			if (i < (argc-1))
			{
				if (atoi(argv[i+1]) != 0)
				{
					bOptionHandler(atoi(argv[i+1]));
					i++;
				}
				else
				{
					cout << "Warning: No buffer bound size inputted. Default to 5" << endl;
				}
			}
			else
			{
				cout << "Warning: No buffer bound size inputted. Default to 5" << endl;
			}
		}
		// If no arguments were inputted
		else if (argc == 1)
		{
			cout << "ERROR: No arguments" << endl;
			return 0;
		}
		// Invalid options will warn the user
		else 
		{
			cout << "Warning: Invalid option " << argv[i] << endl;
			return 0;
		}
	}	
	
	ifstream file;		// File to be read
	file.open("accounts.old"); // Tries to open 'accounts.old'

	
	// If the file cannot be read, error printed
	if (!file.is_open())
	{
		cout << "ERROR: Cannot open accounts.old" << endl;
		return 0;
	}

	std::map<int, float> accounts; // Dictionary to store the accountsfdre32	
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
		
		// Inserts the pair of account number and balance to the accounts dict
		accounts.insert(pair<int, float>(stoi(wordVec[0]), stof(wordVec[1])));
	}
	
	// Initialize buffer to the buffer size
	buffer[bufferSize];
	
	// Initializes the processes
	pthread_t producerThreadInitial[p];
	pthread_t consumerThreadInitial;
	
	// Initialized the semaphores
  	sem_init( &semProducer, 0, bufferSize);
  	sem_init( &semConsumer, 0, 0 );
  	sem_init( &semCriticalSection, 0, 1);
  	
  	 // Create the consumer thread
	if (pthread_create( &consumerThreadInitial, NULL, consumerThread, static_cast<void*>(&accounts)))
    {
    	cout << "Error creating consumer thread" << endl;
      	exit( -1 );
    }
    
    // Creates the producer threads
  	for (int y=0; y<p; y++)
  	{
  		string transFile = "trans" + std::to_string(y);

    	if (pthread_create( &producerThreadInitial[y], NULL, producerThread, (void *) transFile.c_str()) )
    	{
    		cout << "Error creating producer thread" << endl;
      		exit( -1 );
    	}
    	if (pthread_join( producerThreadInitial[y], NULL ))
    	{
    		cout << "Error joining producer threads" << endl;
      		exit( -2 );
    	}
  	}
	
	// Create/open a new file named "accounts.new"
	ofstream newFile("accounts.new");
	
	// Writes the updated accounts to the new file "accounts.new"
	for(auto& x : accounts)
	{
		newFile << x.first << " " << std::setprecision(2) << fixed << x.second << std::endl;
	}
	newFile.close();
}

