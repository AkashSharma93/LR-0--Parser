struct Stack {	//Holds states.
	int states[100];
	int top;
} stack;

void push(int a) {
	stack.top++;
	stack.states[stack.top] = a;
}

void pop() {
	int a = stack.states[stack.top];
	stack.top--;
}

int get_top() {		//Returns top of stack state.
	return stack.states[stack.top];
}

void initialize_stack() {	//Initialize stack to have state 0 on top.
	stack.top = -1;
	
	push(0);
}

int get_int(char *s) {		//Get integer part of the strings found in table entries.
	int i, j;
	char temp[10];
	
	for(i = 0; s[i] != ':'; i++);
	i++;

	for(j = i; s[i] != '\0'; i++)
		temp[i-j] = s[i];
	
	temp[i-j] = '\0';

	return atoi(temp);
}

int get_length(char *production) {	//Returns length of string in the production body.
	int i, j;

	for(i = 0; production[i] != '>'; i++);
	i++;

	for(j = 0; production[i] != '\0'; i++, j++);

	return j;
}


//Start of functions meant only for displaying the result. (Doesn't affect the actual string parsing)

void get_stack_contents(char *t) {	//Stores stack contents in t.
	int i;
	char c[5];

	strcpy(t, "$");

	for(i = 0; i <= stack.top; i++) {
		int n = stack.states[i];
		sprintf(c, "%d", n);
		strcat(t, c);
	}
}

void get_remaining_input(char *string, int index, char *t) {	//Stores remaining Input string in t.
	int i, j;

	for(i = index, j = 0; string[i] != '\0'; i++, j++)
		t[j] = string[i];

	t[j] = '\0';
}

void print_contents(char *string, int index, char *matched_string) {	//Prints the required stuff.
	char t1[20], t2[20];

	get_stack_contents(t1);
	get_remaining_input(string, index, t2);

	printf("\t|  %-25s  |  %-25s  |   %25s  |  \t", t1, matched_string, t2);
}

//End of functions meant only for displaying the result.


void parse() {
	char string[100];
	char matched_string[100];

	initialize_stack();

	printf("\nEnter a string: ");
	scanf("%s", string);

	strcat(string, "$");	//Appending $ to end of input string.
	matched_string[0] = '\0';
	
	printf("\nThe reduction steps for the given string are as follows...\n\n");

	printf("\t|  %-25s  |  %-25s  |  %25s  |  \t%-30s\n\n", "Stack", "Matched String", "Input String", "Action");
	
	int index = 0, m_index = 0;
	
	while(1) {
		char a = string[index];

		print_contents(string, index, matched_string);

		if(table.ACTION[get_top()][get_pos(0, a)][0] == 'S') {		//Shift Action. (Table entry starts with char 'S')
			int t = get_int(table.ACTION[get_top()][get_pos(0, a)]);
			push(t);	//Push state t onto stack.
			index++;

			//Printing the result.
			char t1[20];
			char state[5];

			strcpy(t1, "Shift ");
			sprintf(state, "%d", t);
			strcat(t1, state);

			matched_string[m_index++] = a;
			matched_string[m_index] = '\0';

			printf("%-30s\n", t1);
		}

		else if(table.ACTION[get_top()][get_pos(0, a)][0] == 'R') {	//Reduce Action.
			int i, j = get_int(table.ACTION[get_top()][get_pos(0, a)]);

			for(i = 0; i < get_length(augmented_grammar[j]); i++)	//Pop "length of string" times, w.r.t production 'j'.
				pop();
			
			int t = get_top();
			char A = augmented_grammar[j][0];	//Production head of 'j'th production. (non-terminal)

			push(table.GOTO[t][get_pos(1, A)]);	//Push state using GOTO of the table.

			//Printing the result.
			m_index -= get_length(augmented_grammar[j]);
			matched_string[m_index++] = A;
			matched_string[m_index] = '\0';

			char t1[20];
			strcpy(t1, "Reduce by ");
			strcat(t1, augmented_grammar[j]);

			printf("%-30s\n", t1);
		}

		else if(table.ACTION[get_top()][get_pos(0, a)][0] == 'a') {	//Acceptance.
			printf("%-30s\n", "Accept!!");
			break;
		}

		else {								//Error.
			printf("%-30s\n", "Error!!\n\n");
			printf("String doesn't belong to the language of the particular grammar!\n");
			exit(0);
		}
	}

	printf("\nString accepted!\n");
}
