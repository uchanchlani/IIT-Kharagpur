/*
 * the server
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h> 
#include <sys/stat.h>
#include <fcntl.h>

#define debug
#define CON_PORT 5000
#define MAX_CL 10

int listenfd = 0, newlistenfd = 0, n = 0, m = 0;
int pid[MAX_CL];
int parpid, par_wait;
int connfd[MAX_CL];
int assign_child;
char infile[MAX_CL][50];
char myfile[50];

void get_script(char *buffer, char *filename, int check) {
  char temp[100];
  int n;
  static int pg_no = 1;
  static int z_level = 0;
  static int width = 600;
  static int height = 600;
  static int depth = 0;
  strcpy(buffer,"");

  // if it's a new client then open the file first
  if(check==0)
    sprintf(buffer,"xdg-open %s && sleep 2 && ", filename);

  sprintf(temp, "xdotool search --onlyvisible --name '%s' windowactivate --sync && ", filename);
  strcat(buffer, temp);

  // for the new process take it to the current state
  if(check==0) {
    sprintf(temp, "xdotool key ctrl+l && xdotool type %d && xdotool key Return && ", pg_no);
    strcat(buffer, temp);
    sprintf(temp, "xdotool getactivewindow windowsize %d %d && ", width, height);
    strcat(buffer, temp);
    if(z_level>0)
      for(n=0;n<z_level;n++) {
        sprintf(temp, "xdotool key ctrl+21 && ");
        strcat(buffer, temp);
      }
    else if(z_level>0)
      for(n=0;n>z_level;n--) {
        sprintf(temp, "xdotool key ctrl+20 && ");
        strcat(buffer, temp);
      }
    if(depth>0)
      for(n=0;n<depth;n++) {
        sprintf(temp, "xdotool key 116 && ");
        strcat(buffer, temp);
      }
    strcat(buffer, "\n");
    return;
  }
  printf("Choose:\n1. Page navigation\n2. Zoom in\n3. Zoom out\n4. Resize window\n5. Scroll Window down\n6. Scroll Window up.\n<anything else>. Not Valid! It will exit the corresponding session\n");
  printf("Your choice for %s session: ",filename);
  scanf("%d",&n);
  switch(n) {
  case 1:
    printf("Page No.: "); scanf("%d", &pg_no);
    depth = 0;
    sprintf(temp, "xdotool key ctrl+l && xdotool type %d && xdotool key Return && ", pg_no);
    strcat(buffer, temp);
    break;
  case 2:
    z_level++;
    sprintf(temp, "xdotool key ctrl+21 && ");
    strcat(buffer, temp);
    break;
  case 3:
    z_level--;
    sprintf(temp, "xdotool key ctrl+20 && ");
    strcat(buffer, temp);
    break;
  case 4:
    printf("Width pixels: "); scanf("%d",&width);
    printf("Height pixels: "); scanf("%d",&height);
    sprintf(temp, "xdotool getactivewindow windowsize %d %d && ", width, height);
    strcat(buffer, temp);
    z_level = 0;
    break;
  case 5:
    depth++;
    sprintf(temp, "xdotool key 116 && ");
    strcat(buffer, temp);
    break;
  case 6:
    if(depth<=0) {
      printf("Upward Scroll Error!\n");
      break;
    }
    depth--;
    sprintf(temp, "xdotool key 111 && ");
    strcat(buffer, temp);
    break;
  default:
    printf("Not a right choice! Exitting...\n");
    sprintf(buffer,"exit");
    break;
  }
  strcat(buffer, "\n");
  return;
}

/*
 * function to parse the one line terminal input to arguements
 */
void parse(char* line) {
  while (*line != '\0' && *(line+1) != '\0') {
    if (*line == '&' || *(line+1) == '&') {
      *line++ = ' '; 
      *line++ = '\n'; 
    } 
    else
      line++;
  }
}


void exec_script(char* buffer) {
  char newbuff[1024];
  FILE* fd;
  char* ptok;
  strcpy(newbuff,buffer);
  parse(newbuff);
  fd = fopen("server.sh", "w+");
  fwrite(newbuff, sizeof(char), strlen(newbuff),fd);
  fclose(fd);

  ptok = strtok(newbuff, "\n \t");
  if(strcmp(ptok,"exit")==0)
    return;
	
  if(fork()==0) { // child process
    execlp("./server.sh","./server.sh", NULL);
    perror("File Execution Error: ");
    exit(-1);
  }
  else // parent process
    wait(NULL);
}

void parent_interrupt(int sig) {
  if(sig==SIGUSR1)
    par_wait=0;
  return;
}

void child_interrupt(int sig) {
  char sendBuff[1024];
  int j;

  if(sig==SIGUSR1) { // interrupt to add new client
    connfd[n++] = accept(listenfd, (struct sockaddr*)NULL, NULL); 
    get_script(sendBuff, myfile, 0);
    write(connfd[n-1], sendBuff, strlen(sendBuff));
    sleep(5);
  } else if(sig==SIGUSR2) { // interrupt to execute a command
    get_script(sendBuff, myfile, 1);
    exec_script(sendBuff);
    for(j=0;j<n;j++)
      write(connfd[j], sendBuff, strlen(sendBuff));
    if(strcmp(sendBuff, "exit\n")==0) {
      for(j=0;j<n;j++)
        close(connfd[j]);
      printf("Session %s is being closed! %d client connections closed...\n", myfile, n);
      kill(parpid, SIGUSR1);
      exit(0);
    }
    kill(parpid, SIGUSR1);
  }
  return;
}

int main(int argc, char *argv[]) {
  int i, j;
  struct sockaddr_in serv_addr; 
  char* sendBuff = (char*)malloc(1024*sizeof(char));;
  char sessionfile[50];
  void* bytestream = (void*)malloc(1024);
  FILE* fd;
  parpid = getpid();
  if(argc != 2) {
    perror("Missing filename: ");
    exit(1);
  } 
  else
    strcpy(sessionfile, argv[1]);

  fd = fopen(sessionfile,"r");
  while((!feof(fd)) && (m<MAX_CL))
    fscanf(fd, "%s", infile[m++]);
  if(strcmp(infile[m-1],"")==0)
    m--;
  fclose(fd);

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&serv_addr, '0', sizeof(serv_addr));
  memset(sendBuff, '0', sizeof(sendBuff)); 

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(CON_PORT); 

  bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

  listen(listenfd, 10);

  for(i = 0; i < m; i++) {
    pid[i] = fork();
    strcpy(myfile,infile[i]);
    if(pid[i]==0) { // child process
      signal(SIGUSR1, child_interrupt);
      signal(SIGUSR2, child_interrupt);
      get_script(sendBuff, myfile, 0);
      exec_script(sendBuff);
      while(1);
      /*				get_script(sendBuff, myfile, 1);1

                        exec_script(sendBuff);
                        for(j=0;j<n;j++)
                        write(connfd[j], sendBuff, strlen(sendBuff));
                        if(strcmp(sendBuff, "exit\n")==0)
                        break;
                        sleep(5);
                        }
                        for(j=0;j<n;j++)
                        close(connfd[j]);
                        printf("Session %s is being closed!\n", myfile);
                        exit(0);
      */		}
    sleep(4);
  }

  if(fork() != 0) {
    signal(SIGUSR1, parent_interrupt);
    par_wait=0;
    int choice;
    while(1) {
      while(par_wait);
      printf("In which session do you want to give your input?\n");
      for(i=0;i<m;i++)
        printf("%d for %s session\n",i,infile[i]);
      printf("Your choice: ");
      scanf("%d",&choice);
      kill(pid[choice],SIGUSR2);
      par_wait = 1;
      //sleep(5);
    }		
  } else	while(1) {
      newlistenfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
      if(newlistenfd < 0) {
        perror("Connection Error: ");
        exit(1);
      }
      int length;
      strcpy(sendBuff,"");
      for(i=0;i<m;i++) {
        strcat(sendBuff, infile[i]);
        strcat(sendBuff, " ");
      }
      strcat(sendBuff, "\n");
      write(newlistenfd, sendBuff, strlen(sendBuff));
      read(newlistenfd, sendBuff, strlen(sendBuff)-1);
      sscanf(sendBuff,"%d",&assign_child);
      /*		fd = fopen(infile[assign_child], "r");
                while(!feof(fd)) {
                fgets(sendBuff, sizeof(sendBuff), fd);
                write(newlistenfd, sendBuff, strlen(sendBuff));
                }
                fclose(fd);
      */
      int _fd;
      _fd=open(infile[assign_child],O_RDONLY);
      char *data;
      data=(char*) malloc(1000*sizeof(char));
      while(1) {
        n=read(_fd,data, 1000);
        if(n<=0)
          break;
        write(newlistenfd,data,1000);
      }
      close(newlistenfd);
      close(_fd);
      free(data);
      //		write(cfd,"-x-x-x-",9);
    
      /*		if((fd = fopen(infile[assign_child], "r"))==NULL)
                perror("file open error: ");
                while (( length = fread( bytestream, 1, 1024, fd )) > 0 ) {
                write( newlistenfd, bytestream, length);
                }
                fclose(fd);
      */
      kill(pid[assign_child], SIGUSR1);
      sleep(5);
	}

  exit(0);
}

