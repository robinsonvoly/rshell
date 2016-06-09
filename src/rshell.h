#ifndef _RSH_H_
#define _RSH_H_

#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h> //waitpid
#include <sys/wait.h> //waitpid
#include <unistd.h> //execve, pid_t, getlogin, gethostname
#include <stdlib.h>  
#include <stdio.h> //perror

#include <sys/stat.h> //stat
#include <sys/types.h>
#include <errno.h> //errno
#include <fcntl.h> //file types

int fds[2];

/* Helper Functions */
bool fileExists(const char* file)   // Checks if file exists
{
    struct stat buf;
    return (stat(file, &buf) == 0);
}

bool fileReg(const char* file)      // Checks if file is regular
{
    struct stat buf;
    stat(file, &buf);
    return S_ISREG(buf.st_mode);
}

bool fileDir(const char* file)      // Checks if file is a directory
{
    struct stat buf;
    stat(file, &buf);
    return S_ISDIR(buf.st_mode);
}

/* Base Class */
class CommandLine 
{
	public:
		CommandLine() { };
		virtual int run() = 0;
};

/* Leaf Class */
class Command : public CommandLine
{
	protected:
		char *arg[64]; // Command and Argument or Flag list
		std::vector<std::string> vec;
		
	public:
		Command() { }
		
		Command(std::vector<std::string> vec) : vec(vec) { }
		
		void display() // Extra credit: gethostname, getlogin 
		{ 
			char hostname[1024];
			char username[1024];
			
			gethostname(hostname, 1024);
			getlogin_r(username, 1024);
			
			std::cout << "[" << username << "@" << hostname << "]" << "$ ";
		}; 
		
		int run() // Executes the command
		{
			pid_t pid = fork(); // Fork and get child pid
			int err = 0;
			
			if (pid == -1) // If pid is -1, error and fork() has failed
			{
			    exit(-1);
			} 
			else if (pid > 0) // If pid is positive and greater than 0, wait 
			//for child process
			{
			    int status;
			    waitpid(pid, &status, 0);
			}
			else // If pid is 0, execute command in child process
			{
				unsigned int i;
				unsigned int j = 100;
				unsigned int k = 100;
				bool outredirect = 0;
				bool outredirecta = 0;
				bool inredirect = 0;
				bool piping;
				
				for (unsigned int a = 0; a < vec.size(); ++a)
				{
					if (vec.at(a) == ">>") // Check for i/o redirection
					{
						outredirecta = 1;
						k = a + 1;
					}
					else if (vec.at(a) == ">") 
					{
						outredirect = 1;
						k = a + 1;
					}
					else if (vec.at(a) == "<")
					{
						inredirect = 1;
						j = a + 1;
					}
				}
				
				for (i = 0; i < vec.size(); ++i) //Create argv array
				{
					if (vec.at(i) == "|")
					{
						piping = 1;
						++i;
						break;
					}
					else if (i != j - 1 && i != k - 1 && i != k && i != j)
					{
						// std::cout << vec.at(i) << std::endl;
						this->arg[i] = const_cast<char*>(vec.at(i).c_str());
						// Add arg if not i/o redirect
					}
				}
				this->arg[i] = NULL;

				int fdi, fdo; // File directory for i/o
				int savestdout, savestdin; // Save original stdin, stdout
				int fds[2];
				
				if ((savestdout = dup(1)) < 0) exit(1); // Save original
				if ((savestdin = dup(0)) < 0) exit(1);
				
				if (outredirect)
				{
					if ((fdo = open(vec.at(k).c_str(), O_WRONLY | O_TRUNC | 
						O_CREAT, S_IRUSR | S_IWUSR)) < 0) // Open file directory
					{
						perror("Unable to open output file: ");
						exit(1);
					}
					if (dup2(fdo, 1) < 0) exit(1); // Set i/o to directory
				}
				else if (outredirecta) // For appending
				{
					if ((fdo = open(vec.at(k).c_str(), O_WRONLY | 
						O_CREAT | O_APPEND, S_IRUSR | S_IWUSR)) < 0)
					{
						perror("Unable to open output file: ");
						exit(1);
					}
					if (dup2(fdo, 1) < 0) exit(1);
				}
				if (inredirect) // For input
				{
					if ((fdi = open(vec.at(j).c_str(), O_RDONLY | 
						O_CREAT, S_IRUSR | S_IWUSR)) < 0)
					{
						perror("Unable to open input file: ");
						exit(1);
					}
					if (dup2(fdi, 0) < 0) exit(1);
				}
				
				if (piping)
				{
					if (pipe(fds) < 0)
					{
						perror("Pipe Error: ");
						exit(1);
					}
					if (dup2(fds[0], 0) < 0) exit(1);
					
					if (fork() == 0)
					{
						dup2(fds[1], 1);
						
						close(fds[0]);
						
				    	err = execvp(arg[0], arg); // Execute command
					}
				    else
				    {
						char* pArg[64];
	                    unsigned int b = 0;
	                    
	    				for (; i < vec.size(); ++i) //Create argv array
	    				{
	    					if (vec.at(i) == "|")
	    					{
	    						piping = 1;
	    						++i;
	    						++b;
	    						break;
	    					}
	    					else if (i != j && i != j - 1 && i != k - 1 && 
	    						i != k)
	    					{
	    						// std::cout << vec.at(i) << std::endl;
	    						pArg[b] = const_cast<char*>(vec.at(i).c_str());
	    						++b;
	    						// Add arg if not i/o redirect
	    					}
	    				}
	    				pArg[b] = NULL;
	    				
	    				close(fds[1]);
	    				
						err = execvp(pArg[0], pArg); // Execute command
						_exit(EXIT_FAILURE);
				    }
				}
				else 
				{
					err = execvp(arg[0], arg);
				}

			    if (outredirect || outredirecta) // Restore stdout
			    {
				    fflush(stdout);
				    
				    if (dup2(savestdout, 1) < 0) exit(1);
				    close(savestdout);
			    }
			    if (inredirect) // Restore stdin
			    {
			    	fflush(stdin);
			    	
				    if (dup2(savestdin, 0) < 0) exit(1);
				    close(savestdin);
			    }
			    if (piping)
			    {
			    	fflush(stdin);
			    	fflush(stdout);
			    	
				    if (dup2(0, 0) < 0) exit(1);
				    if (dup2(1, 1) < 0) exit(1);
				    
				    close(savestdin);
				    close(savestdout);
			    }

			    //Check error, output perror if execvp does not execute
			    //output error message with cmd
			    std::string c_message(arg[0]);
			    std::string e_message = "Error: " + c_message;
			    if (err == -1) perror(e_message.c_str()); 
			    
			    // Exec never returns
			    _exit(EXIT_FAILURE);   
			}
			
			if (err == -1) return err;
			return 0;
		};     
};

class Test : public Command
{
    public:
        Test() { }
        
        Test(std::vector<std::string> vec) : Command(vec) { }
        
		int run() // Executes the command
		{
            std::string e_message = "Error: [: missing `]'";
		    
		    if (vec.at(1) == "-e")  // Check flag
		    {
		        if (vec.at(0) != "test")
		            if (vec.size() < 4) perror(e_message.c_str());
		        if (fileExists(vec.at(2).c_str()))
		        {
		            std::cout << "(True)\n";
		            return 0;
		        }
		    }
		    else if (vec.at(1) == "-f")
		    {
		        if (vec.at(0) != "test")
		            if (vec.size() < 4) perror(e_message.c_str());
		        if (fileReg(vec.at(2).c_str()))
		        {
		            std::cout << "(True)\n";
		            return 0;
		        }
		    }
		    else if (vec.at(1) == "-d")
		    {
		        if (vec.at(0) != "test")
		            if (vec.size() < 4) perror(e_message.c_str());
		        if (fileDir(vec.at(2).c_str()))
		        {
		            std::cout << "(True)\n";
		            return 0;
		        }
		    }
		    else 
		    {
		        if (vec.at(0) != "test")
    		        if (vec.size() < 3) perror(e_message.c_str());
		        if (fileExists(vec.at(1).c_str()))
		        {
		            std::cout << "(True)\n";
		            return 0;
		        }
		    }
		    
		    std::cout << "(False)\n";
		    return -1;
		};
};

class Pipe : public Command
{
	public:
		Pipe() { }
		
		Pipe(std::vector<std::string> vec) : Command(vec) { }
		
		int run() // Executes the command
		{
			pid_t pid = fork(); // Fork and get child pid
			int err = 0;
			
			if (pid == -1) // If pid is -1, error and fork() has failed
			{
			    exit(-1);
			} 
			else if (pid > 0) // If pid is positive and greater than 0, wait 
			//for child process
			{
			    int status;
			    waitpid(pid, &status, 0);
			}
			else // If pid is 0, execute command in child process
			{
				unsigned int i;
				unsigned int j = 100;
				unsigned int k = 100;
				bool outredirect = 0;
				bool outredirecta = 0;
				bool inredirect = 0;
				bool piping = 0;
				
				for (i = 0; i < vec.size(); ++i) //Create argv array
				{
					if (vec.at(i) == ">>") // Check for i/o redirection
					{
						outredirecta = 1;
						k = i + 1;
					}
					else if (vec.at(i) == ">") 
					{
						outredirect = 1;
						k = i + 1;
					}
					else if (vec.at(i) == "<")
					{
						inredirect = 1;
						j = i + 1;
					}
					else if (vec.at(i) == "|")
					{
						piping = 1;
						break;
					}
					else if (i != j && i != k)
					{
						this->arg[i] = const_cast<char*>(vec.at(i).c_str());
						// Add arg if not i/o redirect
					}
				}
				this->arg[i] = NULL;

				int fdi, fdo; // File directory for i/o
				int savestdout, savestdin; // Save original stdin, stdout
				int fds[2];
				
				if (outredirect)
				{
					if ((savestdout = dup(1)) < 0) exit(1); // Save original
					if ((fdo = open(vec.at(k).c_str(), O_WRONLY | O_TRUNC | 
						O_CREAT, S_IRUSR | S_IWUSR)) < 0) // Open file directory
					{
						perror("Unable to open output file: ");
						exit(1);
					}
					if (dup2(fdo, 1) < 0) exit(1); // Set i/o to directory
					close(fdo); // Close unneeded file directory
				}
				else if (outredirecta) // For appending
				{
					if ((savestdout = dup(1)) < 0) exit(1); 
					if ((fdo = open(vec.at(k).c_str(), O_WRONLY | 
						O_CREAT | O_APPEND, S_IRUSR | S_IWUSR)) < 0)
					{
						perror("Unable to open output file: ");
						exit(1);
					}
					if (dup2(fdo, 1) < 0) exit(1);
					close(fdo);
				}
				if (inredirect) // For input
				{
					if ((savestdin = dup(0)) < 0) exit(1);
					if ((fdi = open(vec.at(j).c_str(), O_RDONLY | 
						O_CREAT, S_IRUSR | S_IWUSR)) < 0)
					{
						perror("Unable to open input file: ");
						exit(1);
					}
					if (dup2(fdi, 0) < 0) exit(1);
					close(fdi); 
				}
				if (piping)
				{
					pipe(fds);
					if ((savestdin = dup(0)) < 0) exit(1);
					if (dup2(fds[0], 0) < 0) exit(1);
					close(fds[0]); 
				}
				
			    err = execvp(arg[0], arg); // Execute command
			    
			    if (outredirect || outredirecta) // Restore stdout
			    {
				    fflush(stdout);
				    
				    if (dup2(savestdout, 1) < 0) exit(1);
				    close(savestdout);
			    }
			    if (inredirect) // Restore stdin
			    {
			    	fflush(stdin);
			    	
				    if (dup2(savestdin, 0) < 0) exit(1);
				    close(savestdin);
			    }
			    if (piping)
			    {
			    	fflush(stdin);
			    	
				    if (dup2(savestdin, 0) < 0) exit(1);
				    close(savestdin);
			    }
			    
			    //Check error, output perror if execvp does not execute
			    //output error message with cmd
			    std::string c_message(arg[0]);
			    std::string e_message = "Error: " + c_message;
			    if (err == -1) perror(e_message.c_str()); 
			    
			    // Exec never returns
			    _exit(EXIT_FAILURE);   
			}
			
			if (err == -1) return err;
			return 0;
		};    
};

/* Composite Class */
class Connector : public CommandLine 
{
	protected:
        CommandLine* com1;
        CommandLine* com2;
		
	public:
		Connector() { }
		
		Connector(CommandLine* c1, CommandLine* c2) : com1(c1), com2(c2) { }
		
		//Run command 1 and 2 for || and &&
		virtual int run() = 0;
		
        virtual void parse() { };
};

//AND
class AND : public Connector 
{
	public:
	    AND() { }
	    
	    AND(CommandLine* c1, CommandLine* c2) : Connector(c1, c2) { }
	    
	    int run() 
	    { 
	        if (com1->run() == 0)
	        {
	            if (com2->run() == 0)
	                return 0;
	        }
	        return -1;
	    };
};

//OR
class OR : public Connector 
{
	public:
	    OR() { }
	    
	    OR(CommandLine* c1, CommandLine* c2) : Connector(c1, c2) { }
	    
        int run() 
        { 
	        if (com1->run() == -1)
	        {
	            if (com2->run() == 0)
	                return 0;
	            else
	                return -1;
	        }
	        return 0;
        };
};

//PIPE
// class PIPE : public Connector
// {
// 	public:
// 	    PIPE() { }
	    
// 	    PIPE(CommandLine* c1, CommandLine* c2) : Connector(c1, c2) { }
	    
//         int run() 
//         {
// 	        if (com1->run(fds) == 0)
// 	        {
// 	            if (com2->run(fds) == 0)
// 	                return 0;
// 	        }
// 	        return -1;
//         };	
// };

//Terminating
class Terminate : public Connector 
{
    protected:
		std::string line;
		std::vector<CommandLine*> commands; //List of commands
		std::vector<CommandLine*> connected; //List of connected commands
		std::vector<int> connectors;  //List of connectors

	public:
	    Terminate() : Connector() { }
	    
	    Terminate(std::string input) : line(input) { }
	    
	    int run() { return 0; };
	    
	    CommandLine* makeCmd(std::vector<std::string> ca)
	    {
		    if (ca.at(0) == "test" || ca.at(0) == "[")
		    {
		        return new Test(ca);
		    }
		    
	        return new Command(ca); 
	    }
	    
	    // Parse through string for every command or argument
	    // Return a vector of commands arguments and connectors to be easily
	    // utilized
	    std::vector<std::string> stringParse(std::string s)
	    {
	        std::vector<std::string> coms;
	        std::string word = "";
	        
	        for (unsigned int i = 0; i < s.size(); ++i)
	        {
	            if (s.at(i) == ' ')
	            {
	                if (word != "")
	                {
	                    coms.push_back(word);
	                    word = "";
	                }
	            }
	            else 
	            {
	            	// Check for parentheses
	                if (s.at(i) == '(') 
	                {
	                    coms.push_back("(");
	                    ++i;
	                    while (s.at(i) != ')')
                        {
                            word += s.at(i);
                            ++i;
                        }
                        coms.push_back(word);
                        word = "";
                    }
                    else
                    {
                        word += s.at(i);
                    }
	            }
	        }
	        coms.push_back(word);
	        
	        return coms;
	    }
	    
	    CommandLine* cparse(std::string line)
		{
	        // Separate every word in command
    		std::vector<CommandLine*> c1; //List of commands
    		std::vector<CommandLine*> c2; //List of connected commands
    		std::vector<int> c3;  //List of connectors
    		
    		// Parse string
            std::vector<std::string> vc = stringParse(line);
            std::vector<std::string> ca;
            
            // Go through parsed string for any connections
            for (unsigned int j = 0; j < vc.size(); ++j) 
            {
                if (vc.at(j) == "(")
                {
                    ++j;
                    c1.push_back(cparse(vc.at(j)));
                }
                else if (vc.at(j) == "&&")
                {
                    if (j != vc.size() - 1) 
                        c3.push_back(2);

                    if (!ca.empty())
                    {
						c1.push_back(makeCmd(ca));
                        ca.clear();
                    }
               }
               else if (vc.at(j) == "||")
               {
                    if (j != vc.size() - 1)
                        c3.push_back(3);
                    
                    if (!ca.empty())
                    {
    				    c1.push_back(makeCmd(ca));
                        ca.clear();
                    }
               }
               else 
               {
                    ca.push_back(vc.at(j));
                    if (j == vc.size() - 1 && !ca.empty()) 
                    {
    				    c1.push_back(makeCmd(ca));
                    }
               }
            }
		    
		    // Connect commands in the line
		    for (unsigned int k = 0; k < c3.size(); ++k)
		    {
                if (k == 0) 
                {
                    if (c3.at(k) == 2)
			        {
			            c2.push_back(new AND(c1.at(0), 
			                c1.at(1)));
			        }
			        else if (c3.at(k) == 3)
			        {
			            c2.push_back(new OR(c1.at(0), 
			                c1.at(1)));
			        }
                }
                else
                {
			        if (c3.at(k) == 2)
			        {
			            c2.push_back(new AND(c1.at(k - 1), 
			                c1.at(k + 1)));
			        }
			        else if (c3.at(k) == 3)
			        {
			            c2.push_back(new OR(c1.at(k - 1), 
			                c1.at(k + 1)));
			        }
                }
		    }
		    
			// Return command from paraentheses
            if (!c2.empty()) 
            {
                return c2.at(c2.size() - 1);
            }
            else if (!c1.empty()) 
            {
                return c1.at(0);
            }
			
			return NULL;
		};
	    
	    void parse()
		{
			if (line.find("#") == 0) return; //Return if first char is #
			std::vector<std::string> vl;
			char *cstr, *p;
			
			cstr = new char [line.size() + 1];
			strcpy (cstr, line.c_str());
		
		    //Removes any comments from being run or gets everything before 
		    //comment
			p = strtok(cstr, "#"); 
			//Separates all commands
			p = strtok(cstr, ";"); 
			while (p != NULL)
			{
				// std::cout << p << std::endl;
				vl.push_back(p);
				p = strtok(NULL, ";");
			}

            //Creates a command for each one that was separated by ';'
			for (unsigned int i = 0; i < vl.size(); ++i) 
			{
		        // Separate every word in command
		        // Parse string for connectors
                std::vector<std::string> vc = stringParse(vl.at(i));
                std::vector<std::string> ca;
                
                // Create commands for connectors 
                for (unsigned int j = 0; j < vc.size(); ++j)
                {
                    // for (unsigned int m = 0; m < vc.size(); ++ m)
                    // {
                    //     std::cout << vc.at(m) << " ";
                    // }
                    // std::cout << std::endl;
                    // std::cout << vc.size() << " : " << j << std::endl;
                    // std::cout << vc.at(j) << std::endl;
                    if (vc.at(j) == "(")
                    {
                        ++j;
                        commands.push_back(cparse(vc.at(j)));
                    }
                    else if (vc.at(j) == "&&")
                    {
                        if (j != vc.size() - 1) 
                            connectors.push_back(2);

                        if (!ca.empty())
                        {
    				        commands.push_back(makeCmd(ca)); 
                            ca.clear();
                        }
                   }
                   else if (vc.at(j) == "||")
                   {
                        if (j != vc.size() - 1)
                            connectors.push_back(3);
                        
                        if (!ca.empty())
                        {
        				    commands.push_back(makeCmd(ca));
                            ca.clear();
                        }
                   }
                   else 
                   {
                        ca.push_back(vc.at(j));
                        if (j == vc.size() - 1 && !ca.empty()) 
                        {
        				    commands.push_back(makeCmd(ca));
                        }
                   }
			    }
			    
			    // Create connections
			    for (unsigned int k = 0; k < connectors.size(); ++k)
			    {
                    if (k == 0) 
                    {
                        if (connectors.at(k) == 2)
    			        {
    			            connected.push_back(new AND(commands.at(0), 
    			                commands.at(1)));
    			        }
    			        else if (connectors.at(k) == 3)
    			        {
    			            connected.push_back(new OR(commands.at(0), 
    			                commands.at(1)));
    			        }
                    }
                    else
                    {
    			        if (connectors.at(k) == 2)
    			        {
    			            connected.push_back(new AND(commands.at(k - 1), 
    			                commands.at(k + 1)));
    			        }
    			        else if (connectors.at(k) == 3)
    			        {
    			            connected.push_back(new OR(commands.at(k - 1), 
    			                commands.at(k + 1)));
    			        }
                    }
			    }

                if (!connected.empty()) 
                {
                    //Run from left to right of connected commands
                    connected.at(connected.size() - 1)->run(); 
                }
                else if (!commands.empty()) 
                {
                    commands.at(0)->run();
                }
                
                commands.clear();
                connectors.clear();
                connected.clear();
			}
		};
};

#endif