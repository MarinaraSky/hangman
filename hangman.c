#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* Length of Supercalifragilisticexpialidocious + 1 */
#define MAX_LENGTH 		35
#define LOSS_COUNT 		6
#define GUESS_LENGTH 	2

/* Used to print usage in Error cases */
void
printUsage(void);

/* Used to validate user input or file data * 
 * and changes case if not lower case 		*/
int
validateInput(char *guess);

/* Takes the valid input and compares it against the secret word. * 
 * Changes the display word if correct letter was guessed 		  */
int
checkGuess(char *guess, char *secretWord, char *displayWord);

/* main driver for game play */
void
playGame(char *secretWord, char *displayWord);

/* Opens a wordlist, and selects a random	* 
 * valid word from it to play hangman 		*/
void
readFile(char *secretWord, char *filename);

/* Reads .hangman file for stats if its there, 	*
 * if its not treats as first game 				*/
void
getStats(int *win, int *loss, int *totalGuess);

/* Saves current stats to .hangman file */
void
saveStats(int *win, int *loss, int *totalGuess);

int 
main(int argc, char *argv[])
{
	char secretWord[MAX_LENGTH];
	char filename[] = ".words";

	if(argc == 1)
	{
		readFile(secretWord, filename);
	}
	else if(argc == 2)
	{
		strcpy(filename, argv[1]);
		readFile(secretWord, filename);
	}
	else
	{
		printf("Wrong amount of arguments\n");
		printUsage();
	}
	printf("%s\n", secretWord);

	char *displayWord = malloc(sizeof(char) * strlen(secretWord) + 1);
	memset(displayWord, 0, strlen(secretWord) + 1);
	for(unsigned int i = 0; i < strlen(secretWord); i++)
	{
		displayWord[i] = '_';
	}
	playGame(secretWord, displayWord);
	free(displayWord);
	return 0;
}

void
printUsage(void)
{
	printf("USAGE: ./hangman [filename]\n");
}

int
validateInput(char *guess)
{
	for(unsigned int i = 0; i < strlen(guess); i++)
	{
		if(isalpha(guess[i]))
		{
			if(isupper(guess[i]))
			{
				guess[i] = tolower(guess[i]);
			}
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

int
checkGuess(char *guess, char *secretWord, char *displayWord)
{
	int found = 0;
	for(unsigned int i = 0; i < strlen(secretWord); i++)
	{
		if(guess[0] == secretWord[i])
		{
			displayWord[i] = guess[0];
			found ++;
		}
	}
	if(found)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void 
playGame(char *secretWord, char *displayWord)
{
	int notWinning = 1;
	int guessCount = 0;
	int win;
	int loss;
	int totalGuess;

	getStats(&win, &loss, &totalGuess);
	printf("win: %d, loss: %d, TG: %d\n", win, loss, totalGuess);
	while(notWinning && guessCount < LOSS_COUNT)
	{
		char guess[GUESS_LENGTH];
		char buff[MAX_LENGTH];
		int garbage;

		printf("%s\n", displayWord);
		printf("Please enter a letter : ");
		fgets(buff, GUESS_LENGTH, stdin);
		while((garbage = getchar()) != '\n');
		strcpy(guess, buff);
		guess[1] = 0;
		if(validateInput(guess))
		{
			if(checkGuess(guess, secretWord, displayWord))
			{
				if(strcmp(secretWord, displayWord) == 0)
				{
					notWinning = 0;
				}
			}
			else
			{
				guessCount++;
			}
		}
		else
		{
			printf("Invalid\n");
		}
	}
	if(notWinning == 0)
	{
		printf("You win with %d chances left.\n",  LOSS_COUNT - guessCount);
		win++;
		totalGuess += guessCount;
	}
	else
	{
		printf("You lose.\n");
		loss++;
	}
	saveStats(&win, &loss, &totalGuess);
}

void
readFile(char *secretWord, char *filename)
{
	int random = 0;
	int readWordSize = 0;
	FILE *wordList;
	char *readWord;
	long unsigned int readWordLength = 0;
	int validLines = 0;
	int totalLines = 0;

	wordList = fopen(filename, "r");
	if(wordList == NULL)
	{
		printf("Wordlist does not exist.\n");
		printUsage();
		exit(1);
	}
	while((readWordSize = getline(&readWord, &readWordLength, wordList)) != -1)
	{
		readWord[readWordSize - 1] = 0;
		if(readWordSize > MAX_LENGTH)
		{
			printf("Word in file is to large.\n");
			exit(2);
		}
		if(validateInput(readWord) == 1 && strlen(readWord) != 0)
		{
			validLines++;
		}
		totalLines++;
	}
	do
	{
		if(validLines == 0)
		{
			printf("No Valid Words found\n");
			exit(2);
		}
		srand(time(0));
		random = rand() % totalLines;
		fseek(wordList, 0, SEEK_SET);
		totalLines = 0;
		while(totalLines <= random)
		{
			readWordSize = getline(&readWord, &readWordLength, wordList);
			if(totalLines == random)
			{
				readWord[readWordSize - 1] = 0;
				strcpy(secretWord, readWord);
			}
			totalLines++;
		}
	}while(validateInput(secretWord) == 0);
	fclose(wordList);
}

void
getStats(int *win, int *loss, int *totalGuess)
{
	FILE *stats;
	char *statsLine;
	long unsigned int statsLineLength = 0;

	stats = fopen(".hangman", "r");
	if(stats == NULL)
	{
		*win = 0;
		*loss = 0;
		*totalGuess = 0;
	}
	else
	{
		getline(&statsLine, &statsLineLength, stats);
		if(sscanf(statsLine, "%d %d %d", win, loss, totalGuess) != 3)
		{
			printf("Corrupted .hangman file.\n");
			*win = 0;
			*loss = 0;
			*totalGuess = 0;
		}
		fclose(stats);
	}	
}

void
saveStats(int *win, int *loss, int *totalGuess)
{
	FILE *stats;
	char *statsLine;
	
	stats = fopen(".hangman", "w+");
	fprintf(stats, "%d %d %d", *win, *loss, *totalGuess);
	fclose(stats);
}
