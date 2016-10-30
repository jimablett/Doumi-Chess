#include <stdio.h>
#include <iostream>
#include <string>
#include <Windows.h>
#include <fstream>
#include <array>
#include "board.h"
#include "movegen.h"
#include "timer.h"
#include "evaluation.h"
#include "pgn.h"

#include "debug.h"
#include "command.h"

/******************************************************************************/
/*                                   DEBUG                                    */
/******************************************************************************/
void main() {
     Board initialBoard;
     
     printf("--------------------------------------------------------------------------------\n");
     printf(" Board120Setup / FENBoardSetup Testing                                          \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     FENboardSetup(initialBoard, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
     boardToFEN(initialBoard);

     board120Setup(initialBoard);
     boardToFEN(initialBoard);


     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" MoveGen Testing                                                                \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");
     
     MoveList moveList = moveGeneration(initialBoard);

     printf("Movecount: %d\n", moveList.getCounter());
     for (int i = 0; i < moveList.getCounter(); i++) {
          printf("%2d  ", i + 1);
          std::cout << numberToFilerank(moveList.getMove(i).getInitial())
               << numberToFilerank(moveList.getMove(i).getTerminal()) << std::endl;
     }

     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Perft Testing                                                                  \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     printf("Perft (Depth %d): %llu\n", 1, divide(1, 0, initialBoard, false));
     printf("Perft (Depth %d): %llu\n", 2, divide(2, 0, initialBoard, false));
     printf("Perft (Depth %d): %llu\n", 3, divide(3, 0, initialBoard, false));
     printf("Perft (Depth %d): %llu\n", 4, divide(4, 0, initialBoard, false));

     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Evaluation Testing                                                             \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     // TODO: Include this to FENboardsetup / Board120Setup
     initialBoard.updatePieceCount();
     printf("Initial Board Evaluation: %d\n", initialBoard.boardEvaluation());

     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" MakeMove / Evaluation Testing                                                  \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");
     
     Board copiedInitBoard;

     for (int i = 0; i < moveList.getCounter(); i++) {
          copiedInitBoard = initialBoard;
          printf("%2d  ", i + 1);
          std::cout << numberToFilerank(moveList.getMove(i).getInitial())
                    << numberToFilerank(moveList.getMove(i).getTerminal());
          int capturedPiece = makeMove(copiedInitBoard, moveList.getMove(i));
          printf("  %4d\n", copiedInitBoard.boardEvaluation());
     }

     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Depth 1 Alphabeta Testing                                                      \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Board savedBoard[MAX_MOVENUMBER];
     int saveIndex = 0;

     Move bestMoves1[MAX_DEPTH];
     int bestMoveScore1 = rootAlphabeta(1, initialBoard, DEFAULT_ALPHA, DEFAULT_BETA, bestMoves1, savedBoard, saveIndex);
     printf("Best Move Score: %3d\n", bestMoveScore1);
     std::cout << "Best Moves: ";
     for (int i = 1; i >= 1; i--) {
          std::cout << numberToFilerank(bestMoves1[i].getInitial())
               << numberToFilerank(bestMoves1[i].getTerminal())
               << " ";
     }
     printf("\n");

     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Depth 2 Alphabeta Testing                                                      \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Move bestMoves2[MAX_DEPTH];
     int bestMoveScore2 = rootAlphabeta(2, initialBoard, DEFAULT_ALPHA, DEFAULT_BETA, bestMoves2, savedBoard, saveIndex);
     printf("Best Move Score: %3d\n", bestMoveScore2);
     std::cout << "Best Moves: ";
     for (int i = 2; i >= 1; i--) {
          std::cout << numberToFilerank(bestMoves2[i].getInitial())
               << numberToFilerank(bestMoves2[i].getTerminal())
               << " ";
     }
     printf("\n");

     
     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Depth 3 Alphabeta Testing                                                      \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Move bestMoves3[MAX_DEPTH];
     int bestMoveScore3 = rootAlphabeta(3, initialBoard, DEFAULT_ALPHA, DEFAULT_BETA, bestMoves3, savedBoard, saveIndex);
     printf("Best Move Score: %3d\n", bestMoveScore3);
     std::cout << "Best Moves: ";
     for (int i = 3; i >= 1; i--) {
          std::cout << numberToFilerank(bestMoves3[i].getInitial())
               << numberToFilerank(bestMoves3[i].getTerminal())
               << " ";
     }
     printf("\n");

     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Depth 4 Alphabeta Testing                                                      \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Move bestMoves4[MAX_DEPTH];
     int bestMoveScore4 = rootAlphabeta(4, initialBoard, DEFAULT_ALPHA, DEFAULT_BETA, bestMoves4, savedBoard, saveIndex);
     printf("Best Move Score: %3d\n", bestMoveScore4);
     std::cout << "Best Moves: ";
     for (int i = 4; i >= 1; i--) {
          std::cout << numberToFilerank(bestMoves4[i].getInitial())
               << numberToFilerank(bestMoves4[i].getTerminal())
               << " ";
     }
     printf("\n");

     /*
     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Depth 5 Alphabeta Testing                                                      \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Move bestMoves5[MAX_DEPTH];
     int bestMoveScore5 = rootAlphabeta(5, initialBoard, DEFAULT_ALPHA, DEFAULT_BETA, bestMoves5, savedBoard, saveIndex);
     printf("Best Move Score: %3d\n", bestMoveScore5);
     std::cout << "Best Moves: ";
     for (int i = 5; i >= 1; i--) {
          std::cout << numberToFilerank(bestMoves5[i].getInitial())
               << numberToFilerank(bestMoves5[i].getTerminal())
               << " ";
     }
     printf("\n");
     */

     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" First 4 Moves with Depth 4                                                     \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     for (int i = 0; i < 4; i++) {
          Move bestMoves[MAX_DEPTH];
          int bestMoveScore = rootAlphabeta(4, initialBoard, DEFAULT_ALPHA, DEFAULT_BETA, bestMoves, savedBoard, saveIndex);
          printf("Best Move Score: %3d\n", bestMoveScore);
          std::cout << "Best Moves: ";
          for (int j = 4; j >= 1; j--) {
               std::cout << numberToFilerank(bestMoves[j].getInitial())
                    << numberToFilerank(bestMoves[j].getTerminal())
                    << " ";
          }
          printf("\n");
          makeMove(initialBoard, bestMoves[4]);
     }

}