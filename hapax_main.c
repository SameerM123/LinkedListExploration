#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "word_tally.h"

#define	MAX_WORD_LEN	24

/** function to be passed in to llFree to delete allocated keys */
void deleteKey(LLNode *node, void *userdata)
{
	if (node->key != NULL) {
		free(node->key);
	}
}


/** Delete the contents of all of the word lists
 *
 * This function assumes that both the nodes and
 * the keys within the nodes have been allocated
 * using malloc() and need to be freed.
 */
void deleteWordLists(LLNode **wordListHeads, int maxLen){
	int i;

	for (i = 0; i <+ maxLen; i++) {
		if (wordListHeads[i] != NULL) {
			llFree(wordListHeads[i], deleteKey, NULL);
			wordListHeads[i] = NULL;
		}
	}
}


/** print out all of the data in a word list */
int printData(char *filename, LLNode *wordListHeads[], int maxLen){
	LLNode *node;
	int i;

	printf("All word count data from file '%s':\n", filename);

	/**
	 * For each length, if the list is not null, print out
	 * the values in the list
	 */
	for (i = 0; i <= maxLen; i++) {
		node = wordListHeads[i];
		if (node != NULL) {
			printf("Length %d:\n", i);
			while (node != NULL) {
				printf("    '%s' %d\n", node->key, node->value);
                node = node->next;
			}
		}
	}
	return 1;
}

int printHapax(char *filename, LLNode *wordListHeads[], int maxLen, int hapaxLength) {

	//initialze array of all zeros to store count of hapax words
    int hapaxCount[MAX_WORD_LEN + 1] = {0};

    if (hapaxLength > 0 && hapaxLength <= maxLen) {  //user specefied hapaxLength
        LLNode *node = wordListHeads[hapaxLength];
        while (node != NULL) {
            if (node->value == 1) {  //ie hapax legomenon
                printf("Hapax legomenon (length %d) from %s: '%s'\n", hapaxLength, filename, node->key);
                hapaxCount[hapaxLength]++;
            }
            node = node->next;  //continue through linked list
        }

        if (hapaxCount[hapaxLength] == 0) {  //if no hapax are found of specefied length
            printf("No hapax legomena of length %d found in file '%s'.\n", hapaxLength, filename);
        }
    } else {
        // user didn't specify length
        for (int i = 0; i <= maxLen; i++) {
            LLNode *node = wordListHeads[i];
            while (node != NULL) {
                if (node->value == 1) {
                    printf("Hapax legomenon (length %d) from %s: '%s'\n", i, filename, node->key);
                    hapaxCount[i]++;
                }
                node = node->next;
            }
        }
    }

    return 1;
}



/* print out the command line help */
void usage(){
	fprintf(stderr, "\n");
	fprintf(stderr, "Find and print the hapax legomena in one or more files.\n");
	fprintf(stderr, "A \"hapax legomenon\" is a word that occurs only once in the file\n");

	fprintf(stderr, "\n");
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "    hapax [<options>] <datafile> [ <datafile> ...]\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "-d     : print out all data loaded before printing hapax legomena.\n");
	fprintf(stderr, "-h     : this help.  You are looking at it.\n");
	fprintf(stderr, "-l <N> : only print hapax legomena of length <N>.\n");
	fprintf(stderr, "       : If no -l option is given, all hapax legomena are printed.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Sample command line:\n");
	fprintf(stderr, "    hapax -l5 smalldata.txt");
	fprintf(stderr, "\n");
	fprintf(stderr, "This example would print all words of length 5 that exist in the\n");
	fprintf(stderr, "file \"smalldata.txt\".\n");
	fprintf(stderr, "\n");

	exit(1);
}


int main(int argc, char **argv){
	int i, shouldPrintData = 0, didProcessing = 0, printHapaxLength = -1;

	//allocate an array of list heads of required size
   LLNode **wordListHeads = (LLNode **)malloc((MAX_WORD_LEN + 1) * sizeof(LLNode *)); 
    if (wordListHeads == NULL) {
        fprintf(stderr, "memory allocation failed\n");
        return 1;
    }


	//list heads become NULL
	for (i = 0; i <= MAX_WORD_LEN; i++) {
        wordListHeads[i] = NULL;
    }


for (i = 1; i < argc; i++) {     //command line handler
        if (!strcmp(argv[i], "-d")) {
            shouldPrintData = 1;
        }
        else if (!strcmp(argv[i], "-h")) {
            usage();
        }
        else if (!strcmp(argv[i], "-l")) {
				if (i + 1 < argc) {           //check if next argument (the number) exists
					printHapaxLength = atoi(argv[i + 1]);
					if (printHapaxLength <= 0) {
						fprintf(stderr, "invalid argument for -l\n");
						usage();
					}
					i = i + 1; 
				} else {
					fprintf(stderr, "missing argument for -l\n");  //error handling if user puts -l on its own without a number
					usage();
				}
        }
		else {
			if (tallyWordsInFile(argv[i], wordListHeads, MAX_WORD_LEN) == 0) {
			fprintf(stderr, "processing '%s' failed -- exitting\n", argv[i]);
			return 1;
			}

			didProcessing = 1;

			printf("Tally loaded\n");

			/**
			 * conditionally print out all the words loaded, based
			 * on the command line option
			 */
			if (shouldPrintData) {
			printData(argv[i], wordListHeads, MAX_WORD_LEN);
			}
			else{
			printHapax(argv[i], wordListHeads, MAX_WORD_LEN, printHapaxLength);
			}

			//memory cleanup
			for (i = 0; i <= MAX_WORD_LEN; i++) {
				if (wordListHeads[i] != NULL) {
					llFree(wordListHeads[i], deleteKey, NULL);
					wordListHeads[i] = NULL;
				}
			}
			
			free(wordListHeads);
		}
	}


	//error presented if didProcessing does not change
	if ( ! didProcessing ) {
		fprintf(stderr, "No data processed -- provide the name of"
				" a file on the command line\n");
		usage();
		return 1;
	}


	return 0;
}
