/* Victor Forbes - 9293394 */

#ifndef MY_CHESS_UTILS_H
#define MY_CHESS_UTILS_H

#include "MyChessMain.h"

#define NO_DIRECTION 0
#define UP -1
#define DOWN 1
#define LEFT -1
#define RIGHT 1

#define HASH_TABLE_SIZE 499
#define PRIME 13

/* Função que dada uma peça retorna o seu time. Retorna NONE caso não seja uma peça. */
int getTeam(const ChessPiece *);

/* Função que dada uma peça retorna o seu tipo. Retorna INVALID_PIECE caso não seja uma peça. */
int getType(const ChessPiece *);

/* Função que dada uma peça retorna o seu valor. Retorna INVALID_PIECE caso não seja uma peça. */
int getValue(const ChessPiece *);

/* Função que dada uma peça retorna o endereço da sua função de movimento. Retorna NULL caso não seja uma peça. */
void (*getMoveFunction(const ChessPiece *))(ChessBoard *, ChessPiece *, int, int);

/* Função que checa se as coordenadas passadas por parâmetro estão dentro
do tabuleiro. */
bool isInside(int, int);

/* Função que converte a coordenada X (linha) para a notação FEN. */
char XToFEN(int);

/* Função que converte a coordenada Y (coluna) para a notação FEN. */
char YToFEN(int);

/* Função que converte a coordenada X (linha) da notação FEN. */
int XFromFEN(char);

/* Função que converte a coordenada Y (coluna) da notação FEN. */
int YFromFEN(char);

/* Função de comparação decrescente para Pontuação (com o tipo da peça e o movimento como medida de desempate). */
int ScoreTypeMovementCompare(const void *, const void *);

// Função que clona um tabuleiro.
ChessBoard *CloneBoard(const ChessBoard *);

/* Retorna uma HashTable vazia. */
HashTable *CreateHashTable();

/* Libera a memória alocada para armazenar uma HashTable.*/
void DestroyHashTable(HashTable *);

/* Função que apaga uma peça. */
void ErasePiece(ChessPiece *);

/* Função que apaga todos os movimentos do tabuleiro. */
void EraseMovements(ChessBoard *);

/* Função que salva um movimento na notação FEN. */
void SaveMovement(ChessPiece *, int, int, int, int, bool, char);

/* Função que checa se o movimento passado por parâmetro é válido.
Caso seja inválido, a função retorna NULL. */
char *ValidMovement(const ChessBoard *, const char *);

/* Função que verifica se mover uma determinada peça irá fazer com que o
Rei fique em Cheque. */
bool TryToMove(ChessBoard *, int, int, int, int, bool);

/* Função que verifica se o Rei está sob ataque (em Cheque). */
bool KingUnderAttack(const ChessBoard *);

/* Função que verifica se uma determinada casa do tabuleiro está sob ataque. */
bool UnderAttack(const ChessBoard *, int, int);

// Função que verifica se a casa está sob ataque de uma peça aliada.
bool UnderAllyAttack(ChessBoard *, int, int);

// Função que verifica se a casa está sob ataque de uma peça inimiga.
bool UnderEnemyAttack(ChessBoard *, int, int);

#endif
