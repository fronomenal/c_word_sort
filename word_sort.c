#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "word_sort.h"

#define MAX_WORD 46

// BST for sorting and searching words
typedef struct WordTree { 
    int     count; 
    struct  WordTree *left;
    struct  WordTree *right;
    char    word[]; 
} WordTree;

WordTree *get_word_tree(unsigned int);
void free_word_tree(WordTree *);
WordTree *add_word( WordTree *, char * , unsigned int);
void all_words( WordTree *, char* *);


unsigned int word_sort( const char *src, char *dst, unsigned int dst_len, unsigned int flags )
{

    
    WordTree *tree_root = NULL;
    char word_builder[MAX_WORD], c;
    unsigned int char_count = 0, space_count = 0, index = 0;
    bool word_end = false;

    char* dst_cpy = dst;
    char* *dst_ptr = &dst;

    // strip white spaces and get words for inserting into bst
    while( ( c = *src ) != '\0' ){

        //check to ensure stripped src text fits dst_len
        if ( char_count > dst_len ) { 
            free_word_tree(tree_root);
            return 0;
        }

        //ignore leading spaces if any
        if ( (char_count == 0) && isspace(c) ) { 

            src++;
            space_count++;
            while (isspace(*src))
            {
                src++;
                space_count++;
            }
            continue;

        //ignore consecutive space characters
        } else if ( isspace(c) && src[1] != '\0' ) { 
            word_end = true;
            src++;
            space_count++;
            continue;
        }

        if ( !word_end && !isspace(c) ) {
            word_builder[index++] = c;
            char_count += 1;
        }

        //current character is last of word
        if ( src[1] == '\0' ){ 

            word_end = true;
            if (isspace(c))
                space_count++;
            src++;

        }

        if (word_end) {
			
			if ( index >= MAX_WORD ) //truncate word if it's too long
				index = MAX_WORD - 1;
            
            word_builder[index] = '\0';
            tree_root = add_word(tree_root, word_builder, flags);
			if ( !tree_root ) { 
				free_word_tree(tree_root);
				return 0;
			}

            char_count += 1;
            index = 0;
            word_end = false;
            continue;
        }

        src++;
        
    }

    if (char_count) {

        all_words(tree_root, dst_ptr);
        dst_cpy[char_count-1] = '\0';

        return char_count;

    }
    
    dst[0] = '\n';
    return 0;

}

// get WordTree instance
WordTree *get_word_tree(unsigned int word_cap)
{
    WordTree *wt = (WordTree *) malloc(sizeof(WordTree) + sizeof(char[word_cap]));
	
	if(wt)
		wt->left = wt->right = NULL;
	
    return wt;

}

void free_word_tree(WordTree *t)
{   
    
    if(t != NULL){
		free_word_tree(t->left);
		free_word_tree(t->right);
		free(t);
	}

    return;

}

WordTree *add_word(WordTree *t, char *w, unsigned int flags)
{
    int cond;

    if (t == NULL) { //

        t = get_word_tree(strlen(w)+1);
        if (t){
            strcpy(t->word, w);
			t->count = 1;
		}

    } else if (
        (cond = (flags == 2 || flags == 3)? 
        strcasecmp(w, t->word) : strcmp(w, t->word) ) == 0
    ) //word exists, inc count
        t->count++; 
    else if ( (flags == 1 || flags == 3)? (cond > 0) : cond < 0) // insert word based on flag
        t->left = add_word(t->left, w, flags);
    else // insert word base on flag
        t->right = add_word(t->right, w, flags);

    return t;

}

// traverses tree and sets buffer to node words.
// Will free tree and allocated words
void all_words(WordTree *t, char* *dst_ptr)
{
    if (t != NULL) {

        all_words(t->left, dst_ptr);

        for (int i = 0; i < t->count; i++){

            unsigned int word_size = strlen(t->word) + 1;

            strcpy(*dst_ptr, t->word);
            (*dst_ptr)[word_size - 1] = ' ';
            *dst_ptr += word_size;

        }

        all_words(t->right, dst_ptr);

        free(t);

    }

    return;
}

#ifdef RUN_TEST

int main(){
    
    
    struct test_suite {
        char with[50];
        unsigned int flag;
        struct{char srt_words[50]; unsigned int count; } want; 
    };

    struct test_suite test_sort[5];
    
    strcpy( test_sort[0].with, " the \tcat sat \n" );
    strcpy( test_sort[0].want.srt_words, "cat sat the" );
    test_sort[0].want.count = 12;
    
    strcpy( test_sort[1].with, " The sat \n" );
    strcpy( test_sort[1].want.srt_words, "The sat" );
    test_sort[1].want.count = 8;
    
    strcpy( test_sort[2].with, "the    \tcat Sit   \n" );
    strcpy( test_sort[2].want.srt_words, "Sit cat the" );
    test_sort[2].want.count = 12;
    
    strcpy( test_sort[3].with, "the  mouse cat sat" );
    strcpy( test_sort[3].want.srt_words, "cat mouse sat the" );
    test_sort[3].want.count = 18;
    
    strcpy( test_sort[4].with, " the \tcat sat in another cat\n" );
    strcpy( test_sort[4].want.srt_words, "another cat cat in sat the" );
    test_sort[4].want.count = 27;


    char* *flags = calloc(4, sizeof(char *));
    flags[0] = "No Flag";
    flags[1] = "Reversed Flag";
    flags[2] = "Case-Insensitive Flag";
    flags[3] = "Reversed & Case-Insensitive Flags";


    struct test_suite test_flags[5];
    strcpy( test_flags[0].with, " the \tcat sat is Scared \n" );
    strcpy( test_flags[0].want.srt_words, "Scared cat is sat the" );
    test_flags[0].want.count = 22;
    
    test_flags[1].flag = WORD_SORT_REVERSE;
    strcpy( test_flags[1].with, " the \tcat sat is Scared \n" );
    strcpy( test_flags[1].want.srt_words, "the sat is cat Scared" );
    test_flags[1].want.count = 22;
    
    test_flags[2].flag = WORD_SORT_IGNORE_CASE;
    strcpy( test_flags[2].with, "the    \tcat Sat   \n" );
    strcpy( test_flags[2].want.srt_words, "cat Sat the" );
    test_flags[2].want.count = 12;
    
    test_flags[3].flag = WORD_SORT_IGNORE_CASE;
    strcpy( test_flags[3].with, " the  Mouse  \t\n cat Hippo sat" );
    strcpy( test_flags[3].want.srt_words, "cat Hippo Mouse sat the" );
    test_flags[3].want.count = 24;
    
    test_flags[4].flag = WORD_SORT_REVERSE | WORD_SORT_IGNORE_CASE;
    strcpy( test_flags[4].with, " the \tcat sat in James another cat\n" );
    strcpy( test_flags[4].want.srt_words, "the sat James in cat cat another" );
    test_flags[4].want.count = 33;



    unsigned int buff_size;
    unsigned int got_count;
    
    for (int i = 0; i < 5; i++){

        buff_size = strlen(test_sort[i].with) + 1;
        char got_buffer[buff_size];
        got_count = word_sort(test_sort[i].with, got_buffer, buff_size, 0);

        printf(
            "\nTest-case No Flag with: '%s' \nGOT => '%s' write_count=%d \nWANT => '%s' write_count=%d", 
            test_sort[i].with, got_buffer, test_sort[i].want.count,
            test_sort[i].want.srt_words, got_count 
            );
        assert( ( strcmp(test_sort[i].want.srt_words, got_buffer) == 0 ) );
        assert( test_sort[i].want.count == got_count );
        printf("\nPASSED\n\n");

        
        buff_size = strlen(test_flags[i].with) + 1;
        char got_buffer_f[buff_size];
        got_count = word_sort(test_flags[i].with, got_buffer_f, buff_size, test_flags[i].flag);

        printf(
            "\nTest-case %s with: '%s'\nGOT => '%s' write_count=%d \nWANT => '%s' write_count=%d", 
            flags[test_flags[i].flag], test_flags[i].with, got_buffer_f, test_flags[i].want.count,
            test_flags[i].want.srt_words, got_count 
            );
        assert( ( strcmp(test_flags[i].want.srt_words, got_buffer_f) == 0 ) );
        assert( test_flags[i].want.count == got_count );
        printf("\nPASSED\n\n");
    }
    
    printf("\nTest returns 0 when buffer is less than stripped source");
    char test_lesser_buffer[] = "words must have less buffer";
    char lesser_buffer[10];
    got_count = word_sort(test_lesser_buffer, lesser_buffer, 10, 0);
    assert( 0 == got_count);
    printf("\nPASSED\n");
    
    printf("\nTest all white space sets buffer to terminator");
    char test_space_src[] = "      \n    ";
    unsigned int buffer_size = strlen(test_space_src) + 1;
    char got_buffer[buffer_size];
    got_count = word_sort(test_space_src, got_buffer, buffer_size, 0);
    assert( 0 == got_count);
    assert( '\n' == got_buffer[0] );
    printf("\nPASSED\n");

    free(flags);
}

#endif