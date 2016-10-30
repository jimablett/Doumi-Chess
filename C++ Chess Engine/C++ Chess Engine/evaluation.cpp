#include "evaluation.h"
#include "movegen.h"





int alphabeta(const int depth, Board& board, int alpha, int beta, LINE* pline, Board savedBoard[MAX_MOVENUMBER], int saveIndex) {

     LINE line;

     // TODO: pline->cmove = 0 for these cases?
     switch (checkGameState(board, savedBoard, saveIndex)) {
     case NOTMATE:
          break;
     case WHITE_CHECKMATE:
          return -1 * (MATE_VALUE + depth);
          break;
     case BLACK_CHECKMATE:
          return (MATE_VALUE + depth);
          break;
     case STALEMATE_3F: case STALEMATE_50:
          if (board.getTurn() * board.boardEvaluation() <= STALEMATE_BOUND) {
               return 0;
          }
          break;
     case STALEMATE_75: case STALEMATE_MOVE:
          return 0;
          break;
     }

     if (depth == 0) {
          pline->cmove = 0;
          return board.getTurn() * board.boardEvaluation();
     }

     Board oldBoard = board;
     MoveList moveList = moveGeneration(board);
     for (int i = 0; i < moveList.getCounter(); i++) {
          int capturedPiece = makeMove(board, moveList.getMove(i));

          savedBoard[saveIndex] = board;

          int score = -alphabeta(depth - 1, board, -beta, -alpha, &line, savedBoard, saveIndex + 1);

          if (score >= beta) {
               board = oldBoard;
               return beta;
          }

          if (score > alpha) {
               alpha = score;
               pline->argmove[0] = moveList.getMove(i); 
               for (int j = 0; j < line.cmove; j++) {
                    pline->argmove[j + 1] = line.argmove[j];
               }
               pline->cmove = line.cmove + 1;
          }
          board = oldBoard;
     }

     return alpha;
}
int rootAlphabeta(const int maxDepth, Board board, int alpha, int beta, LINE* pline, Board savedBoard[MAX_MOVENUMBER], int saveIndex) {
     int score;
     int capturedPiece;

     LINE line;
     Board oldBoard = board;
     MoveList moveList = moveGeneration(board);


     for (int i = 0; i < moveList.getCounter(); i++) {
          capturedPiece = makeMove(board, moveList.getMove(i));
          savedBoard[saveIndex] = board;

          score = -alphabeta(maxDepth - 1, board, -beta, -alpha, &line, savedBoard, saveIndex + 1);

          // TODO: Check if this is needed and change it
          if (score >= beta) {
               board = oldBoard;
               return beta;
          }

          if (score > alpha) {
               alpha = score;
               pline->argmove[0] = moveList.getMove(i);
               for (int j = 0; j < line.cmove; j++) {
                    pline->argmove[j + 1] = line.argmove[j];
               }
               //memcpy(pline->argmove + 1, line.argmove, line.cmove * sizeof(Move));
               pline->cmove = line.cmove + 1;
          }
          board = oldBoard;
     }

     return alpha;
}
gameState checkGameState(Board& board, const Board savedBoard[MAX_MOVENUMBER], int saveIndex) {
     int kingPos = -1;
     for (int i = 0; i < 120; i++) {
          if (board.getSquare(i) == WHITEKING && board.getTurn() == WHITE) {
               kingPos = i;
               break;
          }
          if (board.getSquare(i) == BLACKKING && board.getTurn() == BLACK) {
               kingPos = i;
               break;
          }
     }

     // Stalemate: 75 Move Rule
     // TODO: 50 Move rule will be implemented in moveGen
     if (board.getHalfMoveClock() >= 150) {
          return STALEMATE_75;
     }

     MoveList moveList = moveGeneration(board);

     // Checkmate
     if (moveList.getCounter() == 0 && isSquareAttacked(board, kingPos)) {
          if (board.getSquare(kingPos) == WHITE) {
               return BLACK_CHECKMATE;
          }
          else {
               return WHITE_CHECKMATE;
          }
     }

     // Stalemate: No legal move
     if (moveList.getCounter() == 0) {
          return STALEMATE_MOVE;
     }

     // Check 50 move rule
     if (fiftyMoveCheck(board)) {
          return STALEMATE_50;
     }

     // Check Threefold Repetition
     // TODO: Pass savedBoard + saveIndex as arguments
     int repetitionCount = 0;
     for (int i = 0; i < saveIndex; i++) {
          if (board.isAlmostEqual(savedBoard[i])) {
               repetitionCount++;
          }
          if (repetitionCount >= 3) {
               return STALEMATE_3F;
          }
     }

     return NOTMATE;
}