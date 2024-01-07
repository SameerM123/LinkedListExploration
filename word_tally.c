#include <stdio.h>
#include <stdlib.h> // for malloc(), free()
#include <string.h>

#include "word_extractor.h"
#include "word_tally.h"

/**
 * A tool to hold the words we care about in an array, and
 * provide a method for processing the file.
 *
 */

// forward declarations
static int updateWordInTallyList(LLNode **wordListHeads, int maxLen, char *word){
  int wordLength = strlen(word);

    if (wordLength <= maxLen) {
        // select correct list
        LLNode *node = wordListHeads[wordLength];

        // search for the word 
        while (node != NULL) {
            if (strcmp(node->key, word) == 0) {
                // Word is already in list
                node->value++;
                return 1; 
            }
            node = node->next;
        }

        // create new node for newly occuring words
        LLNode *newNode = llNewNode(strdup(word), 1); //strdup to copy word to node
        if (newNode == NULL) {
            fprintf(stderr, "memory allocation failed for a new word node\n");
            return 0; 
        }

        //add new node
        wordListHeads[wordLength] = llPrepend(wordListHeads[wordLength], newNode);

        return 1; 
    }

    return 0; 

}

int tallyWordsInFile(char *filename, LLNode **wordLists, int maxLen)
{
	struct WordExtractor *wordExtractor = NULL;
	char *aWord = NULL;
	// add any more variables that you need


	// create the extractor and open the file
	wordExtractor = weCreateExtractor(filename, maxLen);

	if (wordExtractor == NULL) {
		fprintf(stderr, "failed creating extractor for file '%s'\n", filename);
		return 0;
	}

	/** TODO: ensure that all of the word list heads are NULL */
	//this is done in hapax_main.c

	int totalWordCount = 0;

	//add word to tally if seen before
	while (weHasMoreWords(wordExtractor)) {
		aWord = weGetNextWord(wordExtractor);
		totalWordCount++; 
		updateWordInTallyList(wordLists, maxLen, aWord);
	}

	
	printf("total word count %d\n", totalWordCount);

	// Close the file when we are done
	weDeleteExtractor(wordExtractor);

	return 1;
}

