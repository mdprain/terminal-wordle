/*
###############
Terminal Wordle
###############

Created by:
	- Matthew Prain
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

#define BUFFER_SIZE 80
#define BLANK '-'
#define NEWLINE '\n'
#define NEWLINE_STRING "\n"
#define NULL_BYTE '\0'
#define MIN 3
#define MAX 9
#define DEFAULT_LEN 5
#define DEFAULT_MAX 6
#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define RESET "\x1B[0m"

/* load_word()
 * ----------------------------------------------------------------------------
 *  Loads all the words from the dictionary file, to be used for valid
 *  guess checking.
 *
 *  dictionary: the dictionary file to be loaded.
 *  dictSize: the size of the dictionary to be loaded.
 *
 *  Returns: the dictionary pointer loaded into memory.
 */
char** load_words(FILE* dictionary, int dictSize) {
    char buffer[BUFFER_SIZE];
    char** words = (char**) malloc(sizeof(char*) * dictSize); 

    for (int i = 0; i < dictSize; i++) {
		char* loaded = fgets(buffer, BUFFER_SIZE, dictionary);

		int wordSize = strlen(loaded);
		words[i] = (char*) malloc(sizeof(char) * wordSize);
		for (int j = 0; j < wordSize; j++) {
	    	words[i][j] = tolower(loaded[j]);
			// Remove any apostrophies
			if (words[i][j] == '\'') {
				words[i][j] = 's';
				wordSize -= 1;
			}
		}

		words[i][wordSize - 1] = NULL_BYTE;
	}

    return words;
}

/* get_random_word() 
 * ----------------------------------------------------------------------------
 *  Selects a random word from the loaded words, based on a given length
 * 
 *  words: the loaded words to select from
 *  size: the size of the loaded words
 *  length: the desired length of the word to select
 * 
 *  Returns: a randomly selected word
 */
char* get_random_word(char** words, int size, int length) {
	char* randomWord = NULL;
	srand(time(NULL));

	while (true) {
		char* testWord = words[rand() % size];
		if (strlen(testWord) == length) {
			randomWord = strdup(testWord);
			break;
		}
	}

	return randomWord;
}

/* get_dict_size()
 * ----------------------------------------------------------------------------
 *  Calculates the length or "size" of the dictionary in terms of lines.
 *  This needs to be calculated for correct memory allocation.
 *
 *  dictionaryLoc: the file path to the dictionary file.
 *
 *  Returns: the dictionary size as an integer.
 */
int get_dict_size(char* dictionaryLoc) {
    FILE* dictionary = fopen(dictionaryLoc, "r");

    int counter = 0;
    int next = 0;

    while (true) {
		next = fgetc(dictionary);
		if (next == EOF) {
	    	break;
		} else if (next == NEWLINE) {
	    	counter++;
		} else {
	    	continue;
		}
    }

    fclose(dictionary);
    return counter;
}

/* check_guess()
 * ----------------------------------------------------------------------------
 *  Checks the guess from the player, and returns the string of the result.
 *  Lowercase is a correct letter in the wrong spot, and uppercase is the
 *  correct letter in the correct spot.
 *
 *  guess, answer: the strings containing the player's guess, and the
 *  correct answer of the game.
 *  wordLength: the length of the word that is being checked.
 *
 *  Returns: the result from the guess, to be printed.
 */
char* check_guess(char* guess, char* answer, int wordLength) {
    char* result = (char*) malloc(sizeof(char) * (wordLength + 1)); 
    char* tempAnswer = strdup(answer);

    // Checking for "green" letters
    for (int i = 0; i < wordLength; i++) {
		if (guess[i] == answer[i]) {
	    	result[i] = toupper(answer[i]);
	    	tempAnswer[i] = BLANK;
		} else {
	    	result[i] = BLANK;
		}
    }

    // Checking for "yellow" letters
    for (int i = 0; i < wordLength; i++) {
		for (int j = 0; j < wordLength; j++) {
	    	if (guess[i] == tempAnswer[j] && guess[i] != answer[i]) {
				result[i] = tolower(guess[i]);
				tempAnswer[j] = BLANK;
				break;
	    	}
		}
    }

    result[wordLength] = NULL_BYTE;
    free(tempAnswer);

    return result;
}

/* print_guess()
 * ----------------------------------------------------------------------------
 *  Prints the guess to the user, with colours for each character
 * 
 *  guess: the guess entered by the user
 */
void print_guess(char* guess) {
	for (int i = 0; i < strlen(guess); i++) {
		if (isupper(guess[i])) {
			printf(GRN "%c" RESET, guess[i]);
		}
		if (islower(guess[i])) {
			printf(YEL "%c" RESET, guess[i]);
		}
		if (guess[i] == BLANK) {
			printf("%c", guess[i]);
		}
	}

	printf(NEWLINE_STRING);
}

/* validate_input()
 * ----------------------------------------------------------------------------
 *  Validates the given input for a guess.
 *
 *  guess: the string entered by the user.
 *  wordLength: the required word length for the guess.
 *  words: the loaded dictionary file for checking.
 *  dictSize: the size of the loaded dictionary file.
 *
 *  Returns: true if the guess is valid, and false otherwise.
 */
bool validate_input(char* guess, int wordLength, char** words, int dictSize) {
    bool checkingGuess = true;
    bool foundWord = false;

    // Remove new line character only if guess isn't empty
    if (strlen(guess) > 1) {
		guess = strtok(guess, NEWLINE_STRING);
    }

    // Checking for empty or bad-sized input
    if (strlen(guess) != wordLength) {
		printf("Words must be %d letters long - try again.\n", wordLength);
		checkingGuess = false;
    } else {
		checkingGuess = true;
    }
    
    // Checking for numbers, then correcting guess to lowercase
    if (checkingGuess) {
		for (int i = 0; i < strlen(guess); i++) {
	    	if (isalpha(guess[i]) == 0) {
				printf("Words must contain only letters - try again.\n");
				checkingGuess = false;
				break;
	    	} else {
				guess[i] = tolower(guess[i]);
				checkingGuess = true;
	    	}
		}
    }
    
    // Checking if word is in dictionary
    if (checkingGuess) {
		for (int i = 0; i < dictSize; i++) {
	    	if (strcmp(guess, words[i]) == 0) {
				foundWord = true;
				break;
	    	}
		}	 
	
		if (!foundWord) {
	    	printf("Word not found in the dictionary - try again.\n");
		}
    }
    
    return checkingGuess && foundWord;
}

/* play_game()
 * ----------------------------------------------------------------------------
 *  Starts the basic gameplay loop of Wordle, and returns the result of the
 *  game.
 *
 *  wordLength, maxGuesses: the integers loaded from either the command line
 *  		or the default settings in main().
 *  dictionary: the dictionary file loaded from either the command line or
 *  		the default setting in main().
 *
 *  Returns: the result status of the game.
 */
int play_game(int wordLength, int maxGuesses, char* dictionaryLoc) {
    char buffer[BUFFER_SIZE];
    int guessesLeft = maxGuesses;
    FILE* dictionary = fopen(dictionaryLoc, "r");
    int dictSize = get_dict_size(dictionaryLoc);
    char** words = load_words(dictionary, dictSize);
    char* answer = get_random_word(words, dictSize, wordLength);
    bool correctGuess = false;
    bool eofQuit = false;

    printf("Welcome to Terminal Wordle!\n");
    while (guessesLeft > 0) {
		if (guessesLeft == 1) {
	    	printf("Enter a %d letter word (last attempt):\n", wordLength);
		} else {
	    	printf("Enter a %d letter word (%d attempts remaining):\n",
		    	wordLength, guessesLeft);
		}
		char* guess = fgets(buffer, BUFFER_SIZE, stdin);

		if (guess == NULL) {
	    	eofQuit = true;
	    	break;
		}

		bool checkedGuess = validate_input(guess, wordLength, words, dictSize);
		if (checkedGuess) {
	    	if (strcmp(guess, answer) == 0) {
				printf(GRN "Correct!\n" RESET);
				correctGuess = true;
				break;
	    	} else {
				char* check = check_guess(guess, answer, wordLength);
				//printf("%s\n", check);
				print_guess(check);
				guessesLeft--;
				free(check);
	    	}
		}
    }

    for (int i = 0; i < dictSize; i++) {
		free(words[i]);
    }
    free(words);
    fclose(dictionary);

    if (correctGuess) {
		free(answer);
		return 0;
    } else {
		fprintf(stderr, RED "Bad luck - the word is \"%s\".\n" RESET, answer);
		free(answer);
		return (eofQuit) ? 1 : 3;
    }
}

/* validate_args()
 * ----------------------------------------------------------------------------
 *  Checks and validates the arguments passed into the program, and exits
 *  or continues as appropriate.
 *
 *  argc, argv: the command line arguments.
 *  wordLength, maxGuesses: the addresses of the word length and maximum
 *  			    guess variables to update.
 *  dictionaryLoc: the address of the dictionary location pointer to update.
 */
void validate_args(int argc, char** argv,
	int* wordLength, int* maxGuesses, char** dictionaryLoc) {
    char* usageMessage = "Usage: wordle [-len word-length]"
 	    " [-max max-guesses] [dictionary]\n";

    if (argc > 6) {
		fprintf(stderr, usageMessage);
		exit(1);
    } else if (argc > 1) {
		for (int i = 0; i < argc; i++) {
	    	if (strcmp(argv[i], "-len") == 0) {
				if (argv[i + 1] == NULL ||
					strlen(argv[i + 1]) > 1 ||
						atoi(argv[i + 1]) == 0 || 
		        		atoi(argv[i + 1]) < MIN ||
		        		atoi(argv[i + 1]) > MAX ||
		        	*wordLength != 5) {
		    		fprintf(stderr, usageMessage);
		    		exit(1);
				} else {
		    		*wordLength = atoi(argv[i + 1]);
				}
   	    	} else if (strcmp(argv[i], "-max") == 0) {
				if (argv[i + 1] == NULL ||
					strlen(argv[i + 1]) > 1 ||
						atoi(argv[i + 1]) == 0 ||
		        		atoi(argv[i + 1]) < MIN ||
						atoi(argv[i + 1]) > MAX ||
					*maxGuesses != 6) {
		    		fprintf(stderr, usageMessage);
		    		exit(1);
				} else {
		    		*maxGuesses = atoi(argv[i + 1]);
				}
	    	} else if (i > 0) {
				if (strcmp(argv[i - 1], "-len") == 0 || 
					strcmp(argv[i - 1], "-max") == 0) {
		    		continue;
				} else if (strcmp(*dictionaryLoc, "/usr/share/dict/words") != 0 || 
						argv[i][0] == '-') {
		    		fprintf(stderr, usageMessage);
		    		exit(1);
				} else {
		    		*dictionaryLoc = argv[i];
				}
	    	}
    	}
    }
}

int main(int argc, char** argv) {
    int wordLength = DEFAULT_LEN;
    int maxGuesses = DEFAULT_MAX; 
    char* dictionaryLoc = "/usr/share/dict/words";
    FILE* dictionary;

    validate_args(argc, argv, &wordLength, &maxGuesses, &dictionaryLoc);

    // Test loading the dictionary file here, exits if invalid file location
    dictionary = fopen(dictionaryLoc, "r");
    if (dictionary == NULL || strcmp(dictionaryLoc, "") == 0) {
		fprintf(stderr, "wordle: dictionary file "
			"\"%s\" cannot be opened\n", dictionaryLoc);
		return 2;
    }
    fclose(dictionary);

    int gameResult = play_game(wordLength, maxGuesses, dictionaryLoc);

    if (gameResult == 0) {
		return 0;
    } else {
		return 3;
    }
}