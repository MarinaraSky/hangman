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
getStats(int *win, int *loss, int *totalGuess, int *totalTime,
		 int *currentStreak, int *highStreak);

/* Saves current stats to .hangman file */
void
saveStats(int *win, int *loss, int *totalGuess, int *totalTime, 
		  int *currentStreak, int *highStreak);

/* Prints a banner that shows previous stats */
void
printBanner(int win, int loss, int totalGuess, int totalTime,
		    int currentStreak, int highStreak);

void
printHungman(int currentGuess);

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

	char *displayWord = malloc(sizeof(char) * strlen(secretWord) + 1);
	memset(displayWord, 0, strlen(secretWord) + 1);
	printf("%s\n", secretWord);
	size_t secretLen = strlen(secretWord);

	for(size_t i = 0; i < secretLen; i++)
	{
		if((ispunct(secretWord[i])))
		{
			displayWord[i] = secretWord[i];
		}
		else
		{
			displayWord[i] = '_';
		}
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
	int isAlphaChar = 0;
	for(size_t i = 0; i < strlen(guess); i++)
	{
		if(isalpha(guess[i]) || ispunct(guess[i]))
		{
			if(isalpha(guess[i]))
			{
				isAlphaChar++;
			}
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
	if(isAlphaChar == 0)
	{
		return 0;
	}
	return 1;
}

int
checkGuess(char *guess, char *secretWord, char *displayWord)
{
	int found = 0;
	for(size_t i = 0; i < strlen(secretWord); i++)
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
	int totalTime;
	int currentStreak;
	int highStreak;
	int startTime = time(0);

	getStats(&win, &loss, &totalGuess, &totalTime, 
				&currentStreak, &highStreak);
	printBanner(win, loss, totalGuess, totalTime, currentStreak, highStreak);
	while(notWinning && guessCount < LOSS_COUNT)
	{
		char guess[GUESS_LENGTH];
		char buff[MAX_LENGTH];
		int garbage;

		printHungman(guessCount);
		printf("%s\n", displayWord);
		printf("Please enter a letter : ");
		fgets(buff, GUESS_LENGTH, stdin);
		while((garbage = getchar()) != '\n');
		strcpy(guess, buff);
		guess[GUESS_LENGTH - 1] = 0;
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
		currentStreak++;
		if(currentStreak > highStreak)
		{
			highStreak = currentStreak;
		}
	}
	else
	{
		printf("You lose.\n");
		loss++;
		currentStreak = 0;
	}
	totalTime += time(0) - startTime;
	printf("Time played this round(seconds): %ld\n", time(0) - startTime);
	saveStats(&win, &loss, &totalGuess, &totalTime, 
				&currentStreak, &highStreak);
}

void
readFile(char *secretWord, char *filename)
{
	int random = 0;
	int readWordSize = 0;
	FILE *wordList;
	char *readWord = NULL;
	long unsigned int readWordLength = 0;
	int validLines = 0;
	int badLines = 0;
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
		srand(time(NULL));
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
		badLines++;
	}while(validateInput(secretWord) == 0 && badLines <= 2);
	free(readWord);
	fclose(wordList);
	if(badLines >= 2)
	{
		printf("Bad Word file.\n");
		exit(2);
	}
}

void
getStats(int *win, int *loss, int *totalGuess, int *totalTime,
		 int *currentStreak, int *highStreak)
{
	FILE *stats;
	char *statsLine = NULL;
	long unsigned int statsLineLength = 0;
	
	*win = 0;
	*loss = 0;
	*totalGuess = 0;
	*totalTime = 0;
	*currentStreak = 0;
	*highStreak = 0;

	stats = fopen(".hangman", "r+");
	if(stats != NULL)
	{
		getline(&statsLine, &statsLineLength, stats);
		sscanf(statsLine, "%d %d %d %d %d %d", win, loss, totalGuess, 
				totalTime, currentStreak, highStreak);
		free(statsLine);
		fclose(stats);
	}
	else
	{
		printf("Corrupted or missing .hangman file\n");
	}
}

void
saveStats(int *win, int *loss, int *totalGuess, int *totalTime,
		  int *currentStreak, int *highStreak)
{
	FILE *stats;
	
	stats = fopen(".hangman", "w+");
	fprintf(stats, "%d %d %d %d %d %d", *win, *loss, *totalGuess,
			*totalTime, *currentStreak, *highStreak);
	fclose(stats);
}

void
printBanner(int win, int loss, int totalGuess, int totalTime,
				int currentStreak, int highStreak)
{
	float average = 0.0;
	float averageTime = 0.0;

	printf("**********************************\n");
	printf("Welcome to Jack Spence's Super Fun\n");
	printf("            Hangman				  \n");
	printf("Records:\nW/L:\t\t\t%d/%d\n", win, loss);
	if(win + loss != 0)
	{
		average = (float) totalGuess / (win + loss);
		averageTime = (float) totalTime / (win + loss);
	}
	printf("Avg Score: \t\t%.2f\n", average);
	printf("Avg Time: \t\t%.2f\n", averageTime);
	printf("Longest Win Streak:\t%d \n", highStreak);
	printf("Current Win Streak:\t%d \n", currentStreak);
	printf("**********************************\n");
}	

void
printHungman(int currentGuess)
{
	printf("\t");
	for(int i = 0; i < 9; i++)
	{
		if(i == 0)
		{
			printf("\u2554");
		}
		else if(i == 8)
		{
			printf("\u2557");
		}
		else
		{
			printf("\u2550");
		}
	}
	printf("\n");

	for(int i = 0; i < 8; i++)
	{
	
		if(i < 2)
		{
			printf("\t\u2551\t\u2551\n");
		}
		else if((currentGuess > 0))
		{
			printf("\t\u2551\t");
		}
		else
		{
			printf("\t\u2551\n");
		}
		if(i == 2 && currentGuess >= 1)
		{
			printf("\u039f\n");
			currentGuess--;
		}
		if(i == 3 && currentGuess == 3)
		{
			printf("\b/|\\\n");
			currentGuess -= 3;
		}
		else if(i == 3 && currentGuess >= 2)
		{
			printf("\b/|\n");
			currentGuess -= 2;
		}
		else if(i == 3 && currentGuess >= 1)
		{
			printf("|\n");
			currentGuess--;
		}
		if(i == 4 && currentGuess >= 1)
		{
			printf("\b/\n");
			currentGuess = 0;
		}
	}
}
