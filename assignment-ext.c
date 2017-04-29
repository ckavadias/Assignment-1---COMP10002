
/* quick sort aimed at suffix array */
void qsort_str(char* A[], int len);
void partition(char** A, int len, int *first_e, int *first_g, char* pivot);
void pointer_swap(char *a, char *b);

/* insert substrings into an array of type charray */
void fill_string(int start, int end, int current,
	charray_t S[], char* pattern);

/* quick sort aimed at array type charray_t or line_t */
void qsort_charray(charray_t S[], line_t A[], int len, int score);

void partition_len(charray_t S[], int len, int *first_e, 
					int *first_g, int pivot);

void partition_score(line_t S[], int len, int *first_e,
						int *first_g, double pivot);

void entry_swap(charray_t *a, charray_t *b);

void entry_swap_score(line_t *a, line_t *b);

/* quick sort aimed at suffix array */

void 
qsort_str(charray A[], int len){
	int fe, fg;
	char* pivot;
	if (len <= 1){
		return;
	}
	pivot = S[len/2].len;
	partition(A, len, &fe, &fg, pivot);
	qsort_str(A, fe);
	qsort_str(A+fg, len-fg);
}

void 
partition(charray_t S[], int len, int *first_e, int *first_g, int pivot){
	int next, start, end;
	next = 0, start = 0, end = len;
	while (next < end){
		if (S[next].line < pivot)){
			pointer_swap(S[next], S[start]);
			start++;
			next++;
	}else if (S[next] > S[pivot]){
		end --;
		pointer_swap(S[next], S[end]);
	}else{
		next++;
	}
  }
  *first_e = start;
  *first_g = end;
}

void 
pointer_swap(char *a, char *b){
	char* temp;
	temp = a;
	a = b;
	b = temp;
}


/* output for stage 3 so as not to interfere with other stages */
void 
stage3_output(line_t A[], int len){}

/* calculate and store stage 2 score of line, assuming array of substrings is
sorted*/
void 
stage2_score(line_t A[], charray_t S[], int subs, int len){
	int i, j;
	for (i = 0; i < len; i++){
		for (j = subs -1; j >= 0 ; j--){
			if (strstr(A[i].array.line, S[j].line)){
				A[i].stage2_score = (double)S[j].len;
				break;
			}
			else{
				A[i].stage2_score = 0.00;
			}
		}
	}
}
	
/* find all substrings for query */
int
substring(char* pattern, charray_t S[]){
	int i,j, current =0, len = strlen(pattern);
	
	for (i = 0; i < len; i++){
		for ( j= i; j < len; j++){
			fill_string(i, j, current, S, pattern);
			current ++;
		}
		
	}
	return current;
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
		printf(" Substring %d %s\n", current+1, S[current].line);
	}
}

/* quick sort aimed at substring array, type charray */
void 
qsort_charray(charray_t S[], line_t A[], int len, int score){
	int fe, fg;
	if (len <= 1){
		return;
	}
	if (score){
		double pivot;
		pivot = A[len/2].stage2_score;
		partition_score(A, len, &fe, &fg, pivot);
		qsort_charray(S, A, fe, score);
		qsort_charray(S, A+fg, len-fg, score);
	}
	else{
		int pivot;
		pivot = S[len/2].len;
		partition_len(S, len, &fe, &fg, pivot);
		qsort_charray(S, A, fe, score);
		qsort_charray(S + fg, A, len-fg, score);
	}
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
partition_score(line_t S[], int len, int *first_e, int *first_g, double pivot){
	int next, start, end;
	next = 0, start = 0, end = len;
	while (next < end){
		if ((S[next].stage2_score/pivot) < 1.00){
			entry_swap_score(&S[next], &S[start]);
			start++;
			next++;
	}else if ((S[next].stage2_score/pivot) > 1.00){
		end --;
		entry_swap_score(&S[next], &S[end]);
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

void 
entry_swap_score(line_t *a, line_t *b){
	line_t temp;
	temp = *a;
	*a = *b;
	*b = temp;
}
