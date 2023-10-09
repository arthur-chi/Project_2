// Name: Arturo Rivera
// UIN: 674381566
// CS 211 - Program III - Hangman Game
// Professor: Ellen Kidane
// UIC - Fall 2023

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

typedef struct Pattern_struct {
    char* pat;  //string pattern - exs: "hello", "-ell-", "-----"
    int count;  //frequency of pattern in the current word list
    int changes;//number of occurences of new letter in this pattern
} Pattern;

// frees malloced space for each word in array and the array itself
void freeWords( char** *words, int *maxSize) {
    for ( int i = 0; i < *maxSize; i++ ) {
        free ( (*words)[i] );
    } 
    free ( *words );
}

// copies words, doubles word array size and points original array to new array
void copyWords( char*** words, char*** newWords, int currSize, int *maxSize  ) {   
    for (int i = 0; i < currSize; i++) { strcpy( (*newWords)[i], (*words)[i] ); }  
    freeWords( words, maxSize );
    *maxSize = *maxSize * 2;
    *words = *newWords; 
}

// adds a string into string array, increments array size by 1, and copies words into bigger array if needed
void addWord(char*** words, int* numWords, int* maxWords, char* newWord ) {
    if ( *numWords == *maxWords ) {
        char** newWords = malloc( sizeof(char*) * (*maxWords * 2 ) );
        for ( int i = 0; i < *maxWords * 2; i++) { // malloc space for each word in new bigger array 
            if ( i < *numWords ) {
                newWords[i] = malloc(sizeof(char) * (strlen( (*words)[i] ) + 1 ) );
            } else {
                newWords[i] = malloc( sizeof(char) * 30 );
            }
        } 
        copyWords( words, &newWords, *numWords, maxWords ); 
    }
    // printf("All words copied\n");
    strcpy( (*words)[*numWords], newWord );
    *numWords = *numWords + 1;
}

int getIndexInDict( char** words, int numWords, char* wordToFind, int wordSize ) {
    for ( int i = 0; i < numWords; i++ ) {
        words[i][wordSize] = '\0';
        wordToFind[wordSize] = '\0';
        if ( strcmp( words[i], wordToFind ) == 0 ) { return i; }
    }
    return -1;
}

void copyAndDelete( char*** words, int* numWords, char* wordToDelete, int wordSize ) {
    char** newWords = malloc( (*numWords) * sizeof( char* ) );
    int idx = getIndexInDict( *words, *numWords, wordToDelete, wordSize );
    for ( int i = 0; i < *numWords; i++ ) {
        if ( i < idx ) {
            newWords[i] = (char*) malloc( wordSize * sizeof( char ) );
            strcpy( newWords[i], (*words)[i] );
            // free( (*words)[i] );
        } else if ( ( i == idx ) && ( strcmp( (*words)[i], wordToDelete ) == 0 ) ) {
            { free( (*words)[i] ); continue; }
        } else if ( ( i == idx ) && ( strcmp( (*words)[i], wordToDelete ) != 0 ) ) {
            newWords[i] = (char*) malloc( wordSize * sizeof( char ) );
            strcpy( newWords[i], (*words)[i] );
            // free( (*words)[i] );
        } else {
            newWords[ i - 1 ] = (char*) malloc( wordSize * sizeof( char ) );
            strcpy( newWords[ i - 1 ], (*words)[i] );
            // free( (*words)[i] );
        }
    }   
    *numWords = *numWords - 1;
    *words = newWords;
}

int strNumMods(char* str1, char* str2) {
    int count = 0;
    int x = strlen( str1 ); int y = strlen( str2 );

    if ( x > y ) {
        for ( int i = 0; i < x; i++ ) {
            if ( str1[i] != str2[i] ) { count = count + 1;}
        }
    } else if ( x < y ) {
        for ( int i = 0; i < y; i++ ) {
            if ( str1[i] != str2[i] ) { count = count + 1;}
        }
    } else {
        for ( int i = 0; i < x; i++ ) {
            if ( str1[i] != str2[i] ) { count = count + 1;}
        }
    }
    return count; //modify this
}

int strDiffInd(char* str1, char* str2) {
    for ( int i = 0; i < strlen( str1 ); i++ ) {
        if ( str1[i] != str2[i] ) {
            return i;
        }
    }
    return strlen( str1 ); //modify this
}

int findArgIdx( char ltr, int argc, char* argv[] ) {
    int idx = -1;
    for ( int i = 0; i < argc; i++ ) {
        if ( argv[i][1] == ltr ) {
            idx = i;
        }
    }
    return idx;
}

int findLtr( char ltr, char* list ) {
    int i = -1; 
    for ( int i = 0; i < strlen( list ); i++ ) {
        if ( list[i] == ltr ) { return i; }
    }
    return i;
}

void printMode( int x ) {
    if ( x == 0 ) { printf("OFF\n"); }
    else if ( x == 1 ) { printf("ON\n"); }
}

bool checkWrongs( char* list, char* word, int wordSize ) {
    for ( int i = 0; i < wordSize; i++ ) {
        for ( int j = 0; j < strlen( list ); j++ ) {
            if ( word[i] == list[j] ) {
                return true;
            }
        }
    }
    return false;
}

// This function replicates word into newWord, except all letters in word == ltr are shown, and all other letters are '-'
void makePattern( char ltr, char* word, char* newPat, int wordSize ) {
    for ( int i = 0; i < wordSize; i++ ) {
        if ( word[i] == ltr ) { newPat[i] = ltr; }
        else { newPat[i] = '-'; }
    }   
    word[wordSize] = '\0';          // for some reason, their last character ( not actually part of the word ) is different
    newPat[wordSize] = '\0';       // fixed it to compare accurately
} 

// This function copies word into newPat for all letters in word == pat
// We use this to add new patterns
void makeComparablePattern( char* newPat, char* pat, int wordSize ) {
    for ( int i = 0; i < wordSize; i++ ) { 
        if ( newPat[i] != pat[i] ) { newPat[i] = '-'; }
            // newPat[i] = word[i];
            
    }
    newPat[wordSize] = '\0';
}

void freePatterns( Pattern** patterns, int *patternSize ) {
    printf("-- Freeing Pattern Array --\n");
    for ( int i = 0; i < *patternSize; i++ ) {
        if ( patterns[i] != NULL ) {
            if ( patterns[i]->pat != NULL ) {
                free( patterns[i]->pat );
            }
            free( patterns[i] ); 
        }
    }
    printf("Freed all patterns...\n");
    *patternSize = 0;
}

void copyPatterns( Pattern** patterns, int* patternSize, int* patternCap, int wordSize ) {
    int newSize = *patternSize * 2;
    // printf("Copying patterns\n");
    // for ( int i = 0; i < *patternSize; i++ ) {
    //   printf("%d: %s\n", i, patterns[i]->pat );
    // }
  
    Pattern** newPatternList = ( Pattern** ) malloc( newSize * sizeof( Pattern* ) );
    for ( int i = 0; i < newSize; i++ ) {
        newPatternList[i] = ( Pattern* ) malloc( sizeof( Pattern ) );
        newPatternList[i]->pat = malloc( wordSize * sizeof( char ) );
        if ( i < *patternSize ) {
            strcpy( newPatternList[i]->pat, patterns[i]->pat );
            newPatternList[i]->count = (patterns)[i]->count;
            newPatternList[i]->changes = (patterns)[i]->count;
            // printf("%d: Pattern copied...\n", i);
        }
    } 
    // printf("All patterns copied...\n");
    // freePatterns( patterns, *patternSize );
    *patterns = *newPatternList;
    *patternCap = newSize;
}

bool findWord( char** words, int numWords, char* word ) {
  for ( int i = 0; i < numWords; i++ ) {
    if ( strcmp( word, words[i] ) == 0 ) {
      return 1;
    }
  }
  return 0;
}

// This function iterates through array of patterns to find if each of the patterns match the word
int findPattern( Pattern** list, int patternSize, char* word, int wordSize, char ltr ) {
    
    if ( patternSize == 0 ) { return -1; }  // don't bother checking for any patterns at all if there are none
    char* newWord = (char*) malloc( sizeof(char) * wordSize + 1 );
    makePattern( ltr, word, newWord, wordSize );
    
    for ( int i = 0; i < patternSize; i++ ) {
        if ( strcmp( newWord, list[i]->pat ) == 0 ) {
            free( newWord );
            return i;
        }
    }
    // printf("New Pattern: %s\n", newWord );
    free( newWord );
    return -1;
}

Pattern* createPattern( char* word, int numMods, int patCount, int size ) {
    Pattern* newPat =  malloc( sizeof( Pattern ) );
    newPat->pat = (char*) malloc( size * sizeof( char ) );
    strcpy( newPat->pat, word );
    newPat->changes = numMods;
    newPat->count = patCount;
    return newPat;
}

void addPatterns( char*** words, int wordSize, int numWords, char ltr, Pattern** patterns, int* patternSize, int* patternCap, Pattern** nextPat, char* guessList ) {
    if ( *patternSize > 0 ) { freePatterns( patterns, patternSize ); }
    int count = 0;
    for ( int i = 0; i < numWords; i++ ) {        // replace 30 with numWords when done testing
        if ( checkWrongs( guessList, (*words)[i], wordSize ) ) {
            // printf("Has letter in previous guesses\n");
            continue;
        }
        int patIdx = findPattern( patterns, *patternSize, (*words)[i], wordSize, ltr );
        if ( *patternSize == 16 ) { break; }
        if ( patIdx == -1 ) {
            // printf("New Pat\n");
            count = count + 1;
            if ( *patternSize == *patternCap ) {
                printf("%d: Copying Patterns\n", *patternSize );
                char* PatThree = (char*) malloc( wordSize * sizeof(char) );
                if ( *patternSize == 8 ) {
                  strcpy( PatThree, patterns[3]->pat );
                  printf("Three: %s\n", PatThree );
                  copyPatterns( patterns, patternSize, patternCap, wordSize );
                  strcpy( patterns[3]->pat, PatThree );
                } else {
                  copyPatterns( patterns, patternSize, patternCap, wordSize );
                  // printf("Pat after Copied: %s\n", patterns[3]->pat );
                }
            } 
            Pattern* newPat = ( Pattern* ) malloc( sizeof( Pattern ) );
            char* newWord = (char*) malloc( ( wordSize + 1 ) * sizeof( char ) );
            makePattern( ltr, (*words)[i], newWord, wordSize );
            int changes = wordSize - strNumMods( (*words)[i], newWord );
            newPat = createPattern( newWord, changes, 1, wordSize );
            // printf("%d | Pat after Copied: %s\n", *patternSize, patterns[3]->pat );
            patterns[ *patternSize ] = newPat;

            if ( *patternSize == 7 ) {
              for ( int i = 0; i < *patternSize; i++ ) {
                printf("%d: %s\n", i, patterns[i]->pat );
              }
            }
          
            *patternSize = *patternSize + 1;
        } else if ( patIdx != -1 ) {   
            patterns[ patIdx ]->count = patterns[ patIdx ]->count + 1;
        }
    }
    // for ( int i = 0; i < wordSize; i++ ) {
    //   printf("3: %c | 7: %c\n", patterns[3]->pat[i], patterns[7]->pat[i] );
    // }
  
      // printf("Pattern 4: %s\n", patterns[7]->pat );
      // printf("len: %ld | len: %ld\n", strlen( patterns[3]->pat ), strlen( patterns[7]->pat));
      // if ( strlen( patterns[3]->pat ) == 4)

    // gets index of pattern with greatest count
    int max = 0;
    int* matchingPatterns = (int*) malloc( *patternSize * sizeof( int ) );
    int matchCount = 0;
    for ( int i = 0; i < *patternSize; i++ ) {      
        if ( max < patterns[i]->count ) {
            if ( matchCount == 0 ) {
                max = patterns[i]->count;
                matchingPatterns[matchCount] = i;
            } else if ( matchCount > 0 ) {
                for ( int j = 0; j < matchCount; j++ ) {
                    free( matchingPatterns );
                }
                max = patterns[i]->count;
                matchingPatterns[matchCount] = i;
            }
        } else if ( max == patterns[i]->count ) {
            matchingPatterns[ matchCount ] = i;
        }
    }

    // If more than one pattern with highest count, get pattern with minimum change
    int minChange = patterns[ matchingPatterns[0] ]->changes;
    int* patternsWithEqualChanges = (int*) malloc( matchCount * sizeof(int) );
    int numEqualChangePatterns = 0;
    for ( int i = 0; i < matchCount; i++ ) {
        if ( minChange < patterns[ matchingPatterns[i] ]->changes ) {
            if ( numEqualChangePatterns == 0 ) {
                minChange = patterns[ matchingPatterns[i] ]->changes;
                patternsWithEqualChanges[ numEqualChangePatterns ] = i;
            } else if ( numEqualChangePatterns > 0 ) {
                for ( int j = 0; j < numEqualChangePatterns; j++ ) {
                    free( patternsWithEqualChanges );
                }
                minChange = patterns[ matchingPatterns[i] ]->changes;
                patternsWithEqualChanges[ numEqualChangePatterns ] = i;
            }
        } else if ( minChange == patterns[ matchingPatterns[i] ]->changes ) {
            patternsWithEqualChanges[ numEqualChangePatterns ] = i;
        }
    }
    int nextPatIdx = patternsWithEqualChanges[0];
    Pattern* newPat = ( Pattern* ) malloc( sizeof( Pattern ) );
    newPat = createPattern( patterns[ nextPatIdx ]->pat, patterns[nextPatIdx]->changes, patterns[nextPatIdx]->count, wordSize );
    strcpy( (*nextPat)->pat, newPat->pat );
    (*nextPat)->changes = newPat->changes;
    (*nextPat)->count = newPat->count;
}

void updateWords ( char*** words, int* numWords, char* pat, int wordSize, char ltr, char* guessList ) {
    for ( int i = 0; i < *numWords; i++ ) {
        if ( checkWrongs( guessList, (*words)[i], wordSize ) ) {
            memset( (*words)[i], 0, (wordSize * sizeof(char) ) );
            *numWords = *numWords - 1;
        }
        char* newWord = (char*) malloc( ( wordSize ) * sizeof( char ) );
        makePattern( ltr, (*words)[i], newWord, wordSize );
        makeComparablePattern( newWord, pat, wordSize );
        
        if ( ( strcmp( newWord, pat ) != 0 ) ) {    // copies all words into a new array 
            memset( (*words)[i], 0, (wordSize * sizeof(char) ) );
            *numWords = *numWords - 1;
        } 
        // check words to see if they have all previous correct guesses
        else {  continue; }
        // printf("-------------------\n");
    }
}

int main(int argc, char* argv[]) {
    printf("Welcome to the (Evil) Word Guessing Game!\n\n");
    bool solved = false;

    int size, guesses, statsMode, wordMode, letterMode, patternMode, verboseMode, extensionMode;
    int guessCount = 0; int wrongGuessCount = 0; int correctGuessCount = 0;
    if ( argc == 1 ) {
        size = 5; guesses = 26; // CHANGE SIZE TO 5 AFTER TESTING
        statsMode = 0; letterMode = 0; patternMode = 0; verboseMode = 0; extensionMode = 0; wordMode = 0;
    } else if ( argc > 1 ) {
        size = 5; guesses = 26;
        statsMode = 0; letterMode = 0; wordMode = 0; patternMode = 0; verboseMode = 0; extensionMode = 0;

        int sizeIdx = findArgIdx ( 'n', argc, argv );
        size = atoi( argv[ sizeIdx + 1 ] );
        
        int wordIdx = findArgIdx( 'w', argc, argv );
        if ( wordIdx != -1 ) { wordMode = 1; }

        int guessesIdx = findArgIdx( 'g', argc, argv );
        if ( guessesIdx != -1 ) { guesses = atoi( argv[ guessesIdx + 1 ] ); }

        int statsIdx = findArgIdx( 's', argc, argv );
        if ( statsIdx != -1 ) { statsMode = 1; }

        int letterIdx = findArgIdx( 'l', argc, argv );
        if ( letterIdx != -1 ) { letterMode = 1; }

        int patternIdx = findArgIdx( 'p', argc, argv );
        if ( patternIdx != -1 ) { patternMode = 1; }

        int verboseIdx = findArgIdx( 'v', argc, argv );
        if ( verboseIdx != -1 ) { 
            verboseMode = 1; patternMode = 1; size = 5;
            letterMode = 1; statsMode = 1; wordMode = 1; 
        }

        int extensionIdx = findArgIdx( 'e', argc, argv );
        if ( extensionIdx != 0 ) { extensionMode = 1; }
    }
    size = 6;
    printf("Game Settings:\n");
    printf("  Word Size = %d\n", size );
    printf("  Number of Guesses = %d\n", guesses );
    printf("  View Stats Mode = " ); printMode( statsMode );
    printf("  View Word List Mode = "); printMode( wordMode );
    printf("  View Letter List Mode = "); printMode( letterMode );
    printf("  View Pattern List Mode = "); printMode( patternMode );
    printf("\n");
    int capacity = 4; int currSize = 0; 
    int count = 0; int fitCount = 0;
    char** wordList = (char**) malloc( capacity * sizeof(char*) );
    char filename[] = "dictionary.txt";
    char input[81]; 
    char longestWord[30]; int longestWordSize = 0;

    FILE *inFS = fopen( filename, "r" );  // File opening and opening verification
    if ( inFS == NULL ) {
        printf("Error: could not open %s for reading\n", filename);
        exit(-1);
    }

    for ( int i = 0; i < capacity; i++ ) { 
        wordList[i] = malloc( sizeof(char) * ( size + 1 ) );
    } 
    // printf("Initial space for words mallocated\n");

    while ( fscanf( inFS, "%s", input ) != EOF ) {      // While loop to read in all words and store in string array
        count = count + 1; 
        if ( strlen( input ) == size ) {                // size verification to add correct-sized words
           addWord( &wordList, &currSize, &capacity, input );
        }
        if ( strlen( input ) > longestWordSize ) {      // keeps track of longest word
            strcpy( longestWord, input );
            longestWordSize = strlen( input );
        }
    }
    // printf("Total: %d\nTotal of size %d: %d\n", count, size, currSize );

    if ( statsMode != 0 ) {
        printf("The  dictionary contains %d words total.\n", count );
        printf("The longest word %s has %d chars.\n", longestWord, longestWordSize ); // Fix this
        printf("The dictionary contains %d words of length %d.\n", currSize, size );
        printf("Max size of the dynamic words array is %d.\n", capacity );
    }

    Pattern* nextPat = (Pattern*) malloc( sizeof(Pattern) );
    nextPat->pat = (char*) malloc( size * sizeof( char ) );

    Pattern** patterns; int patternSize = 0; int patternCap = 4; 
    patterns = (Pattern**) malloc( patternCap * sizeof( Pattern* ) );
    for ( int i = 0; i < patternCap; i++ ) {
        patterns[i] = (Pattern*) malloc( 1 * sizeof( Pattern ) );
    }

    char ltr = 'a';
    char* guessList = (char*) malloc( guesses * sizeof( char ) );
    char* wrongGuesses = (char*) malloc( guesses * sizeof( char ) );
    // for ( int i = 0; i < size; i++ ) {
    //     guessList[i] = NULL;
    //     wrongGuesses[i] = NULL;
    // }
    printf("Guess a letter (# to end game): ");
    scanf(" %c", &ltr );
    while ( ( ltr != '#' ) || ( guesses == 0 ) ) {
        if ( !islower( ltr ) ) {
            printf("\nInvalid letter...\n");
            printf("Guess a letter (# to end game): ");
            scanf("%c", &ltr );
            continue;
        } else {
            int nextPatIdx = -1;
            printf("Number of guesses remaining: %d\n", guesses ); 
            if ( letterMode == 1 ) {
              printf("Previously guessed letters:\n");
              for ( int i = 0; i < guessCount; i++ ) {
                printf(" %c", guessList[i] );
              }
            }
            if ( wordMode == 1 ) {
                printf("Words of length %d:\n", size );
                for ( int i = 0; i < 20; i++ ) {
                    printf("  %s\n", wordList[i] );
                }
            }
            addPatterns( &wordList, size, currSize, ltr, patterns, &patternSize, &patternCap, &nextPat, wrongGuesses );
            patternMode = 1;
            if ( patternMode == 1 ) {
                printf("All patterns for letter %c:\n", ltr);
                for ( int i = 0; i < patternSize; i++ ) {
                    char patCount[20];
                    sprintf( patCount, "%d", patterns[i]->count ); // converts int count to a string to get strlen of count for this vvvv
                    int spaceLen = 8 - strlen( patCount );  // prints correct amount of spaces so patternList prints aligned

                    if ( patterns[i]->pat[ size ] == '\0' ) {
                        printf("  %s        count = %d", patterns[i]->pat, patterns[i]->count );
                    } else {
                      printf("  %s       count = %d", patterns[i]->pat, patterns[i]->count );
                    }
                    for ( int i = 0; i < spaceLen; i++ ) { printf(" "); }
                    printf("changes = %d\n", patterns[i]->changes );
                }
            }


            int numLetters = nextPat->changes;
            if ( numLetters == 0 ) {
                printf("Oops, there are no %c's. You used a guess.\n", ltr ); 
                wrongGuesses[ wrongGuessCount ] = ltr;   
                wrongGuessCount =  wrongGuessCount + 1; 
            }
            else {
                printf("Good guess! The word has at least one %c.\n", ltr );
                guessList[ correctGuessCount ] = ltr;
                correctGuessCount = correctGuessCount + 1;
            }
            guessList[ guessCount ] = ltr;

            updateWords( &wordList, &currSize, nextPat->pat, size, ltr, wrongGuesses );
            printf("Number of possible words remaining: %d\n", currSize );
            printf("The word pattern is: %s\n", nextPat->pat );
            scanf(" %c", &ltr );
            
            guesses = guesses - 1;
            guessCount = guessCount + 1;
        }
        if ( guesses == 0 ) {
            printf("\nYou ran out of guesses and did not solve the word.\n");
            printf("The word is: %s\n", wordList[0] );
            printf("Game over.\n");
        } else if ( ltr == '#' ) {
            printf("\nTerminating game\n");
        }
    }

    
    //-------------------------------------------------------------------
    // TODO - Task VIII:free all heap-allocated memory to avoid potential 
    //                  memory leaks. Since the word size for each game 
    //                  play is a command-line argument, any array whose 
    //                  size depends on the word size should be 
    //                  dynamically heap-allocated, and thus must be 
    //                  tracked and freed
    //-------------------------------------------------------------------
    

    
    return 0;
}
