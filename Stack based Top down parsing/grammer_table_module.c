#include "grammer_table_module.h"

/* Loads the grammer from the filename S */
void load_grammer(char *S, GRAMMER *G) {
  // G contains the heads and bodies of all productions and will also assign them some id
  if((*G)!=NULL) {
	printf("\nLIST not empty. Pass an empty list!!");
	return;
  }

  GRAMMER p = NULL;
  int count = 0;
	
  //used to take body of a production as a string and then store it in the struct
  char buffer[20], buffer1[5];
  FILE *fp;
  fp = fopen(S, "r"); //S is the string name entered
  if(fp==NULL) {
	printf("\nFile doesn't exists!");
	return;
  }
	
  while(!feof(fp)) {
	GRAMMER temp = (GRAMMER)malloc(sizeof(grm));
	temp->next = NULL;
	fscanf(fp, "%s %s", buffer1, buffer);
	temp->head = buffer1[0];
	temp->body = (char*)malloc((1+strlen(buffer))*sizeof(char));
	strcpy(temp->body,buffer);
	buffer[0] = '\0'; // cleaning the buffer
	temp->id = ++count;
	if(p==NULL && strcmp(temp->body,"")) {
      p = temp;
      *G = temp;
	}
	else if(strcmp(temp->body,"")) {
      p->next = temp;
      p = p->next;
	}
	else 
      free(temp);
  }
  fclose(fp);
  return;
}

char get_head(GRAMMER G, int index) {
  if(G == NULL)
	return '\0';
  GRAMMER p = G;
  while(p!=NULL) {
	if(p->id == index)
      return p->head;
	p = p->next;
  }
  return '\0';
}

char* get_body(GRAMMER G, int index) {
  if(G == NULL)
	return NULL;
  GRAMMER p = G;
  while(p!=NULL) {
	if(p->id == index)
      return p->body;
	p = p->next;
  }
  return NULL;
}

void print_grammer(GRAMMER G) {
  if(G==NULL)
	printf("No productions in the current grammer\n");
  while(G) {
	printf("%d %c %s\n",G->id,G->head,G->body);
	G = G->next;
  }
  return;
}

void free_grammer(GRAMMER *G) {
  GRAMMER p = *G, q;
  while(p) {
	q = p;
	p = p->next;
	free(q->body);
	free(q);
  }
  *G = NULL;
  return;
}



void load_FirstFollow(char *S, FstFlw *FF) {
  // G contains the heads and first-follow of all NTs
  if((*FF)!=NULL) {
	printf("\nLIST not empty. Pass an empty list!!");
	return;
  }

  FstFlw p = NULL;
  //    int i;
	
  //used to take first-follow of a NT as a string and then store it in the struct
  char buffer[5], buffer1[20], buffer2[20];
  FILE *fp;
  fp = fopen(S, "r"); //S is the string name entered
  if(fp==NULL) {
	printf("\nFile doesn't exists!");
	return;
  }
	
  while(!feof(fp)) {
	FstFlw temp = (FstFlw)malloc(sizeof(fflw));
	temp->next = NULL;
	fscanf(fp, "%s %s %s", buffer, buffer1, buffer2);
	temp->head = buffer[0];
	temp->first = (char*)malloc((1+strlen(buffer1))*sizeof(char));
	strcpy(temp->first,buffer1);
	temp->follow = (char*)malloc((1+strlen(buffer2))*sizeof(char));
	strcpy(temp->follow,buffer2);
	buffer1[0] = '\0'; buffer2[0] = '\0'; // cleaning the buffers
	if(p==NULL && strcmp(temp->first,"")) {
      p = temp;
      *FF = temp;
	}
	else if(strcmp(temp->first,"")) {
      p->next = temp;
      p = p->next;
	}
	else 
      free(temp);
  }
  fclose(fp);
  return;
}

char* get_first(FstFlw FF, char key) {
  if(FF == NULL)
	return NULL;
  FstFlw p = FF;
  while(p!=NULL) {
	if(p->head == key)
      return p->first;
	p = p->next;
  }
  return NULL;
}

char* get_follow(FstFlw FF, char key) {
  if(FF == NULL)
	return NULL;
  FstFlw p = FF;
  while(p!=NULL) {
	if(p->head == key)
      return p->follow;
	p = p->next;
  }
  return NULL;
}

void free_firstFollow(FstFlw *FF) {
  FstFlw p = *FF, q;
  while(p) {
	q = p;
	p = p->next;
	free(q->first);
	free(q->follow);
	free(q);
  }
  *FF = NULL;
  return;
}



P_Table create_parse_table(int size) {
  P_Table P = (P_Table)malloc(size*sizeof(int*));
  int i,j;
  for(i = 0; i < size; i++)
	P[i] = (int*)malloc(size*sizeof(int));
  for(i = 0; i < size; i++)
	for(j = 0; j < size; j++)
      P[i][j] = -1;
  return P;
}

void fill_parse_table(P_Table P, GRAMMER G, FstFlw F) {
  char first[20], follow[20], body[20];
  int non_term, in_sym, prod_numb, flag, i;
  while(G) {
	flag = 0;
	prod_numb = G->id;
	non_term = G->head - 'A';
	strcpy(body,G->body);

	if((body[0]>='a' && body[0]<='z') || body[0]=='0') {
      first[0] = body[0];
      first[1] = '\0';
	}
	else if(body[0]>='A' && body[0]<='Z')
      strcpy(first, get_first(F,body[0]));

	for(i = 0; first[i]!='\0'; i++) {
      if(first[i]!='0') {
		in_sym = first[i] - 'a';
		P[non_term][in_sym] = prod_numb;
      }
      else
		flag = 1;
	}
	strcpy(follow,get_follow(F,G->head));
	for(i = 0; follow[i]!='\0'; i++) {
      in_sym = follow[i] - 'a';
      if(flag) {
		P[non_term][in_sym] = prod_numb;
      }
      else {
		if(P[non_term][in_sym] == -1)
          P[non_term][in_sym] = SYNCH; //synchronizing set
      }
	}
	G = G->next;
  }
}

void free_parse_table(P_Table *P, int size) {
  P_Table q = *P;
  int i;
  for(i = 0; i < size; i++) {
	free(q[i]);
	q[i] = NULL;
  }
  free (q);
  *P = NULL;
  return;
}



char** load_meaning_set(char* S) {
  char** M = (char**)malloc(256*sizeof(char*));
  //used to take body of a production as a string and then store it in the struct
  char buffer[50], buffer1[5];
  int count = 0;
  FILE *fp;
  fp = fopen(S, "r"); //S is the string name entered
  if(fp==NULL) {
	printf("\nFile doesn't exists!");
	return NULL;
  }
	
  while(!feof(fp)) {
	fscanf(fp, "%s %s", buffer1,buffer);
	if(strcmp(buffer,"")) {
      if(M[buffer1[0]]!=NULL)
		free(M[buffer1[0]]);
      M[buffer1[0]] = malloc(strlen(buffer)*sizeof(char));
      strcpy(M[buffer1[0]],buffer);
	}
  }
  fclose(fp);
  return M;
}

char* get_meaning(char** M, char key) {
  return M[(int)key];
}

void free_meaning_set(char*** M) {
  char** q = *M;
  int i;
  for(i = 0; i < 256; i++) {
	free(q[i]);
	q[i] = NULL;
  }
  free (q);
  *M = NULL;
  return;
}

