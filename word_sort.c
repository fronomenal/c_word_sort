#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "word_sort.h"

#define MAX_WORD 45

// BST for sorting and searching words
typedef struct WordTree { 
    char    *word; 
    int     count; 
    struct  WordTree *left;
    struct  WordTree *right;
} WordTree;

WordTree *get_word_tree( void );
void free_word_tree(WordTree *);
WordTree *add_word( WordTree *, char * );
void all_words( WordTree *);

unsigned int g_flags = 0;
char *g_dest = NULL;
unsigned int g_buffer_cap;


unsigned int word_sort( const char *src, char *dst, unsigned int dst_len, unsigned int flags )
{

    
    WordTree *tree_root = NULL;
    char word_builder[MAX_WORD], c;
    unsigned int char_count = 0, space_count = 0, index = 0;
    bool word_end = false;

    g_flags = flags;
    g_dest = dst;
    g_buffer_cap = -1;

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
            
            word_builder[index] = '\0';
            tree_root = add_word(tree_root, word_builder);

            char_count += 1;
            index = 0;
            word_end = false;
            continue;
        }

        src++;
        
    }

    if ( dst_len - 1 == space_count ) 
        dst[0] = '\n';

    if (char_count) {

        all_words(tree_root);
        dst[g_buffer_cap] = '\0';

        return char_count;

    }
    
    return 0;

}

// get WordTree instance
WordTree *get_word_tree(void)
{
    WordTree *wt = (WordTree *) malloc(sizeof(WordTree));
    wt->left = wt->right = NULL;
    return wt;

}

void free_word_tree(WordTree *t)
{   
    
    if(t == NULL)
        return;

    free_word_tree(t->left);
    free_word_tree(t->right);

    free(t);

    return;

}

WordTree *add_word(WordTree *t, char *w)
{
    int cond;
    char *word_copy;

    if (t == NULL) { //

        t = get_word_tree();

        word_copy = (char *) malloc(strlen(w)+1);
        if (word_copy)
            strcpy(word_copy, w);

        t->word = word_copy;
        t->count = 1;

    } else if (
        (cond = (g_flags == 2 || g_flags == 3)? 
        strcasecmp(w, t->word) : strcmp(w, t->word) ) == 0
    ) //word exists, inc count
        t->count++; 
    else if ( (g_flags == 1 || g_flags == 3)? !(cond < 0) : cond < 0) // insert word based on flag
        t->left = add_word(t->left, w);
    else // insert word base on flag
        t->right = add_word(t->right, w);

    return t;

}

// traverses tree and sets buffer to node words.
// Will free tree and allocated words
void all_words(WordTree *t)
{
    if (t != NULL) {

        all_words(t->left);

        for (int i = 0; i < t->count; i++){

            unsigned int word_size = strlen(t->word) + 1;

            memcpy(g_dest, t->word , word_size);
            g_dest[word_size - 1] = ' ';
            g_dest += word_size;
            g_buffer_cap += word_size;

        }

        free(t->word);

        all_words(t->right);

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


    struct test_suite test_flags[5];

    test_flags[0].flag = 0;
    strcpy( test_flags[0].with, " the \tcat sat is Scared \n" );
    strcpy( test_flags[0].want.srt_words, "Scared cat is sat the" );
    test_flags[0].want.count = 22;
    
    test_flags[1].flag = 1;
    strcpy( test_flags[1].with, " the \tcat sat is Scared \n" );
    strcpy( test_flags[1].want.srt_words, "the sat is cat Scared" );
    test_flags[1].want.count = 22;
    
    test_flags[2].flag = 2;
    strcpy( test_flags[2].with, "the    \tcat Sat   \n" );
    strcpy( test_flags[2].want.srt_words, "cat Sat the" );
    test_flags[2].want.count = 12;
    
    test_flags[3].flag = 2;
    strcpy( test_flags[3].with, " the  Mouse  \t\n cat Hippo sat" );
    strcpy( test_flags[3].want.srt_words, "cat Hippo Mouse sat the" );
    test_flags[3].want.count = 24;
    
    test_flags[4].flag = 3;
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
            "\nTestsuite %d test Sort got => '%s' %d want => '%s' %d: ", 
            i, got_buffer, test_sort[i].want.count,
            test_sort[i].want.srt_words, got_count 
            );
        assert( ( strcmp(test_sort[i].want.srt_words, got_buffer) == 0 ) );
        assert( test_sort[i].want.count == got_count );
        printf("Passed");

        
        buff_size = strlen(test_flags[i].with) + 1;
        char got_buffer_f[buff_size];
        got_count = word_sort(test_flags[i].with, got_buffer_f, buff_size, test_flags[i].flag);

        printf(
            "\nTestsuite %d test Flags %u got => '%s' %d want => '%s' %d: ", 
            i, test_flags[i].flag , got_buffer_f, test_flags[i].want.count,
            test_flags[i].want.srt_words, got_count 
            );
        assert( ( strcmp(test_flags[i].want.srt_words, got_buffer_f) == 0 ) );
        assert( test_flags[i].want.count == got_count );
        printf("Passed");
    }
    
    printf("\nTest returns 0 when buffer is less than stripped source: ");
    char test_lesser_buffer[] = "words must have less buffer";
    char lesser_buffer[10];
    got_count = word_sort(test_lesser_buffer, lesser_buffer, 10, 0);
    assert( 0 == got_count);
    printf("Passed");
    
    printf("\nTest all white space sets buffer to terminator: ");
    char test_space_src[] = "      \n    ";
    unsigned int buffer_size = strlen(test_space_src) + 1;
    char got_buffer[buffer_size];
    got_count = word_sort(test_space_src, got_buffer, buffer_size, 0);
    assert( 0 == got_count);
    assert( '\n' == got_buffer[0] );
    printf("Passed");

}

#endif