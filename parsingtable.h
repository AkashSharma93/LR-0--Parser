//Parsing Table.
struct Parsing_Table {	//Structure to represent the Parsing Table.	
	char ACTION[30][100][100];
	int GOTO[30][100];
} table;

void initialize_table() {  //Initialize all entries to indicate Error.
	int i, j;

	for(i = 0; i < no_of_states; i++) {
		for(j = 0; j < no_of_terminals; j++)
			strcpy(table.ACTION[i][j], "e");

		for(j = 0; j < no_of_nonterminals; j++)
			table.GOTO[i][j] = -1;
	}
}

void print_table() {
	int i, j;

	printf("\nThe Parsing Table for the given grammar is...\n\n");

	printf("%10s   ", "");
	
	for(i = 0; i < no_of_terminals; i++)
		printf("%10c", terminals[i]);

	printf(" | ");
	
	for(i = 1; i < no_of_nonterminals; i++)
		printf("%10c", nonterminals[i]);

	printf("\n\n");

	for(i = 0; i < no_of_states; i++) {
		printf("%10d | ", i);

		for(j = 0; j < no_of_terminals; j++) {
			if(!strcmp(table.ACTION[i][j], "e"))
				printf("%10s", ".");
			else
				printf("%10s", table.ACTION[i][j]);
		}

		printf(" | ");

		for(j = 1; j < no_of_nonterminals; j++) {
			if(table.GOTO[i][j] == -1)
				printf("%10s", ".");
			else
				printf("%10d", table.GOTO[i][j]);
		}

		printf("\n");
	}
}

void Goto(int i, int item, char *temp) {	//Computes goto for 'item'th item of 'i'th state.
	char t;

	strcpy(temp, items[i][item]);

	for(i = 0; temp[i] != '\0'; i++)
		if(temp[i] == '.') {
			t = temp[i];
			temp[i] = temp[i+1];
			temp[i+1] = t;
			break;
		}
}

int get_state(char *t, int state) {	//Returns the state of a given item.
	int i, j;

	for(i = state; i < (no_of_states + state); i++) {	//Start searching from current state and then wrap around.
		for(j = 0; j < no_of_items[i % no_of_states]; j++) {
			if(!strcmp(t, items[i % no_of_states][j]))
				return i % no_of_states;
		}
	}

	printf("No match for string! (%s)\n", t);
}

int get_pos(int flag, char symbol) {	//Returns index of a terminal or a non-terminal from the corresponding arrays.
	int i;

	if(flag == 0)
		for(i = 0; i < no_of_terminals; i++) {
			if(terminals[i] == symbol) 
				return i;
		}
	else
		for(i = 0; i < no_of_nonterminals; i++) {
			if(nonterminals[i] == symbol) 
				return i;
		}				

	if(flag == 0)
		printf("Terminal not found in get_pos! (%c)\n", symbol);
	else
		printf("Non-terminal not found in get_pos! (%c)\n", symbol);
}

int get_production_no(char * item) {	//Given an item, it returns the production number of the equivalent production.
	int i, j;
	
	char production[20];

	for(i = 0, j = 0; item[i] != '\0'; i++)
		if(item[i] != '.') {
			production[j] = item[i];
			j++;
		}
	
	if(j == 3) {		//If it's an epsilon production, the production won't have a body.
		production[j] = '@';
		j++;
	}
	
	production[j] = '\0';

	for(i = 0; i < no_of_productions; i++) {
		if(!strcmp(production, augmented_grammar[i]))
			return i;
	}

	printf("Production not found! (%s)\n", production);
}

void compute_action() {
	int i, item, j;
	char temp[100], symbol;
	
	for(i = 0; i < no_of_states; i++) {
		for(item = 0; item < no_of_items[i]; item++) {
			char *s = strchr(items[i][item], '.');	//Returns a substring starting with '.'
			
			if(!s) {	//In case of error.
				printf("Item not found! State = %d, Item = %d\n", i, item);
				exit(-1);
			}

			if(strlen(s) > 1) {	//dot is not at end of string. SHIFT ACTION!!
				if(isterminal(s[1])) {		//For terminals. Rule 1.
					if(strcmp(table.ACTION[i][get_pos(0,s[1])], "e")) {		//Multiple entries conflict.
						printf("\n\nConflict(1): Multiple entries found for (%d, %c)\n", i, s[1]);
						printf("\nGrammar is not in LR(0)!\n");
						exit(-1);
					}

					char state[3];
	
					Goto(i, item, temp);	//Store item in temp.
					j = get_state(temp, i);

					sprintf(state, "%d", j);
					strcpy(temp, "S:");
					strcat(temp, state);

					strcpy(table.ACTION[i][get_pos(0, s[1])], temp);
				}
				
				else {		//For non-terminals. Rule 4.
					
					Goto(i, item, temp);	//Store item in temp.
					j = get_state(temp, i);

					if(table.GOTO[i][get_pos(1, s[1])] == -1)	//To avoid multiple entries.
						table.GOTO[i][get_pos(1, s[1])] = j;
				}
			}

			else {	//dot is at end of string. Rule 2. REDUCE ACTION!!	
				char f[10], production_no[3];
				int k, n;
				n = get_production_no(items[i][item]);		//Get production number from Augmented Grammar.

				sprintf(production_no, "%d", n);
				strcpy(temp, "R:");
				strcat(temp, production_no);
				
				strcpy(f, FOLLOW[get_pos(1, items[i][item][0])]);	//Get follow of production head.
				for(k = 0; f[k] != '\0'; k++) {
					if(strcmp(table.ACTION[i][get_pos(0, f[k])], "e")) {		//Multiple entries conflict.
						printf("\n\nConflict(3): Multiple entries found for (%d, %c)\n", i, f[k]);
						printf("\nGrammar is not in LR(0)!\n");
						exit(-1);
					}

					strcpy(table.ACTION[i][get_pos(0, f[k])], temp);
				}
			}
		}
	}

	strcpy(table.ACTION[1][get_pos(0, '$')], "acc");	//Accept-entry for item [S'->S.]
}

void create_parsing_table() {
	initialize_table();

	compute_action();

	print_table();
}

//End of Parsing Table.
