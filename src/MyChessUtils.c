/* Victor Forbes - 9293394 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "MyChessMain.h"
#include "MyChessUtils.h"
#include "MyChessMovements.h"

#define LENGTH_OF_FORBES_NOTATION 7

/* Função que verifica se há possibilidade do Rei ficar em risco
caso uma determinada casa fique vazia. */
bool KingMayBeInRisk(const ChessBoard *, int, int);

/* Função que verifica se o rei está em cheque. */
bool KingUnderAttack(const ChessBoard *);

/* Função que verifica se há alguma ameaça a partir de uma casa em uma determinada direção. */
bool UnderAttackInLine(const ChessBoard *, int, int, int, int, const char *);

/* Função que verifica se uma peça está contida em uma string. */
bool PieceInString(const ChessPiece *, const char *, int);

/* Função que verifica se o movimento em FEN simplificado é igual ao movimento
em FN (Forbes Notation). */
bool EqualMovements(const char *, const char *);

/* Função de comparação crescente dos movimentos */
int TypeMovementCompare(const void *, const void *);

/* Função que insere um tabuleiro em notação FEN na Hash Table. */
void FENInsert(ChessBoard *, const char *);

/* Função que coloca um '\0' logo após o indicador de En Passant na notação FEN. */
int CutFENBoard(char *, int);

/* Função que cria e inicializa um elemento da Hash Table. */
HashElement *CreateElement(char *);

int getTeam(const ChessPiece *piece){
	if (piece){
		if (piece->type >= 'A' && piece->type <= 'Z'){
			return WHITE;
		}

		if (piece->type >= 'a' && piece->type <= 'z'){
			return BLACK;
		}
	}

	return NONE;
}

int getType(const ChessPiece *piece){
	if (piece){
		if (piece->type == 'P' || piece->type == 'p'){
			return PAWN;
		}

		if (piece->type == 'N' || piece->type == 'n'){
			return KNIGHT;
		}

		if (piece->type == 'B' || piece->type == 'b'){
			return BISHOP;
		}

		if (piece->type == 'R' || piece->type == 'r'){
			return ROOK;
		}

		if (piece->type == 'Q' || piece->type == 'q'){
			return QUEEN;
		}

		if (piece->type == 'K' || piece->type == 'k'){
			return KING;
		}
	}

	return INVALID_PIECE;
}

int getValue(const ChessPiece *piece){
	int pieceType = getType(piece);

	if (pieceType == PAWN){
		return PAWN_VALUE;
	}

	if (pieceType == KNIGHT){
		return KNIGHT_VALUE;
	}

	if (pieceType == BISHOP){
		return BISHOP_VALUE;
	}

	if (pieceType == ROOK){
		return ROOK_VALUE;
	}

	if (pieceType == QUEEN){
		return QUEEN_VALUE;
	}

	if (pieceType == KING){
		return KING_VALUE;
	}

	return INVALID_PIECE;
}

void (*getMoveFunction(const ChessPiece *piece))(ChessBoard *, ChessPiece *, int, int){
	int pieceType = getType(piece);

	if (pieceType == PAWN){
		return &MovePawn;
	}

	if (pieceType == KNIGHT){
		return &MoveKnight;
	}

	if (pieceType == BISHOP){
		return &MoveBishop;
	}

	if (pieceType == ROOK){
		return &MoveRook;
	}

	if (pieceType == QUEEN){
		return &MoveQueen;
	}

	if (pieceType == KING){
		return &MoveKing;
	}

	return NULL;
}

bool isInside(int x, int y){
	if (x >= 0 && x < NUMBER_OF_RANKS && y >= 0 && y < NUMBER_OF_FILES){
		return true;
	}

	return false;
}

char XToFEN(int x){
	return NUMBER_OF_RANKS - x + '0';
}

char YToFEN(int y){
	return y + 'a';
}

int XFromFEN(char x){
	return NUMBER_OF_RANKS - x + '0';
}

int YFromFEN(char y){
	return y - 'a';
}

int ScoreTypeMovementCompare(const void *a, const void *b){
	int res;

	if ((*((MovementScoreType **)a))->score > (*((MovementScoreType **)b))->score){
		res = 1;
	}
	else if ((*((MovementScoreType **)a))->score < (*((MovementScoreType **)b))->score){
		res = -1;
	}
	else{
		res = - TypeMovementCompare(a, b);
	}

	return res;
}

ChessBoard *CloneBoard(const ChessBoard *b){
	char *FENBoard = GetFENBoard(b);
	ChessBoard *clone = GetBoard(FENBoard);

	free(FENBoard);

	return clone;
}

HashTable *CreateHashTable(){
	HashTable *table = (HashTable *)malloc(sizeof(HashTable));

	table->n = HASH_TABLE_SIZE;
	table->Element = (HashElement **)calloc(table->n, sizeof(HashElement *));
	table->insert = &FENInsert;

	return table;
}

void DestroyHashTable(HashTable *table){
	HashElement *ant;
	HashElement *cur;
	int i;

	for (i = 0; i < table->n; i++){
		ant = table->Element[i];
		cur = ant;

		while (cur){
			cur = cur->next;
			free(ant->string);
			free(ant);
			ant = cur;
		}
	}

	free(table->Element);
	free(table);
}

void ErasePiece(ChessPiece *p){
	int i;

	for (i = 0; i < p->n; i++){
		free(p->movement[i]);
	}

	if (p->movement){
		free(p->movement);
	}

	free(p);
}

void EraseMovements(ChessBoard *b){
	int i, j, k;

	for (i = 0; i < NUMBER_OF_RANKS; i++){
		for (j = 0; j < NUMBER_OF_FILES; j++){
			if (b->Board[i][j]){
				for (k = 0; k < b->Board[i][j]->n; k++){
					free(b->Board[i][j]->movement[k]);
				}

				if (b->Board[i][j]->movement){
					free(b->Board[i][j]->movement);
				}

				b->Board[i][j]->movement = NULL;
				b->Board[i][j]->n = 0;
			}
		}
	}
}

void SaveMovement(ChessPiece *p, int Xi, int Yi, int Xf, int Yf, bool attack, char special){
	p->movement = (char **)realloc(p->movement, (p->n + 1) * sizeof(char *));
	p->movement[p->n] = (char *)malloc(LENGTH_OF_FORBES_NOTATION * sizeof(char));

	// Informações armazenadas: Casa inicial - Casa final - Se é ataque ou não - Identificador de movimento especial.
	sprintf(p->movement[p->n], "%c%c%c%c%c%c", YToFEN(Yi), XToFEN(Xi), YToFEN(Yf), XToFEN(Xf), attack ? 'x' : '_', special ? special : '_');
	
	p->n++;
}

char *ValidMovement(const ChessBoard *b, const char *movement){
	int x, y, i;

	x = XFromFEN(movement[1]);
	y = YFromFEN(movement[0]);

	if (getTeam(b->Board[x][y]) == b->WhoseTurn){
		i = 0;

		while (i < b->Board[x][y]->n && !EqualMovements(movement, b->Board[x][y]->movement[i])){
			i++;
		}

		if (i < b->Board[x][y]->n){
			return b->Board[x][y]->movement[i];
		}
	}

	return NULL;
}

bool TryToMove(ChessBoard *b, int Xi, int Yi, int Xf, int Yf, bool forceCheck){
	bool res = true;
	ChessPiece *aux;

	// Apenas tenta se mover para checar se o Rei ficará em Cheque se o rei já estiver
	// em cheque, se movimento for do próprio Rei ou se há a chance do Rei estar em
	// perigo após o movimento.
	if (forceCheck || b->KingInCheck || b->Board[Xi][Yi]->type == 'K' || b->Board[Xi][Yi]->type == 'k' || KingMayBeInRisk(b, Xi, Yi)){
		aux = b->Board[Xf][Yf];
		b->Board[Xf][Yf] = b->Board[Xi][Yi];
		b->Board[Xi][Yi] = NULL;

		res = !KingUnderAttack(b);
	
		b->Board[Xi][Yi] = b->Board[Xf][Yf];
		b->Board[Xf][Yf] = aux;
	}

	return res;
}

bool UnderAttack(const ChessBoard *b, int x, int y){
	int vx[] = {x + 1, x - 1, x + 2, x - 2, x + 2, x - 2, x + 1, x - 1};
	int vy[] = {y - 2, y - 2, y - 1, y - 1, y + 1, y + 1, y + 2, y + 2};
	char enemyPawn, enemyKnight, enemyKing;
	int pawnPos, i, j;

	if (b->WhoseTurn == WHITE){
		pawnPos = x - 1;
		enemyPawn = 'p';
		enemyKnight = 'n';
		enemyKing = 'k';
	}
	else{
		pawnPos = x + 1;
		enemyPawn = 'P';
		enemyKnight = 'N';
		enemyKing = 'K';
	}

	// Checando se está sob ataque de um peão à esquerda.
	if (isInside(pawnPos, y - 1) && b->Board[pawnPos][y - 1] && b->Board[pawnPos][y - 1]->type == enemyPawn){
		return true;
	}

	// Checando se está sob ataque de um peão à esquerda.
	if (isInside(pawnPos, y + 1) && b->Board[pawnPos][y + 1] && b->Board[pawnPos][y + 1]->type == enemyPawn){
		return true;
	}

	// Checando se está sob ataque de um cavalo.
	for (i = 0; i < 8; i++){
		if (isInside(vx[i], vy[i]) && b->Board[vx[i]][vy[i]] && b->Board[vx[i]][vy[i]]->type == enemyKnight){
			return true;
		}
	}

	// Diagonal esquerda acima.
	if (UnderAttackInLine(b, x, y, UP, LEFT, "BQ")){
		return true;
	}

	// Diagonal esquerda abaixo.
	if (UnderAttackInLine(b, x, y, DOWN, LEFT, "BQ")){
		return true;
	}

	// Diagonal direita acima.
	if (UnderAttackInLine(b, x, y, UP, RIGHT, "BQ")){
		return true;
	}

	// Diagonal direita abaixo.
	if (UnderAttackInLine(b, x, y, DOWN, RIGHT, "BQ")){
		return true;
	}

	// Cima.
	if (UnderAttackInLine(b, x, y, UP, NO_DIRECTION, "RQ")){
		return true;
	}

	// Baixo.
	if (UnderAttackInLine(b, x, y, DOWN, NO_DIRECTION, "RQ")){
		return true;
	}

	// Esquerda.
	if (UnderAttackInLine(b, x, y, NO_DIRECTION, LEFT, "RQ")){
		return true;
	}
	
	// Direita.
	if (UnderAttackInLine(b, x, y, NO_DIRECTION, RIGHT, "RQ")){
		return true;
	}

	// Checando se está sob ataque de um Rei.
	for (i = x - 1; i <= x + 1; i++){
		for (j = y - 1; j <= y + 1; j++){
			if (isInside(i, j) && (i != x || j != y) && b->Board[i][j] && b->Board[i][j]->type == enemyKing){
				return true;
			}
		}
	}

	// Se não estiver sob ataque, retorne false.
	return false;
}

bool UnderAllyAttack(ChessBoard *b, int x, int y){
	bool res;

	b->WhoseTurn = !b->WhoseTurn;
	res = UnderAttack(b, x, y);
	b->WhoseTurn = !b->WhoseTurn;

	return res;
}

bool UnderEnemyAttack(ChessBoard *b, int x, int y){
	return UnderAttack(b, x, y);
}

bool KingMayBeInRisk(const ChessBoard *b, int x, int y){
	if (b->WhoseTurn == WHITE){
		// O Rei branco só está em risco se a posição a ficar desocupada for uma das seguintes.
		return b->WhiteKingX == x || b->WhiteKingY == y || abs(x - b->WhiteKingX) == abs(y - b->WhiteKingY);
	}

	// O Rei preto só está em risco se a posição a ficar desocupada for uma das seguintes.
	return b->BlackKingX == x || b->BlackKingY == y || abs(x - b->BlackKingX) == abs(y - b->BlackKingY);
}

bool KingUnderAttack(const ChessBoard *b){
	if (b->WhoseTurn == WHITE){
		return UnderAttack(b, b->WhiteKingX, b->WhiteKingY);
	}

	return UnderAttack(b, b->BlackKingX, b->BlackKingY);
}

bool UnderAttackInLine(const ChessBoard *b, int x, int y, int xStep, int yStep, const char *str){
	int enemy = !b->WhoseTurn;

	x += xStep;
	y += yStep;

	// Caminhando com o passo passado por parâmetro.
	while (isInside(x, y) && getTeam(b->Board[x][y]) == NONE){
		x += xStep;
		y += yStep;
	}

	// Verificando se achou uma peça inimiga de um tipo contido na str passada por parâmetro.
	if (isInside(x, y) && getTeam(b->Board[x][y]) == enemy && PieceInString(b->Board[x][y], str, enemy)){
		return true;
	}

	return false;
}

bool PieceInString(const ChessPiece *p, const char *str, int team){
	char piece = team == WHITE ? p->type : p->type - ('a' - 'A');
	int len = strlen(str);
	int i;

	for (i = 0; i < len; i++){
		if (str[i] == piece){
			return true;
		}
	}

	return false;
}

bool EqualMovements(const char *mov, const char *FNmov){
	return mov[2] == FNmov[2] && mov[3] == FNmov[3] && ((FNmov[5] >= 'A' && FNmov[5] <= 'Z') ? mov[4] == FNmov[5] : true);
}

int TypeMovementCompare(const void *a, const void *b){
	int res = (*((MovementScoreType **)a))->type - (*((MovementScoreType **)b))->type;

	if (res == 0){
		res = (*((MovementScoreType **)a))->movement[2] - (*((MovementScoreType **)b))->movement[2];

		if (res == 0){
			res = (*((MovementScoreType **)a))->movement[3] - (*((MovementScoreType **)b))->movement[3];
		}
	}

	return res;
}

void FENInsert(ChessBoard *b, const char *FENBoard){
	char *BoardState;
	HashElement *aux;
	int i, n, key;

	// Criando uma string chamada Board State que é igual à string
	// FENBoard, exceto pelo fato de não possuir o número de meios
	// turnos e o número de turnos completos.
	n = strlen(FENBoard);
	BoardState = (char *)malloc((n + 1) * sizeof(char));
	strcpy(BoardState, FENBoard);
	n = CutFENBoard(BoardState, n);

	// Gerando a chave para a string FENBoard.
	for (key = 0, i = 0; i < n; i++){
		key = (key * PRIME + BoardState[i]) % b->Table->n;
	}

	// Se já existir um elemento naquela posição.
	if (b->Table->Element[key]){
		aux = b->Table->Element[key];

		// Percorrendo a lista de elementos enquanto a string for diferente e houver mais elementos na lista.
		while (strcmp(aux->string, BoardState) && aux->next){
			aux = aux->next;
		}

		// Se a string for diferente, crie um novo elemento, se não, incremente o contador.
		if (strcmp(aux->string, BoardState)){
			// Colisão.
			aux->next = CreateElement(BoardState);
		}else{
			(aux->counter)++;

			// Liberando a memória alocada para armazenar essa string
			// pois já existe essa string na HashTable.
			free(BoardState);

			// Atualizando a flag de Tripla Repetição para true caso o contador
			// de ocorrências dessa string seja maior ou igual a 3.
			b->TripleRepetition = aux->counter >= 3 ? true : false;
		}
	}
	else{
		// Sem colisão.
		b->Table->Element[key] = CreateElement(BoardState);
	}
}

int CutFENBoard(char *FENBoard, int n){
	int i = n - 1;

	while (FENBoard[i] != ' '){
		i--;
	}

	i--;

	while (FENBoard[i] != ' '){
		i--;
	}

	FENBoard[i] = '\0';

	return i;
}

HashElement *CreateElement(char *str){
	HashElement *element = (HashElement *)malloc(sizeof(HashElement));

	element->string = str;
	element->counter = 1;
	element->next = NULL;

	return element;
}