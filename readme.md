# Word Sort
C function that sorts words within a string.

## Language
Written in: 
* c

### Function Description
Takes in a space separated string of words with a buffer and a flag value; sorts the words in the string into the provided buffer in lexical order.
The buffer is truncated to a single delimiter if the string consists of only spaces.

#### Arguments:
char* src: string containing words to sort.
char* dst: destination buffer to store sorted words.
unsigned int dst_len: length of the destination buffer.
unsigned int flags: flag to alter sorting behavior

#### Flags:
WORD_SORT_REVERSE       0x1
WORD_SORT_IGNORE_CASE   0x2

#### Return Value:
Returns the number of characters written into the buffer including the terminator if successful or 0 otherwise.

### Test
run `make test` in the project directory
