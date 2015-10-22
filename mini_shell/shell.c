#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
//#include <mcheck.h>

#include "parser.h"
#include "shell.h"

/**
 * Program that simulates a simple shell.
 * The shell covers basic commands, including builtin commands 
 * (cd and exit only), standard I/O redirection and piping (|). 
 
 */

#define MAX_DIRNAME 100
#define MAX_COMMAND 1024
#define MAX_TOKEN 128

/* Functions to implement, see below after main */
int execute_cd(char** words);
int execute_nonbuiltin(simple_command *s);
int execute_simple_command(simple_command *cmd);
int execute_complex_command(command *cmd);


int main(int argc, char** argv) {
	
	char cwd[MAX_DIRNAME];           /* Current working directory */
	char command_line[MAX_COMMAND];  /* The command */
	char *tokens[MAX_TOKEN];         /* Command tokens (program name, 
					  * parameters, pipe, etc.) */

	while (1) {

		/* Display prompt */		
		getcwd(cwd, MAX_DIRNAME-1);
		printf("%s> ", cwd);
		
		/* Read the command line */
		fgets(command_line, MAX_COMMAND, stdin);
		/* Strip the new line character */
		if (command_line[strlen(command_line) - 1] == '\n') {
			command_line[strlen(command_line) - 1] = '\0';
		}
		
		/* Parse the command into tokens */
		parse_line(command_line, tokens);

		/* Check for empty command */
		if (!(*tokens)) {
			continue;
		}
		
		/* Construct chain of commands, if multiple commands */
		command *cmd = construct_command(tokens);
		//print_command(cmd, 0);
    
		int exitcode = 0;
		if (cmd->scmd) {
			exitcode = execute_simple_command(cmd->scmd);
			if (exitcode == -1) {
				break;
			}
		}
		else {
			exitcode = execute_complex_command(cmd);
			if (exitcode == -1) {
				break;
			}
		}
		release_command(cmd);
	}
    
	return 0;
}


/**
 * Changes directory to a path specified in the words argument;
 * For example: words[0] = "cd"
 *              words[1] = "csc209/assignment3/"
 * Your command should handle both relative paths to the current 
 * working directory, and absolute paths relative to root,
 * e.g., relative path:  cd csc209/assignment3/
 *       absolute path:  cd /u/bogdan/csc209/assignment3/
 */
int execute_cd(char** words) {
	
	/** 
	 * TODO: 
	 * The first word contains the "cd" string, the second one contains 
	 * the path.
	 * Check possible errors:
	 * - The words pointer could be NULL, the first string or the second 
	 *   string could be NULL, or the first string is not a cd command
	 * - If so, return an EXIT_FAILURE status to indicate something is 
	 *   wrong.
	 */
	 if (words==NULL) // check if the token is NULL
	 {
	 	return EXIT_FAILURE;
	 }
	 if(words[0]==NULL || strcmp(words[0], "cd") != 0) // check if the first element is cd
	 {
	 	return EXIT_FAILURE;
	 }
	 if(words[1]==NULL) // check if the argument is NULL
	 {
	 	return EXIT_FAILURE;
	 }
	 int ret;

	 ret = chdir(words[1]); // System call to change directory
	 if (ret == -1) // checking chdir fail condition
     {
         perror("chdir");
         return EXIT_FAILURE;
     }
	 return ret;



	/**
	 * TODO: 
	 * The safest way would be to first determine if the path is relative 
	 * or absolute (see is_relative function provided).
	 * - If it's not relative, then simply change the directory to the path 
	 * specified in the second word in the array.
	 * - If it's relative, then make sure to get the current working 
	 * directory, append the path in the second word to the current working
	 * directory and change the directory to this path.
	 * Hints: see chdir and getcwd man pages.
	 * Return the success/error code obtained when changing the directory.
	 */

	 
}


/**
 * Executes a program, based on the tokens provided as 
 * an argument.
 * For example, "ls -l" is represented in the tokens array by 
 * 2 strings "ls" and "-l", followed by a NULL token.
 * The command "ls -l | wc -l" will contain 5 tokens, 
 * followed by a NULL token. 
 */
int execute_command(char **tokens) {
	
	/**
	 * TODO: execute a program, based on the tokens provided.
	 * The first token is the command name, the rest are the arguments 
	 * for the command. 
	 * Hint: see execlp/execvp man pages.
	 * 
	 * - In case of error, make sure to use "perror" to indicate the name
	 *   of the command that failed.
	 *   You do NOT have to print an identical error message to what would 
	 *   happen in bash.
	 *   If you use perror, an output like: 
	 *      my_silly_command: No such file of directory 
	 *   would suffice.
	 * Function returns only in case of a failure (EXIT_FAILURE).
	 */
    
    execvp(tokens[0], tokens); // running the command with token[0]= command and tokens= arguments
    perror(tokens[0]); // this line is run when execvp fails to check fail condition
    //exit(1)
    
    
	 

    return EXIT_FAILURE;

}


/**
 * Executes a non-builtin command.
 */
int execute_nonbuiltin(simple_command *s) {
	/**
	 * TODO: Check if the in, out, and err fields are set (not NULL),
	 * and, IN EACH CASE:
	 * - Open a new file descriptor (make sure you have the correct flags,
	 *   and permissions);
	 * - redirect stdin/stdout/stderr to the corresponding file.
	 *   (hint: see dup2 man pages).
	 * - close the newly opened file descriptor in the parent as well. 
	 *   (Avoid leaving the file descriptor open across an exec!) 
	 * - finally, execute the command using the tokens (see execute_command
	 *   function above).
	 * This function returns only if the execution of the program fails.
	 */
    int fd;
    if (s->out != NULL && s->err != NULL) // &>
    {
        if ((fd=open(s->out, O_CREAT))<0) {
            return EXIT_FAILURE;
        }
        if ((dup2(fd, 1))== -1) // change the stdout to the file
        {
            perror("dup2");
            return EXIT_FAILURE;
        }
        if ((dup2(fd, 2))== -1) //change sdterr to the file
        {
            perror("Dup2");
            return EXIT_FAILURE;
        }
        if ((close(fd))==-1) // closing the new file's file discriptor
        {
            perror("close");
            return EXIT_FAILURE;
        }
        execute_command(s->tokens);
        
        
    }
    else if (s->out != NULL && s->err == NULL) // >
    {
       // int status;
        
        if ((fd=open(s->out,  O_WRONLY | O_CREAT,0666))<0) // open file with appropriate perrmissions
        {
            return EXIT_FAILURE;
        }
        
        if ((dup2(fd, 1))== -1) // change std out to the new file
        {
            perror("Dup2");
            return EXIT_FAILURE;
        }
        if ((close(fd))==-1) // closing the new file's file discriptor
        {
            perror("close");
            return EXIT_FAILURE;
        }
        execute_command(s->tokens);
                
    }
    else if (s->in != NULL) // <
    {
        if ((fd=open(s->in, O_RDONLY))<0) {  // open file with appropriate perrmissions
            return EXIT_FAILURE;
        }
        
        if ((dup2(fd, 0))== -1) // change the stdin to the file.
        {
            perror("Dup2");
            return EXIT_FAILURE;
        }
        execute_command(s->tokens);
    }
    
    else if (s->err != NULL && s->out == NULL) // 2>
    {
        if((fd=open(s->err, O_CREAT | O_WRONLY), 0666)<0)   // open file with appropriate perrmissions
        {
            return EXIT_FAILURE;
        }
        if ((dup2(fd, 2))== -1)   // change the stderr to the file.
        {
            perror("Dup2");
            return EXIT_FAILURE;
        }
        execute_command(s->tokens);
    }
    else if(s->in==NULL && s->err==NULL && s->out==NULL)  // when no redirction
    {
        execute_command(s->tokens);
    }
    else
    {
        return EXIT_FAILURE;
            }
    
    
    return 0;
	

}


/**
 * Executes a simple command (no pipes).
 */
int execute_simple_command(simple_command *cmd) {

	/**
	 * TODO: 
	 * Check if the command is builtin.
	 * 1. If it is, then handle BUILTIN_CD (see execute_cd function provided) 
	 *    and BUILTIN_EXIT (simply exit with an appropriate exit status).
	 * 2. If it isn't, then you must execute the non-builtin command. 
	 * - Fork a process to execute the nonbuiltin command 
	 *   (see execute_nonbuiltin function above).
	 * - The parent should wait for the child.
	 *   (see wait man pages).
	 */
    if (cmd->builtin == BUILTIN_CD) // check if cd or not
    {
        execute_cd(cmd->tokens);
    }
    else if(cmd->builtin == BUILTIN_EXIT) // check if exit or not
    {
        exit(1);
    }
    else if (cmd->builtin == 0) // check if built in or not
    {
        int pid;
        if ((pid=fork())==-1)
        {
            perror("Fork");
            return EXIT_FAILURE;
        }
       else  if (pid == 0)
        {
            execute_nonbuiltin(cmd); // running non built in commands
        }
        else
        {
            int status;
            if(wait(&status) == -1)  { // waiting for the child
                perror("wait");
                return EXIT_FAILURE;
                }
        }
            
    }
    return 0;
    
	
}


/**
 * Executes a complex command.  A complex command is two commands chained 
 * together with a pipe operator.
 */
int execute_complex_command(command *c) {
	
	/**
	 * TODO:
	 * Check if this is a simple command, using the scmd field.
	 * Remember that this will be called recursively, so when you encounter
	 * a simple command you should act accordingly.
	 * Execute nonbuiltin commands only. If it's exit or cd, you should not 
	 * execute these in a piped context, so simply ignore builtin commands. 
	 */
    if(c->scmd != NULL)
    {
        int *check = &((c->scmd)->builtin);
        if (*check == BUILTIN_CD || *check == BUILTIN_EXIT){ // check if builtin or not if built in then it fails.
            return 0;
        }
        execute_nonbuiltin(c->scmd); // checking if simple command or not
    }
	


	/** 
	 * Optional: if you wish to handle more than just the 
	 * pipe operator '|' (the '&&', ';' etc. operators), then 
	 * you can add more options here. 
	 */

	if (!strcmp(c->oper, "|")) {
		
		/**
		 * TODO: Create a pipe "pfd" that generates a pair of file 
		 * descriptors, to be used for communication between the 
		 * parent and the child. Make sure to check any errors in 
		 * creating the pipe.
		 */
        int pfd[2];
        int result;
        
        if((result = pipe(pfd)) == -1) //making pipe
        {
            perror("pipe");
            exit(1);
        }
        

			
		/**
		 * TODO: Fork a new process.
		 * In the child:
		 *  - close one end of the pipe pfd and close the stdout 
		 * file descriptor.
		 *  - connect the stdout to the other end of the pipe (the 
		 * one you didn't close).
		 *  - execute complex command cmd1 recursively. 
		 * In the parent: 
		 *  - fork a new process to execute cmd2 recursively.
		 *  - In child 2:
		 *     - close one end of the pipe pfd (the other one than 
		 *       the first child), and close the standard input file 
		 *       descriptor.
		 *     - connect the stdin to the other end of the pipe (the 
		 *       one you didn't close).
		 *     - execute complex command cmd2 recursively. 
		 *  - In the parent:
		 *     - close both ends of the pipe. 
		 *     - wait for both children to finish.
		 */
        int r;
        int status;
        if((r= fork())==-1)
        {
            perror("Fork");
            exit(1);
        }
        else if (r==0) //child 1
        {
            if ((close(pfd[0])== -1)) //close the end of the pipe not being used
                {
                perror("close");
                return EXIT_FAILURE;
                }
            if((dup2(pfd[1], fileno(stdout))) == -1) // redirect stdout to one end of the pipe
            {
                perror("Dup2");
                return EXIT_FAILURE;
            }
            if ((close(pfd[1]))== -1) // close the end of the pipe that we used.
            {
                perror("close");
                return EXIT_FAILURE;
            }
            execute_complex_command(c->cmd1); // recursive call
            exit(0); // make sure it exits
        }
        else
        {
            int k;
            if((k=fork())==-1)
            {
                perror("Fork");
            }
            else if (k==0) // child 2
            {
                if((close(pfd[1])) == -1) // close the end of the pipe not being used
                {
                    perror("close");
                    return EXIT_FAILURE;
                }
                if((dup2(pfd[0],fileno(stdin)))== -1) //redirect stdin to one end of the pipe that has the output from other command
                {
                    perror("Dup2");
                    return EXIT_FAILURE;
                }
                if ((close(pfd[0]))== -1) // cole the end of the pipe we used
                {
                    perror("close");
                    return EXIT_FAILURE;
                }
                execute_complex_command(c->cmd2); //recursive call
                exit(1);// make sure it exits
            }
            else
            {
                
                if((close(pfd[0])) == -1) // closing all ends of the pipe
                {
                    perror("close");
                    return EXIT_FAILURE;
                }
                if((close(pfd[1])) == -1)
                {
                    perror("close");
                    return EXIT_FAILURE;
                }
                if ((wait(&status))== -1) //wait for child 1
                {
                    perror("wait");
                    return EXIT_FAILURE;
                }
                if ((wait(&status))== -1) // wait for child 2
                {
                    perror("wait");
                    return EXIT_FAILURE;
                }
            }
        }
        
		
	}
	return 0;
}
