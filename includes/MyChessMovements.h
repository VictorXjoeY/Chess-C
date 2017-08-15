/* Victor Forbes - 9293394 */

#ifndef MY_CHESS_MOVEMENTS_H
#define MY_CHESS_MOVEMENTS_H

/* Função que gera os movimentos do Peão. */
void MovePawn(ChessBoard *, ChessPiece *, int, int);

/* Função que gera os movimentos do Cavalo. */
void MoveKnight(ChessBoard *, ChessPiece *, int, int);

/* Função que gera os movimentos do Bispo. */
void MoveBishop(ChessBoard *, ChessPiece *, int, int);

/* Função que gera os movimentos da Torre. */
void MoveRook(ChessBoard *, ChessPiece *, int, int);

/* Função que gera os movimentos da Rainha. */
void MoveQueen(ChessBoard *, ChessPiece *, int, int);

/* Função que gera os movimentos do Rei. */
void MoveKing(ChessBoard *, ChessPiece *, int, int);

#endif