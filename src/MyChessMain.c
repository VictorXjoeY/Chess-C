/* Victor Forbes - 9293394 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "MyChessMain.h"
#include "MyChessGame.h"
#include "MyChessUtils.h"
#include "MyChessMovements.h"
#include "MyUtils.h"

#define FEN_BOARD_STRING_MAX_SIZE 100

/* Função que gera os movimentos de cada peça. */
void GenerateMovements(ChessBoard *);

/* Função que imprime o tabuleiro. */
void PrintBoard(const ChessBoard *);

/* Função que imprime todos os movimentos possíveis. */
void PrintMovements(const ChessBoard *);

ChessBoard *GetBoard(const char *FENBoard){
	ChessBoard *b = (ChessBoard *)malloc(sizeof(ChessBoard));
	b->Board = (ChessPiece ***)malloc(NUMBER_OF_RANKS * sizeof(ChessPiece **));
	int i, j, k;

	// Variável usada para indexar as posições da string FENBoard.
	k = 0;

	for (i = 0; i < NUMBER_OF_RANKS; i++){
		// Inicializando o tabuleiro com NULL.
		b->Board[i] = (ChessPiece **)calloc(NUMBER_OF_FILES, sizeof(ChessPiece *));

		for (j = 0; j < NUMBER_OF_FILES; j++){
			if (FENBoard[k] >= '1' && FENBoard[k] <= '8'){
				j += (FENBoard[k] - '0') - 1;
			}
			else{
				// Criando uma nova peça.
				b->Board[i][j] = (ChessPiece *)malloc(sizeof(ChessPiece));
				b->Board[i][j]->type = FENBoard[k];
				b->Board[i][j]->move = getMoveFunction(b->Board[i][j]);
				b->Board[i][j]->movement = NULL;
				b->Board[i][j]->n = 0;

				// Gravando em ponteiros "especiais" os dois reis.
				if (b->Board[i][j]->type == 'K'){
					b->WhiteKingX = i;
					b->WhiteKingY = j;
				}
				else if (b->Board[i][j]->type == 'k'){
					b->BlackKingX = i;
					b->BlackKingY = j;
				}
			}

			k++;
		}

		// Ignorando o caractere '/'.
		k++;
	}

	// Setando a variável que indica de quem é a vez.
	if (FENBoard[k] == 'w'){
		b->WhoseTurn = WHITE;
	}
	else{
		b->WhoseTurn = BLACK;
	}

	k += 2;

	b->CastlingWhiteKingSide = false;
	b->CastlingWhiteQueenSide = false;
	b->CastlingBlackKingSide = false;
	b->CastlingBlackQueenSide = false;

	// Lendo a disponibilidade de Roque. (FENBoard[k] vale ou 'w' ou 'b' nesse ponto do código)
	while (FENBoard[k] != ' '){
		if (FENBoard[k] == 'K'){
			b->CastlingWhiteKingSide = true;
		}
		else if (FENBoard[k] == 'Q'){
			b->CastlingWhiteQueenSide = true;
		}
		else if (FENBoard[k] == 'k'){
			b->CastlingBlackKingSide = true;
		}
		else if (FENBoard[k] == 'q'){
			b->CastlingBlackQueenSide = true;
		}

		k++;
	}

	k++;

	// Lendo a casa para realizar En Passant.
	b->EnPassant = (char *)malloc(3 * sizeof(char));
	sscanf(FENBoard + k, "%s", b->EnPassant);

	k += strlen(b->EnPassant) + 1;

	// Lendo a quantidade de meios-turnos.
	sscanf(FENBoard + k, "%d", &(b->HalfTurns));

	k += 2;

	// Lendo a quantidade de turnos completos.
	sscanf(FENBoard + k, "%d", &(b->Turns));

	b->Table = NULL;
	b->TripleRepetition = false;

	return b;
}

char *GetFENBoard(const ChessBoard *b){
	char *FENBoard = (char *)malloc(FEN_BOARD_STRING_MAX_SIZE * sizeof(char));
	char counter = '0';
	int i, j, k;

	k = 0;

	for (i = 0; i < NUMBER_OF_RANKS; i++){
		for (j = 0; j < NUMBER_OF_FILES; j++){
			// Se tiver uma peça nessa casa.
			if (b->Board[i][j]){
				// Se o contador não estiver em 0, grave o valor do contador.
				if (counter != '0'){
					FENBoard[k] = counter;
					counter = '0';
					k++;
				}

				// Gravando o caractere que representa a peça.
				FENBoard[k] = b->Board[i][j]->type;
				k++;
			}
			else{
				// Caso não tenha uma peça na casa, incremente o contador.
				counter++;
			}
		}

		// Ao final de uma linha, se o contador for diferente de 0, grave seu valor.
		if (counter != '0'){
			FENBoard[k] = counter;
			counter = '0';
			k++;
		}

		// Gravando a separação de linhas.
		FENBoard[k] = '/';
		k++;
	}

	// Gravando de quem é a vez.
	if (b->WhoseTurn == WHITE){
		sprintf(FENBoard + k - 1, " w ");
	}
	else{
		sprintf(FENBoard + k - 1, " b ");
	}

	k += 2;

	// Gravando a disponibilidade de Roque.
	if (b->CastlingWhiteKingSide || b->CastlingWhiteQueenSide || b->CastlingBlackKingSide || b->CastlingBlackQueenSide){
		if (b->CastlingWhiteKingSide){
			FENBoard[k] = 'K';
			k++;
		}

		if (b->CastlingWhiteQueenSide){
			FENBoard[k] = 'Q';
			k++;
		}

		if (b->CastlingBlackKingSide){
			FENBoard[k] = 'k';
			k++;
		}

		if (b->CastlingBlackQueenSide){
			FENBoard[k] = 'q';
			k++;
		}
	}
	else{
		// Gravando o caractere '-' caso não haja disponibilidade de Roque.
		FENBoard[k] = '-';
		k++;
	}

	// Gravando En Passant, quantidade de meios turnos e quantidade de turnos completos.
	sprintf(FENBoard + k, " %s %d %d", b->EnPassant, b->HalfTurns, b->Turns);

	// Realocando para usar apenas o espaço necessário para armazenar o tabuleiro na notação FEN.
	FENBoard = (char *)realloc(FENBoard, (strlen(FENBoard) + 1) * sizeof(char));

	return FENBoard;
}

double GetScore(const ChessBoard *b, const char *movement){
	int team, value, enemy, i, j;
	double num, den;
	ChessBoard *auxBoard;

	// Criando um tabuleiro auxiliar (idêntico). Clonando tabuleiro.
	auxBoard = CloneBoard(b);

	// Executando o movimento no tabuleiro auxiliar.
	ExecuteMovement(auxBoard, movement);

	enemy = !auxBoard->WhoseTurn;
	num = 0.0;
	den = 0.0;

	for (i = 0; i < NUMBER_OF_RANKS; i++){
		for (j = 0; j < NUMBER_OF_FILES; j++){
			team = getTeam(auxBoard->Board[i][j]);
			value = getValue(auxBoard->Board[i][j]);

			// Determinando os valores a serem somados no numerador e no denominador.
			if (team == auxBoard->WhoseTurn){ // Se for aliado.
				num += UnderAllyAttack(auxBoard, i, j) ? (double)value / 2.0 : 0.0;
				den += UnderEnemyAttack(auxBoard, i, j) ? (double)value : 0.0;
			}
			else if (team == enemy){ // Se for inimigo.
				num += UnderAllyAttack(auxBoard, i, j) ? (double)value : 0.0;
				den += UnderEnemyAttack(auxBoard, i, j) ? (double)value / 2.0 : 0.0;
			}
			else{ // Se for uma casa vazia.
				num += UnderAllyAttack(auxBoard, i, j) ? 50.0 : 0.0;
				den += UnderEnemyAttack(auxBoard, i, j) ? 50.0 : 0.0;
			}
		}
	}

	// Liberando a memória alocada pela função.
	FreeBoard(auxBoard);

	// Retornando a pontuação do movimento.
	return num / (den + 1.0);
}

char *GetBestMovement(const ChessBoard *b){
	MovementScoreType **v = NULL;
	int i, j, k, n;
	char *BestMovement;

	n = 0;

	for (i = 0; i < NUMBER_OF_RANKS; i++){
		for (j = 0; j < NUMBER_OF_FILES; j++){
			if (getTeam(b->Board[i][j]) == b->WhoseTurn){
				for (k = 0; k < b->Board[i][j]->n; k++){
					v = (MovementScoreType **)realloc(v, (n + 1) * sizeof(MovementScoreType *));
					v[n] = (MovementScoreType *)malloc(sizeof(MovementScoreType));

					v[n]->movement = b->Board[i][j]->movement[k];
					v[n]->score = GetScore(b, v[n]->movement);
					v[n]->type = getType(b->Board[i][j]);

					n++;
				}
			}
		}
	}

	// Retornando o máximo MovementScoreType e extraindo seu movimento.
	BestMovement = (*((MovementScoreType **)GetMax(v, sizeof(MovementScoreType *), n, &ScoreTypeMovementCompare)))->movement;

	// Liberando a memória alocada para armazenar o vetor de Movimento-Pontuação-Tipo.
	for (i = 0; i < n; i++){
		free(v[i]);
	}

	free(v);

	return BestMovement;
}

void MainGame(ChessBoard *b){
	char *validMovement;
	char *movement;
	char *FENBoard;

	// Inicializando a Tabela Hash.
	b->Table = CreateHashTable();

	// Gerando os movimentos.
	GenerateMovements(b);

	// Gravando o estado do jogo em uma Tabela Hash.
	FENBoard = GetFENBoard(b);
	b->Table->insert(b, FENBoard);

	// Imprimindo o tabuleiro.
	printf("%s\n", FENBoard);
	free(FENBoard);

	// Enquanto não houver vitória ou empate.
	while (!Checkmate(b) && !Tie(b)){
		if (b->WhoseTurn == WHITE){
			movement = ReadStretch(stdin, "\n");

			// Enquanto o movimento for inválido.
			while (!(validMovement = ValidMovement(b, movement))){
				free(movement);
				printf("Movimento invalido. Tente novamente.\n");
				movement = ReadStretch(stdin, "\n");
			}

			// Liberando o movimento inserido pelo usuário.
			free(movement);
		}
		else{
			validMovement = GetBestMovement(b);
		}

		// Executando o movimento.
		ExecuteMovement(b, validMovement);

		// Incrementando o número de turnos.
		if (b->WhoseTurn == BLACK){
			b->Turns++;
		}

		// Atualizando de quem é a vez.
		b->WhoseTurn = !b->WhoseTurn;

		// Gerando os movimentos.
		GenerateMovements(b);

		// Gravando o estado do jogo em uma Tabela Hash.
		FENBoard = GetFENBoard(b);
		b->Table->insert(b, FENBoard);

		// Imprimindo o tabuleiro.
		printf("%s\n", FENBoard);
		free(FENBoard);
	}
}

void GenerateMovements(ChessBoard *b){
	int i, j;

	// Verificando se o Rei está em cheque.
	b->KingInCheck = KingUnderAttack(b);

	// Apagando todos os movimentos salvos.
	EraseMovements(b);

	for (i = 0; i < NUMBER_OF_RANKS; i++){
		for (j = 0; j < NUMBER_OF_FILES; j++){
			if (b->Board[i][j] && getTeam(b->Board[i][j]) == b->WhoseTurn){
				b->Board[i][j]->move(b, b->Board[i][j], i, j);
			}
		}
	}
}

void PrintBoard(const ChessBoard *b){
	int i, j;

	// Imprimindo o tabuleiro.
	for (i = 0; i < NUMBER_OF_RANKS; i++){
		for (j = 0; j < NUMBER_OF_FILES; j++){
			if (b->Board[i][j]){
				printf("%c ", b->Board[i][j]->type);
			}
			else{
				printf(". ");
			}
		}
		printf("\n");
	}
}

void PrintMovements(const ChessBoard *b){
	int i, j, k;

	// Imprimindo todos os movimentos possíveis.
	for (i = 0; i < NUMBER_OF_RANKS; i++){
		for (j = 0; j < NUMBER_OF_FILES; j++){
			if (b->Board[i][j] && getTeam(b->Board[i][j]) == b->WhoseTurn){
				for (k = 0; k < b->Board[i][j]->n; k++){
					printf("%s\n", b->Board[i][j]->movement[k]);
				}
			}
		}
	}
}

void FreeBoard(ChessBoard *b){
	int i, j, k;

	for (i = 0; i < NUMBER_OF_RANKS; i++){
		for (j = 0; j < NUMBER_OF_FILES; j++){
			// Caso tenha uma peça na casa.
			if (b->Board[i][j]){
				// Liberando seus movimentos.
				for (k = 0; k < b->Board[i][j]->n; k++){
					free(b->Board[i][j]->movement[k]);
				}

				// Liberando o vetor de movimentos (caso não seja NULL).
				if (b->Board[i][j]->movement){
					free(b->Board[i][j]->movement);
				}

				// Liberando a peça em si.
				free(b->Board[i][j]);
			}
		}

		// Liberando cada linha da matriz.
		free(b->Board[i]);
	}

	// Liberando a memória alocada para armazenar a HashTable.
	if (b->Table){
		DestroyHashTable(b->Table);
	}

	// Liberando a matriz, a string que grava o En Passant e por fim a própria struct.
	free(b->Board);
	free(b->EnPassant);
	free(b);
}