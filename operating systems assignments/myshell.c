/*
 * Assignment 3, Problem 1
 * Extension of the shell
 *
 * Submitted by: 
 * Utkarsh Chanchlani 11CS10049
 * Maru Sudarshan Avish 11CS10028
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<dirent.h>
#include<grp.h>
#include <fcntl.h>

size_t allocSize = sizeof(char) * 256; // max size of the buffer and dir_addr

/*
 * function to parse the one line terminal input to arguements
 */
void  parse(char *line, char **argv) {
    while (*line != '\0') {       /* if not the end of line ....... */ 
	while (*line == ' ' || *line == '\t' || *line == '\n')
	    *line++ = '\0';     /* replace white spaces with 0    */
	if(*line!='\0')
	    *argv++ = line;          /* save the argument position     */
	while (*line != '\0' && *line != ' ' && 
	       *line != '\t' && *line != '\n') 
	    line++;             /* skip the argument until ...    */
    }
    *argv = NULL;                 /* mark the end of argument list  */
}


/*
 * in ls -l mode, this function prints the mode, nlink, gid, size and time of modification of the file whose stat is represented by info
 */
void print_stats(struct stat info) {
    char buff[20];
    struct group *grp;
    struct tm * timeinfo;
    grp = getgrgid(info.st_gid);
    timeinfo = localtime (&(info.st_mtime)); 
    strftime(buff, 20, "%b %d %H:%M", timeinfo); 
    printf("%o\t%d\t%s\t%d\t%s\t",info.st_mode, info.st_nlink, grp->gr_name, info.st_size, buff);
}

/*
 * this function checks for ampersand in the array of strings, if found it truncates the remaining arguements and returns 1 else 0
 */
int isampersand(char** argv) {
    int i;
    for(i=0;;i++) {
	if(argv[i]==NULL)
	    return 0;
	else if(strcmp(argv[i], "&") == 0) {
	    argv[i] = '\0';
	    return 1;
	}
    }
}

/*
 * child signal handler
 */
void childterminate(int sig) {
    if(sig == SIGINT) {
	printf("\n");
	exit(-1);
    }
}

/*
 * shell signal handler
 */
void parentsignal(int sig) {
    if(sig == SIGINT) {
	printf("\nmyshell> ");
	fflush(stdout);
    }
}


/*
 * this func recieves a command free from pipes and redirection and it executes it
 */
int exec_shell(char* buffer) {
    char **arg; // array of string, to store the all the command line arguements
    char *ptok, *dir_addr; // ptok is the tokenizer, buffer is the raw command line, dir_addr is the path/address of the directory
    int checker, i = 0, pid, status; // checker is used to check success/failure of system calls. i is the counter

    /* Dynamic memory allocation
     * provision to execute any line of max 256 char, and max 64 arguements
     */
    arg = (char**)malloc(256*sizeof(char*));
    dir_addr = (char*)malloc(allocSize);


    parse(buffer, arg);
    /*
     * cases starts from here
     */

    /* case for cd command */
    if(strcmp(arg[0],"cd")==0) {
	if(arg[1]==NULL) { // directory name not present
	    printf("Error! Arguement Missing\n");
	    goto mem_free;
	}
	    
	checker = chdir(arg[1]);
	    
	if(checker == -1) // system call error, failed to change directory
	    perror("Cd Error");

	if(getcwd(dir_addr,allocSize)==NULL) { // system call error, failed to fetch current directory address
	    perror("Directory Name Error");
	    goto mem_free;
	}

	printf("%s\n",dir_addr); // print the directory address
    }

    /* case for pwd command */
    else if(strcmp(arg[0],"pwd")==0) {
	if(getcwd(dir_addr,allocSize)==NULL) { // system call error, failed to fetch current directory address
	    perror("Directory Name Error");
	    goto mem_free;
	}

	printf("%s\n",dir_addr); // print the directory address
    }

    /* case for mkdir */
    else if(strcmp(arg[0],"mkdir")==0) {
	if(arg[1]==NULL) { // directory name not present
	    printf("Error! Arguement Missing\n");
	    goto mem_free;
	}
	checker = mkdir(arg[1], 0777);

	if(checker == -1) // system call error, failed to create directory
	    perror("Mkdir Error");

	if(getcwd(dir_addr,allocSize)==NULL) { // system call error, failed to fetch current directory address
	    perror("Directory Name Error");
	    goto mem_free;
	}

	printf("%s\n",dir_addr); // print the directory address
    }

    /* case for rmdir */
    else if(strcmp(arg[0],"rmdir")==0) {
	if(arg[1]==NULL) { // directory name not present
	    printf("Error! Arguement Missing\n");
	    goto mem_free;
	}

	checker = rmdir(arg[1]);

	if(checker == -1) // system call error, failed to delete directory
	    perror("Rmdir Error");

	if(getcwd(dir_addr,allocSize)==NULL) { // system call error, failed to fetch current directory address
	    perror("Directory Name Error");
	    goto mem_free;
	}

	printf("%s\n",dir_addr); // print the directory address
    }

    /* case for ls */
    else if(strcmp(arg[0],"ls")==0) {
	DIR *curr_dir; // the current directory struct
	struct dirent *dir_content; // stores the content of the current directory
	struct stat curr_stat; // stores the status of the current content of the directory
	curr_dir = opendir(".");
	if(curr_dir == NULL) { // System call error, failed to open the directory
	    perror("ls error()");
	    goto mem_free;
	}

	while ((dir_content = readdir(curr_dir)) != NULL) { // while we have more content to display
	    if(arg[1]!=NULL) { // there is some mode specified
		if(strcmp(arg[1], "-l") == 0) { // -l mode specified
		    checker = stat(dir_content->d_name, &curr_stat);
		    if(checker==0) { // print the stat of current content
			print_stats(curr_stat);
		    }
		    else { // failed to fetch the current content
			perror("Pstat Error");
		    }
		    printf("%s\n", dir_content->d_name);
		}
		else { // other than -l typed
		    printf("Unrecognized mode!\n");
		    break;
		}
	    }
	    else
		printf("%s\t\t", dir_content->d_name);
	}

	if(arg[1]==NULL)
	    printf("\n"); // in simple ls (without any mode), we display content in tabs, so we need a newlone at the end
	closedir(curr_dir);
    }

    /*
     * add new commands here
     */

    /* case for exit */
    else if(strcmp(arg[0],"exit")==0) {
	/* freeing the args memory */
	//for(;i>=0;i--)
	//free(arg[i]);
	/* freeing all the memories */
	free(dir_addr);
	free(arg);
	return 0;
    }

    /* all other commands are considered as executables */
    else {
	/* check if any ampersand is present in the arguements */
	checker = isampersand(arg);
	if(pid = fork()==0) { // child process
	    signal(SIGINT, childterminate);           /* Register SIGUSR1 handler */
	    status = execvp(arg[0],arg);
	    /* if the command is executed then this part of the child code is not executed, else this part is executed and child exits with a failure status (-1) */
	    perror("File Execution Error");
	    exit(-1);
	}
	else { // parent process
	    /* parent wait for the child to return if no ampersand is present */
	    if(checker==0)
		if(wait(&status)!=pid)
		    wait(NULL);
	}
    }

mem_free:
    /* freeing the args memory, initially no memory was allocated thus no memory will be freed, i = -1 */
    //for(;i>=0;i--)
    //free(arg[i]);
    /* freeing all the memories */
    free(dir_addr);
    free(arg);

    return 1;
}

/*
 * the function checks if there is any redirection <> commands in the "line"
 * redir[0] is 1 if there is input redirection and redir[1] is 1 for output redirection
 * name of input and output files are stored in ifile and ofile if applicable
 * after the execution of the function, the < and > are removed from the command completely, and ' 'are inserted instead of them which are obviously ignored at a later stage
 */
void redirection_check(char* line, int redir[], char ifile[], char ofile[]) {
    int i = 0, j;
    redir[0] = 0; redir[1] = 0;
    while(line[i] != '\0') {
	if(line[i] == '<') {
	    redir[0] = 1;
	    line[i] = ' ';
	    i++;
	    while(line[i] == ' ' || line[i] == '\t')
		i++;
	    j = 0;
	    while(line[i] != ' ' && line[i] != '\t' && line[i]!= '\n' && line[i] != '>' && line[i] != '<' && line[i] != '\0') {
		ifile[j++] = line[i];
		line[i++] = ' ';
	    }
	    ifile[j] = '\0';
	}
	if(line[i] == '>') {
	    redir[1] = 1;
	    line[i] = ' ';
	    i++;
	    while(line[i] == ' ' || line[i] == '\t')
		i++;
	    j = 0;
	    while(line[i] != ' ' && line[i] != '\t' && line[i]!= '\n' && line[i] != '>' && line[i] != '<' && line[i] != '\0') {
		ofile[j++] = line[i];
		line[i++] = ' ';
	    }
	    ofile[j] = '\0';
	}
	i++;
    }

    return;
}

/* the main function */
int main() {
    char *buffer; // takes the input
    char redir_ifile[50], redir_ofile[50]; // redirection ifile and ofile name, if applicable
    int check = 1, redirect_status[2], redir_std_fd[2], redir_fd[2], pipe_std_fd[2], pipe_fd[4], i = 2;
    /* this comment follows the description of all the variables used
     * check: special check for exit command
     * redirec_status: to check the status of redirection <> commands, if any of them is present
     * redir_std_fd: before redirection, we store the original standard inputs and outputs of the shell, which will be restored after the command is executed
     * redir_fd: file descriptors where the i/o is to be redirected
     * pipe_std_fd: before piping the i/o of a process, it's original standard i/o is stored here
     * pipe_fd: file descriptors of the two pipes used
     * i selects 1 of the 2 pipes 1 after other
     */
    char *pipe_tok1, *pipe_tok2; // two sides of the pipe

    signal(SIGINT, parentsignal);           /* Register SIGUSR1 handler */
    buffer = (char*)malloc(allocSize); // allocate size for the buffer


    while(check) { // infinite loop untill exit command is seen
	/* take input from the shell */
	printf("myshell> "); gets(buffer);// fgets(buffer, sizeof(buffer), stdin);
	if(feof(stdin))
	    break;
	if(buffer[0]=='\0')
	    continue;

	/*
	 * redirection of stdin and stdout
	 */
	redirection_check(buffer, redirect_status, redir_ifile, redir_ofile);
	if(redirect_status[0]) { //redir stdin
	    redir_std_fd[0] = dup(0); // save the standard input
	    close(0); // close it
	    redir_fd[0] = open(redir_ifile, O_RDONLY); // open the file descriptor of the file from which we take the input in lowest fd i.e. 0
	}
	if(redirect_status[1]) { //redir stdout
	    redir_std_fd[1] = dup(1); // save the standard output
	    close(1); // close it
	    redir_fd[1] = open(redir_ofile, O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); // open the file descriptor of the file where we direct the output in lowest fd i.e. 1
	}


	/*
	 * pipe functions
	 */

	pipe(pipe_fd+2); // pipe_fd is the pipe's file descriptor, i've used two pipes and firstly the 2nd pipe is opened
	close(pipe_fd[3]); // and it's write end is closed
	i = 2;

	/* pipe_tok1 is the left side of the pipe which writes the output to the pipe and pipe_tok2 is the right side which reads from the pipe */
	pipe_tok1 = strtok(buffer, "|");

	if(pipe_tok1 != NULL) { // if there is atleast 1 end of the pipe
	    /* copy the original stdandard in outs excluding the pipes in std fds*/
	    pipe_std_fd[0] = dup(0);
	    pipe_std_fd[1] = dup(1);

	    pipe_tok2 = strtok(NULL, "|"); // check if there is any right process
	    while(pipe_tok2 != NULL) { // while there is always a right side of the pipe
		i = (i + 2) % 4; // switch pipes
		pipe(pipe_fd+i); // create the pipe
		close(1); // close output of left process
		dup(pipe_fd[1 + i]); // redirect it to the pipe
		
		check = exec_shell(pipe_tok1); // execute the command

		close(pipe_fd[1+i]); close(pipe_fd[(2+i)%4]); // close the write end of the current pipe and read end of other pipe
		close(0); // close input of right process
		dup(pipe_fd[i]); // input from the pipe
		pipe_tok1 = pipe_tok2; // now the left process is over, right takes over it
		pipe_tok2 = strtok(NULL, "|");
	    }

	    dup2(pipe_std_fd[1],1); // restore the original standard output
	    close(pipe_std_fd[1]); // close that file descriptor from the temp value
	    check = exec_shell(pipe_tok1); // execute the final process
	    
	    dup2(pipe_std_fd[0],0); // restore the original standard input
	    close(pipe_std_fd[0]); // close that file descriptor from the temp value

	    close(pipe_fd[i]); // finally close the only open end of the pipe, it's work is over
	}	

	/*
	 * if the stdin and stdout were redirected, then restore them
	 */
	if(redirect_status[0]) { //restore the original stdin
	    close(redir_fd[0]);
	    dup2(redir_std_fd[0], 0);
	    close(redir_std_fd[0]);
	}
	if(redirect_status[1]) { //restore the original stdout
	    close(redir_fd[1]);
	    dup2(redir_std_fd[1], 1);
	    close(redir_std_fd[1]);
	}

    }

    free(buffer); // free the alocated memories

    exit(0); // bye bye :)
}
