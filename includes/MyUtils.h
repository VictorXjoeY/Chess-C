/* Victor Forbes - 9293394 */

#ifndef MY_UTILS_H
#define MY_UTILS_H

#include <stdio.h>

#define true 1
#define false 0

typedef unsigned char bool;

/* Função que lê um trecho de um arquivo qualquer (incluindo a stdin) até
o final do arquivo ou até algum caractere da string passada por parâmetro. */
char *ReadStretch(FILE *, const char *);

/* Função que retorna o máximo de um vetor. */
void *GetMax(void *, int, int, int (*)(const void *, const void *));

#endif