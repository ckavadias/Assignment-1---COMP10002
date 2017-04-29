/* Assignment 1, COMP10002, programming is fun, September 2015
 Constantinos Kavadias, ckavadias@student.unimelb.edu.au, 664790 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define CHARS 1001
#define TOP 10
#define DEBUG 0
#define DIVIDE 6
#define OFFSET 2

/*type definitions for character strings */
typedef char char_t[CHARS];

typedef struct{
	char_t line;
	int len;
	int fi;
	int fj;
}charray_t;

typedef struct{
	charray_t line;
	int num;
	double stage2_score;
	double stage4_score;
}line_t;


/*prints out stage outputs as defined by assignment*/
void output_124(line_t *line);

/* output for stage 3 so as not to interfere with other stages */
void stage3_output(line_t lines[], int len);

/* entry replacement when new addition to TOP scores is found */
void cascade_swap(line_t A[], int len);

/*records current line and its length*/
void fill_struct(line_t *line, char first, int *num_lines);

/* calculate and store stage 2 score of line*/
void stage2_score(line_t *line, charray_t S[], int num_subs);

/* calculate and store stage 4 score of line*/
void stage4_score(line_t *line, charray_t S[], int len);

/* tally number of occurences of substring in a line */
int sum_occurences(char* start, char* pattern, int pat_len);

/*case insensitive substring search, found at 
http://stackoverflow.com/questions/27303062/strstr-
function-like-that-ignores-upper-or-lower-case to act like strcasestr() for 
machines that dont have this function */
char* str_casein(const char *str, const char *strSearch);

/* find all substrings for query, return number of substrings */
int substring(char* pattern, charray_t S[], int offset);

/* insert substrings into an array of type charray */
void fill_string(int start, int end, int current,
	charray_t S[], char* pattern);

/* strip all non alphanumeric characters from string */
void alphanum_strip(char* original, char* new, int len);

/* quick sort aimed at array type charray_t  */
void qsort_charray(charray_t S[], int len);

void partition_len(charray_t S[], int len, int *first_e, 
					int *first_g, int pivot);

void entry_swap(charray_t *a, charray_t *b);

/* quick sort aimed at substring array, type line_t */
int is_sorted(line_t lines[], int len);

void qsort_score(line_t lines[], int len, int score);

void partition_score(line_t S[], int len, int *first_e, 
	int *first_g, double pivot);

void partition_num(line_t S[], int len, int *first_e, 
	int *first_g, int pivot);

void line_swap(line_t *a, line_t *b);

void line_sort_harness(line_t lines[], int len);

int
main(int argc, char *argv[]) {
	int num_subs, num_strip, i, sorted = 0, lines = 0, in_array = 0, pat_len,
		full =0;
	char *pattern, character;
	line_t newest, highest[TOP];
	pattern = argv[1];
	
	pat_len = strlen(pattern);
	char pattern_strip[pat_len];
	
    /* array size derived from number of ordered substrings 
    in array of known size as half of (size*(size +1)) */
	charray_t substrings[(pat_len*(pat_len + 1))/2];
	charray_t strip_subs[(pat_len*(pat_len + 1))/2];
	
	memset(newest.line.line, '\0', CHARS);
	
	/* created array of stripped substrings, record length and sort*/
	alphanum_strip(pattern, pattern_strip, pat_len);
	num_strip = substring(pattern_strip, strip_subs, OFFSET);
	qsort_charray(strip_subs, num_strip);
	
	/*create array of substrings, record length and sort */
	num_subs = substring(pattern, substrings, 0);
	qsort_charray(substrings, num_subs);
	
	printf("S0: query = %s\n", pattern);
	
	/*record new lines and their scores, deal with appropriately to find
	top ten (if ten) and maintain record*/
	while ((character = getchar()) != EOF){
		newest = (line_t){.stage2_score = 0.0, .stage4_score = 0.0};
		fill_struct(&newest, character, &lines);
		stage2_score(&newest, substrings, num_subs);
		stage4_score(&newest, strip_subs, num_strip);
		output_124(&newest);
		
		/*initially fill array for reference set*/
		if (in_array <TOP){
			if (newest.stage4_score != 0.0){
			 highest[in_array] = newest;
			 	if (DEBUG){
			 		printf( "CURRENT TOP\n");
			 		for (i = 0; i < in_array; i++){
			 			printf("Entry: %d, line: %d\n", i+1, highest[i].num);
			 		}
			 	}
			 in_array ++;
			}
		}
		/*Due to nature of comparison function, sorting is only
		required to be performed once, this saves room on the stack */
		if (in_array == TOP){
			sorted = is_sorted(highest, TOP);
			if (DEBUG){
			 		printf( "SORTED\n");
			 		for (i = 0; i < in_array; i++){
			 			printf("Entry: %d, line: %d\n", i+1, highest[i].num);
			 		}
			 	}
		}	
		if ( full && newest.stage4_score != 0.0){
			for ( i = 0; i < TOP; i++){
				if ((newest.stage4_score/highest[i].stage4_score) > 1){
					if(DEBUG){
						printf("NEW: %d, DELETE: %d\n", newest.num, 
							highest[TOP-1].num);
					}
					cascade_swap(highest, i);
					highest[i] = newest;
					if (DEBUG){
						printf("NEW TOP:\n");
						for (i = 0; i < TOP; i++){
							printf("Entry: %d, line: %d\n", i+1, 
								highest[i].num);
						}
					}
					break;
				}
			}
			
		}
		if (in_array == TOP){
			full = 1;
		}
		memset(newest.line.line, '\0',newest.line.len);
	}
	if (!sorted){
		is_sorted(highest, in_array);
	}
	stage3_output(highest, in_array );
	
	return 0;
}

/*records current line and its length*/
void
fill_struct(line_t *line, char first, int *num_lines){
	int  index = 0;
	char current;
	
	if(first != '\n'){
		line->line.line[index] = first;
		index++;
		
		while((current = getchar()) != '\n'){
			line->line.line[index] = current;
			index ++;
		}
	}
	else{
		line->line.line[index] = first;
	}
	line->line.len = index;
	(*num_lines)++;
	line->num = *num_lines;
}

/*prints out stage outputs as defined by assignment*/
void
output_124(line_t *line){
	if (line->line.len){
		printf("---\n");
		/* print out line of interest */
		printf( "%s\n", line->line.line);
		/* Stage 1 output */
		printf("S1: line %5d, bytes = %d\n", line->num, line->line.len);
		/*Stage 2 output */
		printf("S2: line %5d, score = %.3f\n", line->num, line->stage2_score);
		/*Stage 4 output */
		printf("S4: line %5d, score = %.3f\n", line->num, 
			line->stage4_score);
	}
}

/* output for stage 3 so as not to interfere with other stages */
void 
stage3_output(line_t lines[], int len){
	int i,j;
	for (i = 0; i<DIVIDE; i++){
		printf("--------");
	}
	printf("\n");
	
	for (j = 0; j < len; j++){
		printf("S3: line %5d, score = %.3f\n", lines[j].num, 
			lines[j].stage4_score);
		printf("%s\n---\n",lines[j].line.line);
	}
}

/* find all substrings for query, return number of substrings */
int
substring(char* pattern, charray_t S[], int offset){
	int i,j, current =0, len = strlen(pattern);
	
	for (i = 0; i < len; i++){
		for ( j= i + offset; j < len; j++){
			S[current].fi = i;
			S[current].fj = j;
			fill_string(i, j, current, S, pattern);
			current ++;
		}
		
	}
	return current;
}

/* quick sort aimed at substring array, type charray */
int 
is_sorted(line_t lines[], int len){
	
	qsort_score(lines, len, 1);
	line_sort_harness(lines, len);
	
	return 1;
}

void 
qsort_charray(charray_t S[], int len){
	int fe, fg, pivot;
	if (len <= 1){
		return;
	}
	
	pivot = S[len/2].len;
	partition_len(S, len, &fe, &fg, pivot);
	qsort_charray(S, fe);
	qsort_charray(S + fg, len-fg);
}

void 
partition_len(charray_t S[], int len, int *first_e, int *first_g, int pivot){
	int next, start, end;
	next = 0, start = 0, end = len;
	while (next < end){
		if (S[next].len < pivot){
			entry_swap(&S[next], &S[start]);
			start++;
			next++;
	}else if (S[next].len > pivot){
		end --;
		entry_swap(&S[next], &S[end]);
	}else{
		next++;
	}
  }
  *first_e = start;
  *first_g = end;
}

void 
entry_swap(charray_t *a, charray_t *b){
	charray_t temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

/* quick sort aimed at substring array, type line_t */
void 
qsort_score(line_t lines[], int len, int score){
	int fe, fg;
	if (len <= 1){
		return;
	}
	if (score){
		double pivot = lines[len/2].stage4_score;
		partition_score(lines, len, &fe, &fg, pivot);
		qsort_score(lines, fe, score);
		qsort_score(lines + fg, len-fg, score);
	}
	else{
		int pivot = lines[len/2].num;
		partition_num(lines, len, &fe, &fg, pivot);
		qsort_score(lines, fe, score);
		qsort_score(lines + fg, len-fg, score);
	}
}

void 
partition_score(line_t S[], int len, int *first_e, int *first_g, double pivot){
	int next, start, end;
	next = 0, start = 0, end = len;
	while (next < end){
		if ((S[next].stage4_score/pivot) > 1){
			line_swap(&S[next], &S[start]);
			start++;
			next++;
	}else if ((S[next].stage4_score/pivot) < 1){
		end --;
		line_swap(&S[next], &S[end]);
	}else{
		next++;
	}
  }
  *first_e = start;
  *first_g = end;
}

void
partition_num(line_t S[], int len, int *first_e, int *first_g, int pivot){
	int next, start, end;
	next = 0, start = 0, end = len;
	while (next < end){
		if (S[next].num < pivot){
			line_swap(&S[next], &S[start]);
			start++;
			next++;
	}else if (S[next].num > pivot){
		end --;
		line_swap(&S[next], &S[end]);
	}else{
		next++;
	}
  }
  *first_e = start;
  *first_g = end;
}

void 
line_swap(line_t *a, line_t *b){
	line_t temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

void
line_sort_harness( line_t lines[], int len){
	int i, previous = 0;
	double score = lines[0].stage4_score;
	
	/* find groups of equal scores in array and sort within these sub arrays
	with respect to their line numbers*/
	for ( i = 0; i < len; i++){
		if (lines[i].stage4_score != score){
			qsort_score(&lines[previous], (i -previous), 0);
			score = lines[i].stage4_score;
			previous = i;
		}
		if ( i == len -1){
			qsort_score(&lines[previous], ( len -previous), 0);
		}
	}
}

/* insert substrings into an array of type charray */
void
fill_string(int start, int end, int current, charray_t S[], char* pattern){
	int i, j = 0;
	
	for ( i = start; i <= end; i++){
		S[current].line[j] = pattern[i];
		j++;
	}
	S[current].line[j] = '\0';
	S[current].len = j;
	
	if (DEBUG){
		printf("#%d, len %d %s\n", current+1, S[current].len, S[current].line);
		printf("	i = %d, j = %d\n", S[current].fi, S[current].fj);
	}
}

/* calculate and store stage 2 score of line*/
void 
stage2_score(line_t *line, charray_t S[], int num_subs){
	int i, j;
	for (i = 0; i < line->line.len; i++){
		for (j = num_subs -1; j >= 0 ; j--){
			if (strstr(line->line.line, S[j].line)){
				line->stage2_score = (double)S[j].len;
				break;
			}
			else{
				line->stage2_score = 0.00;
			}
		}
	}	
}


/* entry replacement when new addition to TOP scores is found */
void 
cascade_swap(line_t A[], int end){
	int i;
	for (i = TOP-1; i > end; i--){
		A[i] = A[i-1];
	}
}

/* strip all non alphanumeric characters from string */
void
alphanum_strip(char* original, char* new, int len){
	int i, index = 0;
	for (i = 0; i < len; i++){
		if ( isalnum(original[i])){
				new[index] = original[i];
				index++;
		}
	}
	new[index] = '\0';
}

/* calculate and store stage 4 score of line*/
void 
stage4_score(line_t *line, charray_t S[], int len){
	double numerator = 0.0, sum, denominator = log2(30.0 + line->line.len);
	int occurences , i;
	
	for(i = 0; i < len; i++){
		occurences = sum_occurences(line->line.line, S[i].line, S[i].len);
		if (occurences){
			sum = (S[i].fj - S[i].fi - 1.0);
			numerator += ((sum*sum)*(log2(2.0 + occurences)));
			if (DEBUG){
				printf("Current score: %f\n", numerator/denominator);
			}
		}
		if (DEBUG){
			printf("PATTERN: %s, OCC#: %d\n", S[i].line, occurences);
		}
	}
	line->stage4_score = (numerator/denominator);
}

/* tally number of occurences of substring in a line */
int
sum_occurences(char* start, char* pattern, int pat_len){
	char *next = NULL;
	next = str_casein(start, pattern);
	if (next){
		return (1 + sum_occurences(next + pat_len, pattern, pat_len));
	}
	else{
		return 0;
	}
}

/*case insensitive substring search, found at 
http://stackoverflow.com/questions/27303062/strstr-
function-like-that-ignores-upper-or-lower-case to act like strcasestr() for 
machines that dont have this function */
char* 
str_casein(const char* haystack, const char* needle){
  do {
    const char* h = haystack;
    const char* n = needle;
    while (tolower((unsigned char) *h) == tolower((unsigned char ) *n) && *n){
      h++;
      n++;
    }
    if (*n == 0){
      return (char *) haystack;
    }
  } while (*haystack++);
  return 0;
}