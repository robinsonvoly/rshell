#include <iostream>
#include <string.h>	//strtok() function
#include "rshell.h"

using namespace std;

int main()
{
	string input;
	Command *cmd = new Command();
	
	while (input != "exit" && input != "logout") 
	{
		Terminate *line = new Terminate(input);
		
		line->parse(input, 0);
		
		line->run();
	
		cmd->display();
		
		getline(cin, input);
	}
	
	return 0;
}
