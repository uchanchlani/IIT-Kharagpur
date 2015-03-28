#include "FSM_state.h"

int match_kywrd(char c, int reset) {
    static int state = 0;
    static int count = 0;
    if(reset) {
	state = 0;
	count = 0;
    }
    else {
	switch(state) {
	case 0: //start state
	    if(isalpha(c)) {
		count++;
		state = 2;
		return count;
	    }
	    else if(c=='_') {
		count++;
		state = 1;
		return count;
	    }
	    else {
		state = 4;
		return -1;
	    }
	    break;

	case 1: //intermediate state
	    if(c=='_') {
		state = 4;
		return -1;
	    }
	    else if(isalpha(c) || isdigit(c)) {
		count++;
		state = 2;
		return count;
	    }
	    else {
		state = 3;
		return count;
	    }
	    break;

	case 2: // is the stable state
	    if(isalpha(c) || isdigit(c) || c=='_') {
		count++;
		state = 2;
		return count;
	    }
	    else {
		state = 3;
		return count;
	    }
	    break;

	case 3: // accept state
	    state = 3;
	    return count;
	    break;
			
	case 4: // reject state
	    state = 4;
	    return -1;
	    break;
			
	default:
	    break;
	}
    }
    return -1;
}



int match_int(char c, int reset) {
    static int state = 0;
    static int count = 0;
    if(reset) {
	state = 0;
	count = 0;
    }
    else {
	switch(state) {
	case 0: // start state
	    if(c == '+' || c == '-') {
		state = 1;
		count ++;
		return count;
	    }
	    else if(isdigit(c)) {
		state = 2;
		count ++;
		return count;
	    }
	    else {
		state =4;
		return -1;
	    }
	    break;

	case 1: // intermediate state
	    if(isdigit(c)) {
		state = 2;
		count ++;
		return count;
	    }
	    else {
		state =4;
		return -1;
	    }
	    break;

	case 2: // stable state
	    if(isdigit(c)) {
		state = 2;
		count ++;
		return count;
	    }
	    else {
		state = 3;
		return count;
	    }
	    break;

	case 3: // accept state
	    state = 3;
	    return count;
	    break;
			
	case 4: // reject state
	    state = 4;
	    return -1;
	    break;
			
	default:
	    break;
	}

    }
    return -1;
}



int match_float(char c, int reset) {
    static int state = 0;
    static int count = 0;
    if(reset) {
	state = 0;
	count = 0;
    }
    else {
	switch(state) {
	case 0: // start state
	    if(c == '+' || c == '-') {
		state = 1;
		count ++;
		return count;
	    }
	    else if(isdigit(c)) {
		state = 2;
		count++;
		return count;
	    }
	    else if(c == '.') {
		state = 3;
		count ++;
		return count;
	    }
	    else {
		state = 6;
		return -1;
	    }
	    break;

	case 1: //  digits before .
	    if(isdigit(c)) {
		state = 2;
		count ++;
		return count;
	    }
	    else if(c == '.') {
		state = 2;
		count ++;
		return count;
	    }
	    else {
		state =6;
		return -1;
	    }
	    break;

	case 2: // stable state
	    if(isdigit(c)) {
		state = 2;
		count ++;
		return count;
	    }
	    else if(c=='.') {
		state = 3;
		count ++;
		return count;
	    }
	    else {
		state = 5;
		return count;
	    }
	    break;

	case 3: // volatile state
	{
	    if(isdigit(c)) {
		state = 4;
		count++;
		return count;
	    }
	    else {
		state = 6;
		return -1;
	    }
	}

	case 4: // stable state
	    if(isdigit(c)) {
		state = 4;
		count ++;
		return count;
	    }
	    else {
		state = 5;
		return count;
	    }
	    break;

			
	case 5: // accept state
	    state = 5;
	    return count;
	    break;
			
	case 6: // reject state
	    state = 6;
	    return -1;
	    break;
				
	default:
	    break;
	
	}
	return -1;
    }
}


int match_arith(char c, int reset) {
    static int state = 0;
    static int count = 0;
    if(reset) {
	state = 0;
	count = 0;
    }
    else {
	switch(state) {
	case 0: // start state
	    if(c=='*' || c=='/' ||c=='%') {
		state = 3;
		count++;
		return count;
	    }
	    else if(c=='+') {
		state = 1;
		count++;
		return count;
	    }
	    else if(c=='-') {
		state = 2;
		count++;
		return count;
	    }
	    else {
		state = 4;
		return -1;
	    }
	    break;
				
	case 1: // + seen
	    if(c=='+') {
		state = 3;
		count++;
		return count;
	    }
	    else {
		state = 4;
		return -1;
	    }
	    break;
				
	case 2: //- seen
	    if(c=='-') {
		state = 3;
		count++;
		return count;
	    }
	    else {
		state = 4;
		return -1;
	    }
	    break;
			
	case 3: // accept state
	    state = 3;
	    return count;
	    break;
			
	case 4: // reject state
	    state = 4;
	    return -1;
	    break;
				
	default:
	    break;
	}
    }
    return -1;
}



int match_assgn(char c, int reset) {
    static int state = 0;
    static int count = 0;
    if(reset) {
	state = 0;
	count = 0;
    }
    else {
	switch(state) {
	case 0:
	    if(c=='=') {
		state = 2;
		count++;
		return count;
	    }
	    else if(c=='+' || c=='-' || c=='*' || c=='/') {
		state = 1;
		count++;
		return count;
	    }
	    else {
		state = 3;
		return -1;
	    }
	    break;
			
	case 1:
	    if(c=='=') {
		state = 2;
		count++;
		return count;
	    }
	    else {
		state = 3;
		return -1;
	    }
	    break;
			
	case 2: // accept state
	    state = 2;
	    return count;
	    break;
			
	case 3: // reject state
	    state = 3;
	    return -1;
	    break;
				
	default:
	    break;
	}
    }
    return -1;
}


int match_relate(char c, int reset) {
    static int state = 0;
    static int count = 0;
    if(reset) {
	state = 0;
	count = 0;
    }
    else {
	switch(state) {
	case 0:
	    if(c=='<' || c=='>') {
		state = 1;
		count ++;
		return count;
	    }
	    else if(c=='=') {
		state = 2;
		count++;
		return count;
	    }
	    else {
		state = 5;
		return -1;
	    }
	    break;
				
	case 1: // stable state
	    if(c=='=') {
		state = 3;
		count++;
		return count;
	    }
	    else {
		state = 4;
		return count;
	    }
	    break;
				
	case 2:
	    if(c=='=') {
		state = 2;
		count++;
		return count;
	    }
	    else {
		state = 5;
		return -1;
	    }
	    break;
				
	case 3:
	    if(c!='=' && c!='<' && c!='>') {
		state = 4;
		return count;
	    }
	    else {
		state = 5;
		return -1;
	    }
	    break;
				
	case 4: // accept state
	    state = 4;
	    return count;
	    break;
			
	case 5: // reject state
	    state = 5;
	    return -1;
	    break;
			
	default:
	    break;
			
	}
    }
    return -1;
}



int catch_spcl_sym(char c, int reset) {
    static int state = 0;
    static int count = 0;
    if(reset) {
	state = 0;
	count = 0;
    }
    else {
	switch(state) {
	case 0:
	    if(c=='(' || c==')' || c==',' || c=='{' || c=='}' || c=='[' || c==']' || c==';') {
		state = 1;
		count++;
		return count;
	    }
	    else {
		state = 2;
		return -1;
	    }
	    break;
				
	case 1: // accept state
	    state = 1;
	    return count;
	    break;
			
	case 2: // reject state
	    state = 2;
	    return -1;
	    break;
				
	default:
	    break;
			
	}
    }
    return -1;
}

int catch_comment(char c, int reset) {
    static int state = 0;
    static int count = 0;
    if(reset) {
	state = 0;
	count = 0;
    }
    else {
	switch(state) {
	case 0: //start state
	    if(c=='/') {
		count++;
		state = 1;
		return count;
	    }
	    else {
		state = 6;
		return -1;
	    }
	    break;

	case 1:
	    if(c=='/') {
		count++;
		state = 2;
		return count;
	    }
	    else if(c=='*') {
		count++;
		state = 3;
		return count;
	    }
	    else {
		state = 6;
		return -1;
	    }
	    break;

	case 2:
	    if(c=='\n' || c=='\0' || c=='\177' || c=='\377') {
		count++;
		state = 5;
		return count;
	    }
	    else {
		count++;
		state = 2;
		return count;
	    }
	    break;

	case 3:
	    if(c=='\0' || c=='\177' || c=='\377') {
		count++;
		state = 5;
		return count;
	    }
	    else if(c=='*') {
		count++;
		state = 4;
		return count;
	    }
	    else {
		count++;
		state = 3;
		return count;
	    }

	case 4:
	    if(c=='/' || c=='\0' || c=='\177' ||c=='\377') {
		count++;
		state = 5;
		return count;
	    }
	    else if(c=='*') {
		count++;
		state = 4;
		return count;
	    }
	    else {
		count++;
		state = 3;
		return count;
	    }
	    break;

	case 5: // accept state
	    state = 5;
	    return count;
	    break;
			
	case 6: // reject state
	    state = 6;
	    return -1;
	    break;
				
	default:
	    break;

	}
    }
    return -1;
}
