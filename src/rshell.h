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
				
				for (i = 0; i < vec.size(); ++i) //Create argv array
				{
					this->arg[i] = const_cast<char*>(vec.at(i).c_str());
				}
				this->arg[i] = NULL;

			    err = execvp(arg[0], arg); // Execute command
			    
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
	            {
	                return 0;
	            }
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
	            {
	                return 0;
	            }
	            else
	            {
	                return -1;
	            }
	        }
	        return 0;
        };
};

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
	    
	    int run() 
	    {
            if (!connected.empty()) 
            {
                connected.at(connected.size() - 1)->run(); //Run from left to right
            }
            else if (!commands.empty()) 
            {
                commands.at(0)->run();
            }
            
	        return 0;
	    };
	    
	    // Parse through string for every command or argument
	    // Return a vector of commands arguments and connectors to be easily
	    // utilized
	    std::vector<std::string> stringParse(std::string s)
	    {
	        std::vector<std::string> coms;
	        std::string word = "";
	        
	        for (unsigned int i = 0; i < s.size(); ++i)
	        {
	            if (s.at(i) == ';')
                        coms.push_back(";");
                        
	            if (s.at(i) == ' ' || s.at(i) == ';')
	            {
	                if (word != "")
	                {
	                    coms.push_back(word);
	                    word = "";
	                }
	            }
	            else 
	            {
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
	    
	    CommandLine* cparse(std::string line, bool ex)
		{
	        // Separate every word in command
    		std::vector<CommandLine*> c1; //List of commands
    		std::vector<CommandLine*> c2; //List of connected commands
    		std::vector<int> c3;  //List of connectors
    		
            std::vector<std::string> vc = stringParse(line);
            std::vector<std::string> ca;
            
            for (unsigned int j = 0; j < vc.size(); ++j)
            {
                if (vc.at(j) == "(")
                {
                    ++j;
                    c1.push_back(cparse(vc.at(j), 0));
                }
                else if (vc.at(j) == "&&")
                {
                    if (j != vc.size() - 1) 
                        c3.push_back(2);

                    if (!ca.empty())
                    {
    				    if (ca.at(0) == "test" || ca.at(0) == "[")
    				    {
    				        c1.push_back(new Test(ca));
    				    }
    				    else 
    				    {
    				        c1.push_back(new Command(ca)); 
    				    }
                        ca.clear();
                    }
               }
               else if (vc.at(j) == "||")
               {
                    if (j != vc.size() - 1)
                        c3.push_back(3);
                    
                    if (!ca.empty())
                    {
    				    if (ca.at(0) == "test" || ca.at(0) == "[")
    				    {
    				        c1.push_back(new Test(ca));
    				    }
    				    else 
    				    {
    				        c1.push_back(new Command(ca)); 
    				    }
                        ca.clear();
                    }
               }
               else 
               {
                    ca.push_back(vc.at(j));
                    if (j == vc.size() - 1 && !ca.empty()) 
                    {
    				    if (ca.at(0) == "test" || ca.at(0) == "[")
    				        c1.push_back(new Test(ca));
    				    else 
    				        c1.push_back(new Command(ca)); 
                    }
               }
            }
		    
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

            if (!c2.empty()) 
            {
                return c2.at(c2.size() - 1); //Run from left to right
            }
            else if (!c1.empty()) 
            {
                return c1.at(0);
            }
			
			return NULL;
		};
	    
	    CommandLine* parse(std::string line, bool ex)
		{
			if (line.find("#") == 0) return NULL;
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
                std::vector<std::string> vc = stringParse(vl.at(i));
                std::vector<std::string> ca;
                
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
                        commands.push_back(cparse(vc.at(j), 0));
                    }
                    else if (vc.at(j) == "&&")
                    {
                        if (j != vc.size() - 1) 
                            connectors.push_back(2);

                        if (!ca.empty())
                        {
        				    if (ca.at(0) == "test" || ca.at(0) == "[")
        				        commands.push_back(new Test(ca));
        				    else 
        				        commands.push_back(new Command(ca)); 
                            ca.clear();
                        }
                   }
                   else if (vc.at(j) == "||")
                   {
                        if (j != vc.size() - 1)
                            connectors.push_back(3);
                        
                        if (!ca.empty())
                        {
        				    if (ca.at(0) == "test" || ca.at(0) == "[")
        				        commands.push_back(new Test(ca));
        				    else 
        				        commands.push_back(new Command(ca)); 
                            ca.clear();
                        }
                   }
                   else 
                   {
                        ca.push_back(vc.at(j));
                        if (j == vc.size() - 1 && !ca.empty()) 
                        {
        				    if (ca.at(0) == "test" || ca.at(0) == "[")
        				        commands.push_back(new Test(ca));
        				    else 
        				        commands.push_back(new Command(ca)); 
                        }
                   }
			    }
			    
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
			
			return NULL;
		};
};

#endif