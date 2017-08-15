/* Victor Forbes - 9293394 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MyUtils.h"

/* Função que checa se uma string contém um caractere. */
bool StringContainsChar(const char *, char, int);

char *ReadStretch(FILE *fp, const char *str){
    char *stretch = NULL;
    int len = strlen(str);
    int i = 0;

    do{
        stretch = (char *)realloc(stretch, (i + 1) * sizeof(char));
        fscanf(fp, "%c", stretch + i);
        i++;
    }while (!feof(fp) && !StringContainsChar(str, stretch[i - 1], len));
    // Apenas sai do laço caso encontre o final do arquivo ou algum caractere contido na string str.

    // Transformando o vetor de caracteres em uma string.
    stretch[i - 1] = '\0';

    return stretch;
}

bool StringContainsChar(const char *str, char c, int len){
    int i;

    for (i = 0; i < len; i++){
        if (str[i] == c){
            return true;
        }
    }

    return false;
}

void *GetMax(void *vector, int size, int n, int (*compare)(const void *, const void *)){
	void *max = vector;
	int i;

	for (i = 1; i < n; i++){
		if (compare(vector + i * size, max) > 0){
			max = vector + i * size;
		}
	}

	return max;
}