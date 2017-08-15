/* Victor Forbes - 9293394 */

#include <stdlib.h>
#include "MyChessMain.h"
#include "MyChessUtils.h"
#include "MyChessMovements.h"
#include "MyUtils.h"

/* Função que gera os movimentos de ataque de um peão. */
void PawnAttack(ChessBoard *, ChessPiece *, int, int, int, int);

/* Função que gera os movimentos em uma dada direção. */
void MoveInLine(ChessBoard *, ChessPiece *, int, int, int, int);

/* Função que checa se a coordenada passada por parâmetro é igual à
coordenada do movimento En Passant. */
bool EnPassant(const ChessBoard *, int, int);

/* Função que checa se o Peão chegou do outro lado do tabuleiro. */
bool Promotion(const ChessBoard *, int, int);

/* Função que checa se o Roque é possível. */
bool Castling(const ChessBoard *, int, int, int);

void MovePawn(ChessBoard *b, ChessPiece *p, int x, int y){
	int oneSquareAhead, twoSquaresAhead, initialPosition;

	if (b->WhoseTurn == WHITE){
		oneSquareAhead = x - 1;
		twoSquaresAhead = x - 2;
		initialPosition = 6;
	}
	else{
		oneSquareAhead = x + 1;
		twoSquaresAhead = x + 2;
		initialPosition = 1;
	}

	// Ataque na diagonal esquerda.
	PawnAttack(b, p, x, y, oneSquareAhead, y - 1);
	
	// Ataque na diagonal direita.
	PawnAttack(b, p, x, y, oneSquareAhead, y + 1);

	// Um para frente.
	if (isInside(oneSquareAhead, y) && getTeam(b->Board[oneSquareAhead][y]) == NONE){
		if (TryToMove(b, x, y, oneSquareAhead, y, false)){
			if (Promotion(b, oneSquareAhead, y)){
				SaveMovement(p, x, y, oneSquareAhead, y, false, 'N');
				SaveMovement(p, x, y, oneSquareAhead, y, false, 'B');
				SaveMovement(p, x, y, oneSquareAhead, y, false, 'R');
				SaveMovement(p, x, y, oneSquareAhead, y, false, 'Q');
			}
			else{
				SaveMovement(p, x, y, oneSquareAhead, y, false, false);
			}
		}

		// Dois para frente.
		if (x == initialPosition && getTeam(b->Board[twoSquaresAhead][y]) == NONE){
			if (TryToMove(b, x, y, twoSquaresAhead, y, false)){
				SaveMovement(p, x, y, twoSquaresAhead, y, false, 'j'); // 'j' de jump.
			}
		}
	}
}

void MoveKnight(ChessBoard *b, ChessPiece *p, int x, int y){
	int Xf[] = {x + 1, x - 1, x + 2, x - 2, x + 2, x - 2, x + 1, x - 1}; // Criando vetores auxiliares com todas as casas
	int Yf[] = {y - 2, y - 2, y - 1, y - 1, y + 1, y + 1, y + 2, y + 2}; // de destino possíveis (já ordenadas).
	int enemy = !b->WhoseTurn;
	int team, i;

	// Tentando se mover para todas as casas possíveis.
	for (i = 0; i < 8; i++){
		if (isInside(Xf[i], Yf[i])){
			team = getTeam(b->Board[Xf[i]][Yf[i]]);

			if (team != b->WhoseTurn && TryToMove(b, x, y, Xf[i], Yf[i], false)){
				SaveMovement(p, x, y, Xf[i], Yf[i], team == enemy, false);
			}
		}
	}
}

void MoveBishop(ChessBoard *b, ChessPiece *p, int x, int y){
	// Diagonal esquerda acima.
	MoveInLine(b, p, x, y, UP, LEFT);

	// Diagonal esquerda abaixo.
	MoveInLine(b, p, x, y, DOWN, LEFT);

	// Diagonal direita acima.
	MoveInLine(b, p, x, y, UP, RIGHT);

	// Diagonal direita abaixo.
	MoveInLine(b, p, x, y, DOWN, RIGHT);
}

void MoveRook(ChessBoard *b, ChessPiece *p, int x, int y){
	// Cima.
	MoveInLine(b, p, x, y, UP, NO_DIRECTION);

	// Baixo.
	MoveInLine(b, p, x, y, DOWN, NO_DIRECTION);

	// Esquerda.
	MoveInLine(b, p, x, y, NO_DIRECTION, LEFT);

	// Direita.
	MoveInLine(b, p, x, y, NO_DIRECTION, RIGHT);
}

void MoveQueen(ChessBoard *b, ChessPiece *p, int x, int y){
	// Diagonal esquerda acima.
	MoveInLine(b, p, x, y, UP, LEFT);

	// Diagonal esquerda abaixo.
	MoveInLine(b, p, x, y, DOWN, LEFT);

	// Diagonal direita acima.
	MoveInLine(b, p, x, y, UP, RIGHT);

	// Diagonal direita abaixo.
	MoveInLine(b, p, x, y, DOWN, RIGHT);

	// Cima.
	MoveInLine(b, p, x, y, UP, NO_DIRECTION);

	// Baixo.
	MoveInLine(b, p, x, y, DOWN, NO_DIRECTION);

	// Esquerda.
	MoveInLine(b, p, x, y, NO_DIRECTION, LEFT);

	// Direita.
	MoveInLine(b, p, x, y, NO_DIRECTION, RIGHT);
}

void MoveKing(ChessBoard *b, ChessPiece *p, int x, int y){
	int enemy = !b->WhoseTurn;
	int KingX, KingY, team, i, j;

	// Tentando realizar o Roque.
	if (b->WhoseTurn == WHITE){
		if (b->CastlingWhiteKingSide && Castling(b, x, y, KING)){
			SaveMovement(p, x, y, x, y + 2, false, 'k');
		}

		if (b->CastlingWhiteQueenSide && Castling(b, x, y, QUEEN)){
			SaveMovement(p, x, y, x, y - 2, false, 'q');
		}

		// Salvando as coordenadas originais do rei branco.
		KingX = b->WhiteKingX;
		KingY = b->WhiteKingY;
	}
	else{
		if (b->CastlingBlackKingSide && Castling(b, x, y, KING)){
			SaveMovement(p, x, y, x, y + 2, false, 'k');
		}
		
		if (b->CastlingBlackQueenSide && Castling(b, x, y, QUEEN)){
			SaveMovement(p, x, y, x, y - 2, false, 'q');
		}

		// Salvando as coordenadas originais do rei preto.
		KingX = b->BlackKingX;
		KingY = b->BlackKingY;
	}

	for (i = x - 1; i <= x + 1; i++){
		for (j = y - 1; j <= y + 1; j++){
			if (isInside(i, j)){
				team = getTeam(b->Board[i][j]);

				if (team != b->WhoseTurn){
					// Alterando as coordenadas do rei para testar o movimento.
					if (b->WhoseTurn == WHITE){
						b->WhiteKingX = i;
						b->WhiteKingY = j;
					}
					else{
						b->BlackKingX = i;
						b->BlackKingY = j;
					}

					// Tentando mover a peça e salvando seus movimentos.
					if (TryToMove(b, x, y, i, j, false)){
						SaveMovement(p, x, y, i, j, team == enemy, false);
					}
				}
			}
		}
	}

	// Recuperando as coordenadas originais do rei.
	if (b->WhoseTurn == WHITE){
		b->WhiteKingX = KingX;
		b->WhiteKingY = KingY;
	}
	else{
		b->BlackKingX = KingX;
		b->BlackKingY = KingY;
	}
}

void PawnAttack(ChessBoard *b, ChessPiece *p, int Xi, int Yi, int Xf, int Yf){
	int enemy = !b->WhoseTurn;
	ChessPiece *aux;

	if (isInside(Xf, Yf)){
		if (getTeam(b->Board[Xf][Yf]) == enemy){
			if (TryToMove(b, Xi, Yi, Xf, Yf, false)){
				if (Promotion(b, Xf, Yf)){
					SaveMovement(p, Xi, Yi, Xf, Yf, true, 'N');
					SaveMovement(p, Xi, Yi, Xf, Yf, true, 'B');
					SaveMovement(p, Xi, Yi, Xf, Yf, true, 'R');
					SaveMovement(p, Xi, Yi, Xf, Yf, true, 'Q');
				}
				else{
					SaveMovement(p, Xi, Yi, Xf, Yf, true, false);
				}
			}
		}
		else if (EnPassant(b, Xf, Yf)){
			aux = b->Board[Xi][Yf];
			b->Board[Xi][Yf] = NULL;

			if (TryToMove(b, Xi, Yi, Xf, Yf, true)){
				SaveMovement(p, Xi, Yi, Xf, Yf, true, 'e');
			}

			b->Board[Xi][Yf] = aux;
		}
	}
}

void MoveInLine(ChessBoard *b, ChessPiece *p, int x, int y, int xStep, int yStep){
	int enemy = !b->WhoseTurn;
	int Xf = x + xStep;
	int Yf = y + yStep;

	while (isInside(Xf, Yf) && getTeam(b->Board[Xf][Yf]) == NONE){
		if (TryToMove(b, x, y, Xf, Yf, false)){
			SaveMovement(p, x, y, Xf, Yf, false, false);
		}

		Xf += xStep;
		Yf += yStep;
	}

	if (isInside(Xf, Yf) && getTeam(b->Board[Xf][Yf]) == enemy && TryToMove(b, x, y, Xf, Yf, false)){
		SaveMovement(p, x, y, Xf, Yf, true, false);
	}
}

bool EnPassant(const ChessBoard *b, int x, int y){
	return (YToFEN(y) == b->EnPassant[0] && XToFEN(x) == b->EnPassant[1]);
}

bool Promotion(const ChessBoard *b, int x, int y){
	return (b->WhoseTurn == WHITE ? x == 0 : x == 7);
}

bool Castling(const ChessBoard *b, int x, int y, int side){
	int step = side == KING ? 1 : -1;
	int corner = side == KING ? 7 : 0;
	int i = y + step;

	if (b->KingInCheck){
		return false;
	}

	// Verificando se há alguma peça entre o Rei e a Torre.
	while (getTeam(b->Board[x][i]) == NONE){
		i += step;
	}

	// Se não houver nenhuma peça entre o Rei e a Torre e as casas por onde o rei
	// passa não estiverem sob ataque, retorne true.
	if (i == corner && !UnderAttack(b, x, y + step) && !UnderAttack(b, x, y + 2 * step)){
		return true;
	}

	// Se não, retorne false.
	return false;
}