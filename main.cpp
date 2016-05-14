#include <iostream>
#include <string.h>	//strtok() function
#include "rshell.h"

using namespace std;

int main()
{
	string input;
	
	while (input != "exit" && input != "logout") 
	{
		Terminate *line = new Terminate(input);
		
		line->parse();
		line->run();
		
		Command *cmd = new Command();
	
		cmd->display();
		
		getline(cin, input);
	}
	
	return 0;
}
