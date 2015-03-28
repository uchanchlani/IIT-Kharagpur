#include "parser_module.h"

// initiallize the stack
STACK init_stack() {
  STACK S = (STACK)malloc(sizeof(node));
  S->val = 'Z';
  S->next = NULL;
  STACK p = (STACK)malloc(sizeof(node));
  p->val = 'A';
  p->next = S;
  S = p;
  return S;
}

// pops the top element
char pop_stack(STACK *S) {
  STACK p;
  p = *S;
  *S = (*S)->next;
  char val = p->val;
  free(p);
  return val;
}

// pushes the "vals" into the stack
void push_stack(STACK *S, char *vals) {
  if(vals[0]=='0')
	return;
  int i;
  for(i = 0; vals[i]!='\0'; i++);
  i--;
  for(; i >= 0; i--) {
	STACK p = malloc(sizeof(node));
	p->val = vals[i];
	p->next = *S;
	*S = p;
  }
}

// prints the stack values
void print_stack(STACK S, char** meaning) {
  while(S) {
	printf("%s ",get_meaning(meaning,S->val));
	S = S->next;
  }
}

// frees the stack
void free_stack(STACK *S) {
  STACK p = *S, q;
  while(p) {
	q = p;
	p = p->next;
	free(q);
  }
  *S = NULL;
  return;
}

// the parse function
void parse_string(char *S, P_Table P, GRAMMER G, char** meaning) {
  STACK st = init_stack();

  char a = *S, X = st->val, prod_body[20];
  int table_entry, i, flag = 1;

  while(X!='Z' /*&& a!='z'*/) { //Z and z are my end marker symbols 
	// table entry corresponding to the stack top and the current input symbol
	table_entry = P[X - 'A'][a - 'a'];

	// case 1, when the token is matched with the stack top
	// action: pop the stack and move the input string ahead
	if(X==a) {
      pop_stack(&st);
      printf("match %s\t\t",get_meaning(meaning,a));
      a = *(++S);
	}
	
	// case 2 when the table entry corresponding to the stack top is not epmty
	// action: pop the stack top and push the corresponding production symbols
	else if(table_entry > 0) {
      strcpy(prod_body,get_body(G,table_entry));
      printf("%s\t->\t",get_meaning(meaning,get_head(G,table_entry)));
      for(i=0;prod_body[i]!='\0';i++)
		printf("%s ",get_meaning(meaning,prod_body[i]));
      printf("\t");
      pop_stack(&st);
      push_stack(&st, prod_body);
	}
	
	// case 3 when the stack top is a terminal but it doesn't matches with the input symbol
	// action: throw away the stack top
	else if(X>='a'&&X<='z') {
      // error entry
      // handle it in error recovery
      // for the timing just print error
      printf("Error type 1!! Missing token <%s>, the token appearing is <%s>...\t",get_meaning(meaning,X),get_meaning(meaning,a));
      printf("Pop terminal <%s>...\t",get_meaning(meaning,X));
      pop_stack(&st);
      flag = 0;
      //break;
	}

	// case 4 when the table entry is SYNCH
	// action: throw away the stack top
	else if(table_entry == SYNCH || (a=='z')) {
      // error entry
      // handle it in error recovery
      // for the timing just print error
      printf("Error type 2!! ");
      printf("Pop non-terminal <%s>...\t",get_meaning(meaning,X));
      pop_stack(&st);
      flag = 0;
      //break;
	}

	// case 3 when the table entry is a blank entry
	// action: throw away the input character
	else if(table_entry == -1) {
      // error entry
      // handle it in error recovery
      // for the timing just print error
      printf("Error type 3!! ");
      printf("Skip symbol <%s>...\t",get_meaning(meaning,a));
      a = *(++S);
      flag = 0;
      //break;
	}

	// print the remaining string to be parsed
	for(i=0;S[i]!='\0';i++)
      printf("%s",get_meaning(meaning,S[i]));

	// print the current stack position
    //	printf("\t\t");	print_stack(st,meaning);

	printf("\n");

	// X is the new stack top
	X = st->val;
  }

  // at any point if the parser goes to an error case, the flag becomes 0
  if(flag)
	printf("The string is accepted!!\n");
  else
	printf("The string is rejected!!\n");

  // free the stack
  free_stack(&st);
  return;
}

