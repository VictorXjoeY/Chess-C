/* Victor Forbes - 9293394 */

#ifndef MY_CHESS_MAIN_H
#define MY_CHESS_MAIN_H

#include "MyUtils.h"

#define NUMBER_OF_RANKS 8
#define NUMBER_OF_FILES 8

#define NONE -1
#define WHITE 0
#define BLACK 1

#define INVALID_PIECE -1
#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5

#define PAWN_VALUE 100
#define KNIGHT_VALUE 325
#define BISHOP_VALUE 325
#define ROOK_VALUE 550
#define QUEEN_VALUE 1000
#define KING_VALUE 50000

typedef struct ChessPiece ChessPiece;
typedef struct ChessBoard ChessBoard;
typedef struct MovementScoreType MovementScoreType;
typedef struct HashTable HashTable;
typedef struct HashElement HashElement;

/* Tabela Hash para FEN Boards com função de inserção. */
struct HashTable{
	HashElement **Element;
	void (*insert)(ChessBoard *, const char *);
	int n;
};

/* Elemento de uma Tabela Hash. */
struct HashElement{
	char *string;
	int counter;
	struct HashElement *next;
};

/* Estrutura que guarda informações de cada peça. */
struct ChessPiece{
	char type; // Tipo da peça.
	void (*move)(ChessBoard *, ChessPiece *, int, int); // Endereço da função que gera seus movimentos.
	char **movement; // Vetor de movimentos na notação Forsyth-Edwards.
	int n; // Quantidade de movimentos da peça.
};

/* Estrutura que guarda todas as informações do jogo. */
struct ChessBoard{
	HashTable *Table; // Tabela hash.
	ChessPiece ***Board; // Matriz de peças.
	int WhiteKingX; // Coordenada X do rei branco.
	int WhiteKingY; // Coordenada Y do rei branco.
	int BlackKingX; // Coordenada X do rei preto.
	int BlackKingY; // Coordenada Y do rei preto.
	bool WhoseTurn; // De quem é a vez.
	bool CastlingWhiteKingSide; // Se o Roque para as peças brancas para o lado do Rei está disponível.
	bool CastlingWhiteQueenSide; // Se o Roque para as peças brancas para o lado da Rainha está disponível.
	bool CastlingBlackKingSide; // Se o Roque para as peças pretas para o lado do Rei está disponível.
	bool CastlingBlackQueenSide; // Se o Roque para as peças pretas para o lado da Rainha está disponível.
	bool KingInCheck; // Se o Rei está em Cheque ou não.
	char *EnPassant; // Casa do movimento En Passant.
	int HalfTurns; // Número de meios-turnos desde a última captura ou o último movimento de um peão.
	int Turns; // Número de turnos completos.
	bool TripleRepetition; // Flag que é setada para true caso haja tripla repetição de um estado do tabuleiro.
};

/* Estrutura que guarda uma tripla: movimento, pontuação e tipo da peça que realiza o movimento. */
struct MovementScoreType{
	char *movement; // Movimento.
	double score; // Pontuação do movimento.
	int type; // Tipo da peça que executa o movimento.
};

/* Função que preenche todos os campos da struct ChessBoard a partir
de uma string com a notação Forsyth-Edwards. */
ChessBoard *GetBoard(const char *);

/* Função que retorna uma string com a notação Forsyth-Edwards a partir
da struct ChessBoard. */
char *GetFENBoard(const ChessBoard *);

/* Função que roda o jogo de Xadrez. */
void MainGame(ChessBoard *);

/* Função que libera toda a memória alocada pelo tabuleiro. */
void FreeBoard(ChessBoard *);

#endif