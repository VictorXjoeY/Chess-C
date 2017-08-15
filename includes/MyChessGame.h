/* Victor Forbes - 9293394 */

#ifndef MY_CHESS_GAME_H
#define MY_CHESS_GAME_H

/* Função que executa o movimento. */
void ExecuteMovement(ChessBoard *, const char *);

/* Função que checa se o jogador atual está em Cheque-mate. */
bool Checkmate(ChessBoard *);

/* Função que checa se alguma das condições de empate foi satisfeita. */
bool Tie(ChessBoard *);

#endif