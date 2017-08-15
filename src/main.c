/* Victor Forbes - 9293394 */

#include <stdlib.h>
#include "MyUtils.h"
#include "MyChessMain.h"

int main(int argc, char *argv[]){
	ChessBoard *board;
	char *FENBoard;

	// Lendo o FENBoard.
	FENBoard = ReadStretch(stdin, "\n");

	// Transformando em um tabuleiro de xadrez.
	board = GetBoard(FENBoard);

	// Executando o jogo.
	MainGame(board);

	// Liberando a memória alocada.
	free(FENBoard);
	FreeBoard(board);

	return 0;
}