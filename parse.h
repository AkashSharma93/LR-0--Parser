struct Stack {
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

int get_top() {
	return stack.states[stack.top];
}

void initialize_stack() {
	stack.top = -1;
	
	push(0);
}

int get_int(char *s) {
	int i, j;
	char temp[10];
	
	for(i = 0; s[i] != ':'; i++);
	i++;

	for(j = i; s[i] != '\0'; i++)
		temp[i-j] = s[i];
	
	temp[i-j] = '\0';

	return atoi(temp);
}

int get_length(char *production) {
	int i, j;

	for(i = 0; production[i] != '>'; i++);
	i++;

	for(j = 0; production[i] != '\0'; i++, j++);

	return j;
}

void parse() {
	char string[100];
	printf("\nEnter a string: ");
	scanf("%s", string);

	strcat(string, "$");
	
	printf("\nThe reduction steps for the given string are as follows...\n\n");
	
	int index = 0;
	
	while(1) {
		char a = string[index];

		if(table.ACTION[get_top()][get_pos(0, a)][0] == 'S') {
			int t = get_int(table.ACTION[get_top()][get_pos(0, a)]);
			push(t);
			index++;
		}

		else if(table.ACTION[get_top()][get_pos(0, a)][0] == 'R') {
			int i, j = get_int(table.ACTION[get_top()][get_pos(0, a)]);

			for(i = 0; i < get_length(augmented_grammar[j]); i++)
				pop();
			
			int t = get_top();
			char A = augmented_grammar[j][0];

			push(table.GOTO[t][get_pos(1, A)]);

			printf("%s\n", augmented_grammar[j]);
		}

		else if(table.ACTION[get_top()][get_pos(0, a)][0] == 'a')
			break;

		else {
			printf("String doesn't belong to the language of the particular grammar!\n");
			exit(0);
		}
	}

	printf("\nString accepted!\n");
}
