/* Victor Forbes - 9293394 */

#include <stdio.h>
#include "MyChessMain.h"
#include "MyChessGame.h"
#include "MyChessUtils.h"
#include "MyChessMovements.h"
#include "MyUtils.h"

/* Função que move uma da casa (Xi, Yi) para a casa (Xf, Yf). */
void MovePiece(ChessBoard *, int, int, int, int);

/* Função que mata a peça na casa (x, y). */
void KillPiece(ChessBoard *, int, int);

/* Função que promove um peão. */
void PromotePawn(ChessPiece *, char);

/* Função que checa se algo ocorreu na casa (x, y). */
bool SomethingHappened(int, int, int, int, int, int);

/* Função que checa se o jogador atual não possui mais movimentos válidos. */
bool NoMovementsLeft(const ChessBoard *);

/* Função que checa se não há mais material para realizar cheque-mate. */
bool NoMaterial(const ChessBoard *);

void ExecuteMovement(ChessBoard *b, const char *movement){
	int type, team, Xi, Yi, Xf, Yf;

	// Pegando as coordenadas da casa de origem e da casa de destino.
	Xi = XFromFEN(movement[1]);
	Yi = YFromFEN(movement[0]);
	Xf = XFromFEN(movement[3]);
	Yf = YFromFEN(movement[2]);

	// Pegando o tipo e o time da peça.
	type = getType(b->Board[Xi][Yi]);
	team = getTeam(b->Board[Xi][Yi]);

	// Atualizando o En Passant.
	sprintf(b->EnPassant, "-");

	// Se for um ataque.
	if (movement[4] == 'x'){
		// Se for En Passant, mate a peça na mesma linha mas na coluna de destino da peça.
		if (movement[5] == 'e'){
			KillPiece(b, Xi, Yf);
		}
		else{
			KillPiece(b, Xf, Yf);
		}

		// Atualizando o número de meios turnos.
		b->HalfTurns = 0;
	}
	else{
		if (type == PAWN){
			// Atualizando EnPassant.
			if (movement[5] == 'j'){
				sprintf(b->EnPassant, "%c%c", YToFEN(Yi), team == WHITE ? XToFEN(Xf + 1) : XToFEN(Xf - 1));
			}

			// Atualizando o número de meios turnos.
			b->HalfTurns = 0;
		}else{
			b->HalfTurns++;
		}
	}

	// Tratando casos específicos para o movimento do Rei.
	if (type == KING){
		// Atualizando a disponibilidade de Roque e a posição do Rei.
		if (team == WHITE){
			b->CastlingWhiteKingSide = false;
			b->CastlingWhiteQueenSide = false;
			b->WhiteKingX = Xf;
			b->WhiteKingY = Yf;
		}
		else{
			b->CastlingBlackKingSide = false;
			b->CastlingBlackQueenSide = false;
			b->BlackKingX = Xf;
			b->BlackKingY = Yf;		
		}

		// Executando o Roque.
		if (movement[5] == 'k'){
			MovePiece(b, Xi, 7, Xf, Yf - 1);
		}
		else if (movement[5] == 'q'){
			MovePiece(b, Xi, 0, Xf, Yf + 1);
		}
	}

	// Atualizando o Roque se algo aconteceu com alguma torre.
	b->CastlingWhiteKingSide = SomethingHappened(7, 7, Xi, Yi, Xf, Yf) ? false : b->CastlingWhiteKingSide;
	b->CastlingWhiteQueenSide = SomethingHappened(7, 0, Xi, Yi, Xf, Yf) ? false : b->CastlingWhiteQueenSide;
	b->CastlingBlackKingSide = SomethingHappened(0, 7, Xi, Yi, Xf, Yf) ? false : b->CastlingBlackKingSide;
	b->CastlingBlackQueenSide = SomethingHappened(0, 0, Xi, Yi, Xf, Yf) ? false : b->CastlingBlackQueenSide;

	// Movendo a peça.
	MovePiece(b, Xi, Yi, Xf, Yf);

	// Caso o movimento seja uma promoção.
	if (movement[5] >= 'A' && movement[5] <= 'Z'){
		PromotePawn(b->Board[Xf][Yf], movement[5]);
	}
}

bool Checkmate(ChessBoard *b){
	// Caso não haja mais movimentos e o rei esteja em cheque.
	if (b->KingInCheck && NoMovementsLeft(b)){
		printf("Xeque-mate -- Vitoria: %s\n", b->WhoseTurn == WHITE ? "PRETO" : "BRANCO");
		return true;
	}

	return false;
}

bool Tie(ChessBoard *b){
	// Caso não haja mais movimentos e o rei não esteja em cheque.
	if (!b->KingInCheck && NoMovementsLeft(b)){
		printf("Empate -- Afogamento\n");
		return true;
	}

	// Regra dos 50 movimentos.
	if (b->HalfTurns >= 50){
		printf("Empate -- Regra dos 50 Movimentos\n");
		return true;
	}

	// Caso não haja peças o suficiente para realizar um cheque-mate.
	if (NoMaterial(b)){
		printf("Empate -- Falta de Material\n");
		return true;
	}

	if (b->TripleRepetition){
		printf("Empate -- Tripla Repeticao\n");
		return true;
	}

	return false;
}

void MovePiece(ChessBoard *b, int Xi, int Yi, int Xf, int Yf){
	b->Board[Xf][Yf] = b->Board[Xi][Yi];
	b->Board[Xi][Yi] = NULL;
}

void KillPiece(ChessBoard *b, int x, int y){
	ErasePiece(b->Board[x][y]);
	b->Board[x][y] = NULL;
}

void PromotePawn(ChessPiece *p, char piece){
	if (getTeam(p) == WHITE){
		p->type = piece;
	}
	else{
		p->type = piece + ('a' - 'A');
	}

	p->move = getMoveFunction(p);
}

bool SomethingHappened(int x, int y, int Xi, int Yi, int Xf, int Yf){
	return (Xi == x && Yi == y) || (Xf == x && Yf == y);
}

bool NoMovementsLeft(const ChessBoard *b){
	int i, j;

	for (i = 0; i < NUMBER_OF_RANKS; i++){
		for (j = 0; j < NUMBER_OF_FILES; j++){
			if (getTeam(b->Board[i][j]) == b->WhoseTurn && b->Board[i][j]->n){
				return false;
			}
		}
	}

	return true;
}

bool NoMaterial(const ChessBoard *b){
	int WhiteCounter, BlackCounter, team, i, j;
	bool WhiteKnight, WhiteBishop;
	bool BlackKnight, BlackBishop;

	WhiteKnight = false;
	WhiteBishop = false;
	BlackKnight = false;
	BlackBishop = false;
	WhiteCounter = 0;
	BlackCounter = 0;

	// Percorrendo o tabuleiro inteiro contando as peças brancas e pretas
	// e marcando se tem cavalo/bispo em ambos os times.
	for (i = 0; i < NUMBER_OF_RANKS; i++){
		for (j = 0; j < NUMBER_OF_FILES; j++){
			team = getTeam(b->Board[i][j]);

			if (team == WHITE){
				if (b->Board[i][j]->type == 'N'){
					WhiteKnight = true;
				}
				else if (b->Board[i][j]->type == 'B'){
					WhiteBishop = true;
				}

				WhiteCounter++;
			}
			else if (team == BLACK){
				if (b->Board[i][j]->type == 'n'){
					BlackKnight = true;
				}
				else if (b->Board[i][j]->type == 'b'){
					BlackBishop = true;
				}

				BlackCounter++;
			}
		}
	}

	// Caso Rei contra Rei.
	if (WhiteCounter == 1 && BlackCounter == 1){
		return true;
	}

	// Caso Rei branco e (Bispo ou Cavalo) contra Rei preto.
	if (WhiteCounter == 2 && BlackCounter == 1 && (WhiteKnight || WhiteBishop)){
		return true;
	}

	// Caso Rei preto e (Bispo ou Cavalo) contra Rei branco.
	if (WhiteCounter == 1 && BlackCounter == 2 && (BlackKnight || BlackBishop)){
		return true;
	}

	return false;
}