/*
 * client interacting with the server
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <sys/stat.h>
#include <fcntl.h>

#define CON_PORT 5000
#define debug

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


int main(int argc, char *argv[])
{
  int sockfd = 0,  n = 0, i, pid, count;
  char* recvBuff = (char*)malloc(1024*sizeof(char));
  char* mybuf = (char*)malloc(1024*sizeof(char));
  char* files;
  char infile[10][50];
  char filename[20];
  char *ptok;
  FILE *fd;
  void* bytestream = (void*)malloc(1024);
  strcpy(filename, "test.sh");
  struct sockaddr_in serv_addr; 

  if(argc != 2) {
	perror("Missing, ip of server: ");
	return 1;
  } 

  sockfd = 0; n = 0;

  memset(recvBuff, '0',sizeof(recvBuff));
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	perror("Error: Could not create socket: ");
	return 1;
  } 

  memset(&serv_addr, '0', sizeof(serv_addr)); 
	
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(CON_PORT); 

  if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
	perror("inet_pton error: ");
	return 1;
  } 
		
  if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
	perror("Error: Connect Failed: ");
	return 1;
  } 
  while(1) {
	if((n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) < 0) break;
	recvBuff[n] = '\0';
	strcat(mybuf,recvBuff);
	if(n>0 && recvBuff[n-1]=='\n') break;
  }
  i = 0;
  files = strtok(mybuf, " \t\n");	
  strcpy(infile[i++], files);
  count = 0;
  printf("Choose:\n%d for %s session\n",count++,files);
  while((files=strtok(NULL, " \t\n"))!=NULL) {
	strcpy(infile[i++], files);
	printf("%d for %s session\n",count++,files);
  }
  printf("Your choice: ");
  //	printf("Count your requested file and input 1 number less\n%s\nYour session choice: ", mybuf);
  scanf("%s",recvBuff);
  strcpy(filename,"c");
  strcat(filename,infile[atoi(recvBuff)]);
  write(sockfd, recvBuff, sizeof(recvBuff));
  /*	if((fd = fopen(filename,"w+"))==NULL)
        perror("file open error: ");
        while((n = read(sockfd,bytestream, sizeof(recvBuff)-1)>0)) {
        fwrite( bytestream, 1, 1024, fd );
        }
        fclose(fd);
  */
  int _fd;
  _fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  char *data = (char*)malloc(1000*sizeof(char));
  while(1) {
	n = read(sockfd,data,1000);
	if(n<1000)
      break;
	write(_fd, data,n);
  }
  close(sockfd);
  close(_fd);
  free(data);

  memset(recvBuff, '0',sizeof(recvBuff));
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	perror("Error: Could not create socket: ");
	return 1;
  } 

  memset(&serv_addr, '0', sizeof(serv_addr)); 
	
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(CON_PORT); 

  if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
	perror("inet_pton error:");
	return 1;
  } 
  if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
	perror("Error: Connect Failed: ");
	return 1;
  } 

  while(1) {
	strcpy(mybuf, "");
	while(1) {
      if((n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) < 0) break;
      recvBuff[n] = '\0';
      strcat(mybuf,recvBuff);
      if(n>0 && recvBuff[n-1]=='\n') break;
	}
	if(n < 0) {
      perror("Connection closed: ");
      exit(0);
	}
#ifdef debug
	printf("Command recieved: %s\n", mybuf);
#endif
	parse(mybuf);
	fd = fopen("client.sh", "w+");
	fwrite(mybuf, sizeof(char), strlen(mybuf),fd);
	fclose(fd);
	ptok = strtok(mybuf, "\n \t");
	if(strcmp(ptok,"exit")==0)
      break;

	if(pid = fork()==0) { // child process
      execlp("./client.sh","./client.sh", NULL);
      perror("File Execution Error: ");
      exit(-1);
	}
	else // parent process
      wait(NULL);

  }
  printf("Client exitting...\n");
  exit(0);
}

