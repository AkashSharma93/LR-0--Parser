int epsilon_flag = 0;

initialize_first_follow() {	//Initialize to null strings.
	int i;

	for(i = 0; i < no_of_terminals; i++)
		FIRST[0][i][0] = '\0';

	for(i = 0; i < no_of_nonterminals; i++) {
		FIRST[1][i][0] = '\0';
		FOLLOW[i][0] = '\0';
	}
}

void add_symbol(int flag, char *f, char *s) {		//Adds a symbol to FIRST or FOLLOW if it doesn't already exist in it.
	int i, j;
	int found;

	if(flag == 0) {		//For FIRST.
		for(i = 0; i < strlen(s); i++) {
			found = 0;

			for(j = 0; j < strlen(f); j++) {
				if(s[i] == f[j])
					found = 1;
			}

			if(!found) {
				char temp[2];
				temp[0] = s[i];
				temp[1] = '\0';
				strcat(f, temp);
			}
		}
	}

	else {		//For FOLLOW.
		for(i = 0; i < strlen(s); i++) {
			found = 0;

			if(s[i] == '@') {
				epsilon_flag = 1;
				continue;
			}

			for(j = 0; j < strlen(f); j++) {
				if(s[i] == f[j])
					found = 1;
			}

			if(!found) {
				char temp[2];
				temp[0] = s[i];
				temp[1] = '\0';
				strcat(f, temp);
			}
		}
	}
}

void first(char s) {
	if(isterminal(s)) {	//For terminals.
		FIRST[0][get_pos(0, s)][0] = s;
		FIRST[0][get_pos(0, s)][1] = '\0';
	}

	else {		//For non-terminals.
		int i, flag = 0;
		for(i = 0; i < no_of_productions; i++) {
			if(augmented_grammar[i][0] == s) {		//Productions with head as s.
				int j;

				for(j = 0; augmented_grammar[i][j] != '>'; j++);
				j++;
				char next_sym = augmented_grammar[i][j];

				if(next_sym == '@') {	//Epsilon Production.
					add_symbol(0, FIRST[1][get_pos(1, s)], "@");
					flag = 1;
				}

				else {
					if(next_sym == s) {		//In case of left recursion, to avoid infinite loop.
						if(flag)
							next_sym = augmented_grammar[i][++j];
						else
							continue;
					}

					first(next_sym);		//Recursive call, to find FIRST of next symbol.

					if(isterminal(next_sym)) 	//Add first of next symbol to first of current symbol.
						add_symbol(0, FIRST[1][get_pos(1, s)], FIRST[0][get_pos(0, next_sym)]);

					else
						add_symbol(0, FIRST[1][get_pos(1, s)], FIRST[1][get_pos(1, next_sym)]);
				}
			}
		}
	}
}

void compute_first() {
	int i;

	for(i = 0; i < no_of_terminals; i++)
		first(terminals[i]);
	
	for(i = 0; i < no_of_nonterminals; i++)
		first(nonterminals[i]);

//	for(i = 0; i < no_of_nonterminals; i++)
//		printf("%s\n", FIRST[1][get_pos(1, nonterminals[i])]);
}



//FOLLOW

void follow(char s) {
	if(s == nonterminals[0]) 
		add_symbol(1, FOLLOW[0], "$");

	else if(s == nonterminals[1])
		add_symbol(1, FOLLOW[1], "$");
	
	int i, j;
	for(i = 0; i < no_of_productions; i++) {
		for(j = 3; j < strlen(augmented_grammar[i]); j++) {
			epsilon_flag = 0;

			if(augmented_grammar[i][j] == s) {
				char next_sym = augmented_grammar[i][j+1];

				if(next_sym != '\0') {		//If current symbol is not the last symbol of production body.
					if(isterminal(next_sym))	//For terminals.
						add_symbol(1, FOLLOW[get_pos(1, s)], FIRST[0][get_pos(0, next_sym)]);
					else {		//For non-terminals.
						add_symbol(1, FOLLOW[get_pos(1, s)], FIRST[1][get_pos(1, next_sym)]);

						if(epsilon_flag) {	//If FIRST[next_sym] has epsilon, find FOLLOW[next_sym].
							follow(next_sym);
							add_symbol(1, FOLLOW[get_pos(1, s)], FOLLOW[get_pos(1, next_sym)]);
						}
					}
				}

				else {		//If current symbol is the last symbol of production body.
					follow(augmented_grammar[i][0]);	//Follow of production head.
					add_symbol(1, FOLLOW[get_pos(1, s)], FOLLOW[get_pos(1, augmented_grammar[i][0])]);
				}
			}
		}
	}
}

compute_follow() {
	int i;
	
	for(i = 0; i < no_of_nonterminals; i++)
		follow(nonterminals[i]);

//	for(i = 0; i < no_of_nonterminals; i++)
//		printf("%s\n", FOLLOW[get_pos(1, nonterminals[i])]);
}
