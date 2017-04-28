/******************************************************************************/
/* debugmain.cpp                                                              */
/* Seung Jae (Ryan) Lee                                                       */
/******************************************************************************/

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <chrono>

#include "timer.h"
#include "board.h"
#include "movegen.h"
#include "evaluation.h"
#include "pgn.h"
#include "debug.h"

typedef std::chrono::high_resolution_clock Clock;

// FIXME: Global variables for clarity?
int passCount = 0;
int failCount = 0;

// FIXME: Add Documentation
void REQUIRE(std::string description, bool condition) {
     if (condition) { std::cout << "   SUCCESS   "; passCount++; }
     else           { std::cout << "   FAILURE   "; failCount++; }
     std::cout << description << std::endl;
}

// FIXME: Add Documentation
void test_BoardClass() {

     std::cout << "BOARD CLASS" << std::endl;

     Board board;

     // Test 1
     board.setup();
     REQUIRE("Board::setup() followed by Board::fen() should give the correct initial FEN",
             board.fen() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

     // Test 2
     board.import("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
     REQUIRE("Board::fen() should return FEN string from Board::import()",
          board.fen() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

     // Test 3
     board.setup();
     REQUIRE("Board::evaluate() should return 0 in initial board",
          board.evaluate() == 0);
}

void test_Perft() {

     std::cout << "PERFT" << std::endl;
     Board board;
     
     // Test 1
     board.setup();
     REQUIRE("Perft of depth 1 should give 20 moves",
             divide(1, 0, board, false) == 20);

     // Test 2
     board.setup();
     REQUIRE("Perft of depth 2 should give 400 moves",
             divide(2, 0, board, false) == 400);

     // Test 3
     board.setup();
     REQUIRE("Perft of depth 3 should give 8902 moves",
             divide(3, 0, board, false) == 8902);

     // Test 4
     board.setup();
     REQUIRE("Perft of depth 4 should give 197281 moves",
             divide(4, 0, board, false) == 197281);

     // Test 5
     board.setup();
     REQUIRE("Perft of depth 5 should give 4865609 moves",
             divide(5, 0, board, false) == 4865609);
}

int main() {
     auto testStart = Clock::now();

     Timer programTimer;
     programTimer.start();
     
     std::cout << "********************************************************************************" << std::endl;
     std::cout << "*                                                                              *" << std::endl;
     std::cout << "*                            DOUMI CHESS UNIT TESTS                            *" << std::endl;
     std::cout << "*                                                                              *" << std::endl;
     std::cout << "********************************************************************************" << std::endl << std::endl;

     test_BoardClass();
     test_Perft();

     // FIXME: Add MoveGen Test: check equality of elements in array
     // FIXME: Add MakeMove Test
     // FIXME: Add UndoMove Test
     // FIXME: Add Alphabeta Test (check unmodified)
     // FIXME: Add GameState Test
     // FIXME: Add Mate in X "Test"

     Board initialBoard;
     initialBoard.setup();

     /*
     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" MakeMove / Evaluation Test                                                     \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");


     Board copiedInitBoard;

     for (int i = 0; i < moveList.getCounter(); i++) {
          copiedInitBoard = initialBoard;
          printf("%2d  ", i + 1);
          std::cout << numberToFilerank(moveList.getMove(i).getInitial())
               << numberToFilerank(moveList.getMove(i).getTerminal());
          int capturedPiece = copiedInitBoard.makeMove(moveList.getMove(i));
          printf("  %4d\n", copiedInitBoard.evaluate());
     }

     
     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Depth 1 Alphabeta Test                                                         \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Board savedBoard[MAX_MOVENUMBER];
     int saveIndex = 0;

     Variation bestMoves1;
     int bestMoveScore1 = rootAlphabeta(1, initialBoard, &bestMoves1, savedBoard, saveIndex);
     printf("Best Move Score: %3d\n", bestMoveScore1);
     std::cout << "Best Moves: ";
     printVariation(std::cout, bestMoves1);


     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Depth 2 Alphabeta Test                                                         \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Variation bestMoves2;
     int bestMoveScore2 = rootAlphabeta(2, initialBoard, &bestMoves2, savedBoard, saveIndex);
     printf("Best Move Score: %3d\n", bestMoveScore2);
     std::cout << "Best Moves: ";
     printVariation(std::cout, bestMoves2);


     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Depth 3 Alphabeta Test                                                         \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Variation bestMoves3;
     int bestMoveScore3 = rootAlphabeta(3, initialBoard, &bestMoves3, savedBoard, saveIndex);
     printf("Best Move Score: %3d\n", bestMoveScore3);
     std::cout << "Best Moves: ";
     printVariation(std::cout, bestMoves3);

     
     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Depth 4 Alphabeta Test                                                         \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Variation bestMoves4;
     int bestMoveScore4 = rootAlphabeta(4, initialBoard, &bestMoves4, savedBoard, saveIndex);
     printf("Best Move Score: %3d\n", bestMoveScore4);
     std::cout << "Best Moves: ";
     printVariation(std::cout, bestMoves4);

     
     
     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Depth 5 Alphabeta Test                                                         \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     LINE bestMoves5;
     int bestMoveScore5 = rootAlphabeta(5, initialBoard, &bestMoves5, savedBoard, saveIndex);
     printf("Best Move Score: %3d\n", bestMoveScore5);
     std::cout << "Best Moves: ";
     for (int i = 0; i < 4; i++) {
          std::cout << numberToFilerank(bestMoves5.argmove[i].getInitial())
               << numberToFilerank(bestMoves5.argmove[i].getTerminal())
               << " ";
     }
     printf("\n");

     
     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" First 4 Moves with Depth 4 Test                                                \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     for (int i = 0; i < 4; i++) {
          LINE bestMoves;
          int bestMoveScore = rootAlphabeta(4, initialBoard, &bestMoves, savedBoard, saveIndex);
          printf("Best Move Score: %3d\n", bestMoveScore);
          std::cout << "Best Moves: ";
          for (int j = 0; j < 4; j++) {
               std::cout << numberToFilerank(bestMoves.argmove[j].getInitial())
                    << numberToFilerank(bestMoves.argmove[j].getTerminal())
                    << " ";
          }
          printf("\n");
          initialBoard.makeMove(bestMoves[4]);
     }
        


     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Stalemate Detection Test 1: No Legal Move                                      \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Board stalemateboard1;
     Board savedBoard2[MAX_MOVENUMBER];
     int saveIndex2 = 0;
     stalemateboard1.import("6bk/5p1p/5P1P/8/8/8/8/4K3 b - - 0 1");
     stalemateboard1.print();

     gameState stalemate1 = checkGameState(stalemateboard1, savedBoard2, saveIndex2);
     if (stalemate1 == STALEMATE_MOVE) {
          printf("\nCorrect stalemate detected.\n");
     }
     else {
          printf("\nStalemate test 1 failed!\n");
     }



     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Stalemate Detection Test 2: 50 Move Rule                                       \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Board stalemateboard2;
     stalemateboard2.import("7k/pppppppp/8/8/8/8/PPPPPPPP/K7 w - - 100 1");
     stalemateboard2.print();

     gameState stalemate2 = checkGameState(stalemateboard2, savedBoard2, saveIndex2);
     if (stalemate2 == STALEMATE_50) {
          printf("\nCorrect stalemate detected.\n");
     }
     else {
          printf("\nStalemate test 2 failed!\n");
     }



     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Stalemate Detection Test 3: 75 Nove Rule                                       \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Board stalemateboard3;
     stalemateboard3.import("7k/pppppppp/8/8/8/8/PPPPPPPP/K7 w - - 150 1");
     stalemateboard2.print();

     gameState stalemate3 = checkGameState(stalemateboard3, savedBoard2, saveIndex2);
     if (stalemate3 == STALEMATE_75) {
          printf("\nCorrect stalemate detected.\n");
     }
     else {
          printf("\nStalemate test 3 failed!\n");
     }



     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Stalemate Detection Test 4: Threefold Repetition                               \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Board stalemateboard4;
     Board savedBoard3[MAX_MOVENUMBER];
     int saveIndex3 = 3;
     stalemateboard4.import("7k/pppppppp/8/8/8/8/PPPPPPPP/K7 w - - 0 1");

     savedBoard3[0] = stalemateboard4;
     savedBoard3[1] = stalemateboard4;
     savedBoard3[2] = stalemateboard4;

     gameState stalemate4 = checkGameState(stalemateboard4, savedBoard3, saveIndex3);
     if (stalemate4 == STALEMATE_3F) {
          printf("Correct stalemate detected.\n");
     }
     else {
          printf("Stalemate test 4 failed!\n");
     }



     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Checkmate Detection Test                                                       \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Board checkmateboard;
     Board savedBoard4[MAX_MOVENUMBER];
     int saveIndex4 = 0;
     checkmateboard.import("R6k/R7/8/8/8/8/8/7K b - - 0 1");

     gameState checkmate = checkGameState(checkmateboard, savedBoard4, saveIndex4);
     if (checkmate == WHITE_CHECKMATE) {
          printf("Correct stalemate detected.\n");
     }
     else {
          printf("Checkmate test failed!\n");
     }

     

     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Mate in 1 Test                                                                 \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Board matein1Board;
     Board savedBoard5[MAX_MOVENUMBER];
     int saveIndex5 = 0;
     matein1Board.import("7k/RR6/8/8/8/8/8/7K w - - 0 1");

     Variation matein1Move;
     int matein1Score = rootAlphabeta(4, matein1Board, &matein1Move, savedBoard5, saveIndex5);
     printf("Best Move Score: %3d\n", matein1Score);
     std::cout << "Best Moves: ";
     printVariation(std::cout, matein1Move);

     

     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Mate in 2 Test                                                                 \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Board matein2Board;
     matein2Board.import("7k/8/RR6/8/8/8/8/7K w - - 0 1");

     Variation matein2Move;
     int matein2Score = rootAlphabeta(4, matein2Board, &matein2Move, savedBoard5, saveIndex5);
     printf("Best Move Score: %3d\n", matein2Score);
     std::cout << "Best Moves: ";
     printVariation(std::cout, matein2Move);

     

     printf("\n");
     printf("--------------------------------------------------------------------------------\n");
     printf(" Forced Stalemate in Unfavorable Position Test                                  \n");
     printf("--------------------------------------------------------------------------------\n");
     printf("\n");

     Board forcedStalemateBoard;
     forcedStalemateBoard.import("5k2/r7/4PKP1/4PPP1/8/8/8/N3NN2 b - - 0 1");
     //forcedStalemateBoard.import("5k2/r7/4PKP1/4PPP1/8/8/8/4QQQ1 b - - 0 1");
     printf("Board Evaluation: %d\n", forcedStalemateBoard.evaluate());

     forcedStalemateBoard.print();

     Variation forcedStalemateMoves;
     int forcedStalemateScore = rootAlphabeta(4, forcedStalemateBoard, &forcedStalemateMoves, savedBoard5, saveIndex5);
     printf("Best Move Score: %3d\n", forcedStalemateScore);
     std::cout << "Best Moves: ";
     printVariation(std::cout, forcedStalemateMoves);

     */

     auto testEnd = Clock::now();
     std::cout << std::endl;
     std::cout << "********************************************************************************" << std::endl;
     std::cout << "*                                                                              *" << std::endl;
     std::cout << "*                              UNIT TESTS SUMMARY                              *" << std::endl;
     std::cout << "*                                                                              *" << std::endl;
     std::cout << "********************************************************************************" << std::endl << std::endl;
     
     //auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(testEnd-testStart).count();

     programTimer.stop();
     uint64_t duration = programTimer.duration_milli();
     std::cout << "  Test Duration: " << duration << " ms." << std::endl;
     std::cout << "  Passed tests : " << passCount << std::endl;
     std::cout << "  Failed tests : " << failCount << std::endl;

     std::cout << std::endl;

     return 0;
}