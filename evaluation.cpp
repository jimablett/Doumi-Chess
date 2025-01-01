/******************************************************************************/
/* evaluation.cpp                                                             */
/* Seung Jae (Ryan) Lee                                                       */
/******************************************************************************/

#include "evaluation.h"
#include "movegen.h"
#include <fstream>
#include <string>

// Principal Variation:
// https://chessprogramming.wikispaces.com/Principal+Variation


int alphabeta(const int depth, Board& board, int alpha, int beta, Variation* pVariation, 
              Board savedBoard[MAX_MOVENUMBER], int saveIndex) {
    // Early game state check
    int gameState = checkGameState(board, savedBoard, saveIndex);
    if (gameState != NOTMATE) {
        pVariation->length = 0;
        switch (gameState) {
            case WHITE_CHECKMATE:
                return -1 * (MATE_VALUE + depth);
            case BLACK_CHECKMATE:
                return (MATE_VALUE + depth);
            case STALEMATE_3F: 
            case STALEMATE_50:
                if (board.getTurn() * board.evaluate() <= STALEMATE_BOUND) {
                    return 0;
                }
                break;
            case STALEMATE_75: 
            case STALEMATE_MOVE:
                return 0;
        }
    }

    // Leaf node evaluation
    if (depth == 0) {
        pVariation->length = 0;
        return board.getTurn() * board.evaluate();
    }

    // Move generation and search
    MoveList moveList = moveGeneration(board);
    const Board originalPosition = board;  // Store initial position
    Variation bestLine;
    int moveCount = moveList.getCounter();

    // Principal Variation Search optimization
    bool searchPV = true;

    for (int i = 0; i < moveCount; i++) {
        const Move& currentMove = moveList.getMove(i);
        board.makeMove(currentMove);
        savedBoard[saveIndex] = board;

        int score;
        Variation currentLine;

        // PVS: Full window search for first move, null window for others
        if (searchPV) {
            score = -alphabeta(depth - 1, board, -beta, -alpha, &currentLine, 
                             savedBoard, saveIndex + 1);
        } else {
            // Null window search
            score = -alphabeta(depth - 1, board, -alpha - 1, -alpha, &currentLine, 
                             savedBoard, saveIndex + 1);
            if (score > alpha && score < beta) {
                // Re-search with full window if the move might be better
                score = -alphabeta(depth - 1, board, -beta, -alpha, &currentLine, 
                                 savedBoard, saveIndex + 1);
            }
        }

        board = originalPosition;  // Restore position

        if (score >= beta) {
            return beta;  // Beta cutoff
        }

        if (score > alpha) {
            alpha = score;
            searchPV = false;
            
            // Update principal variation
            pVariation->moves[0] = currentMove;
            std::copy(currentLine.moves, currentLine.moves + currentLine.length, 
                     pVariation->moves + 1);
            pVariation->length = currentLine.length + 1;
        }
    }

    return alpha;
}


int rootAlphabeta(const int maxDepth, Board board, Variation* principalVariation, Board savedBoard[MAX_MOVENUMBER], int saveIndex) {
    MoveList moveList = moveGeneration(board);
    if (moveList.getCounter() == 0) {
        principalVariation->length = 0;
        return DEFAULT_ALPHA;  // or appropriate score for no moves available
    }

    const Board originalBoard = board;  // Store initial board state
    Variation localVariation;
    int alpha = DEFAULT_ALPHA;
    const int beta = DEFAULT_BETA;

    principalVariation->length = 0;
    localVariation.length = 0;

    // Pre-allocate move buffer for principal variation
    Move* const pvMoves = principalVariation->moves;
    const Move* const variationMoves = localVariation.moves;

    for (int moveIndex = 0; moveIndex < moveList.getCounter(); ++moveIndex) {
        const Move currentMove = moveList.getMove(moveIndex);
        board.makeMove(currentMove);
        
        savedBoard[saveIndex] = board;
        const int score = -alphabeta(maxDepth - 1, board, -beta, -alpha, &localVariation, 
                                   savedBoard, saveIndex + 1);
        board = originalBoard;

        if (score > alpha) {
            alpha = score;
            pvMoves[0] = currentMove;
            
            // Optimize memory copy using direct assignment
            const int varLength = localVariation.length;
            for (int j = 0; j < varLength; ++j) {
                pvMoves[j + 1] = variationMoves[j];
            }
            principalVariation->length = varLength + 1;
        }
    }
    
    return alpha;
}


gameState checkGameState(Board& board, const Board savedBoard[MAX_MOVENUMBER], int saveIndex) {
     // Stalemate: 75 Move Rule
     if (board.getHalfMoveClock() >= 150) {
          return STALEMATE_75;
     }

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
     MoveList moveList = moveGeneration(board);
     // Checkmate
     if (moveList.getCounter() == 0 && board.isAttacked(kingPos)) {
          if (board.getSquare(kingPos) == WHITE) {
               return BLACK_CHECKMATE;
          }
          else {
               return WHITE_CHECKMATE;
          }
     }
     // Stalemate: No Legal Move
     if (moveList.getCounter() == 0) {
          return STALEMATE_MOVE;
     }

     // Stalemate: 50 Move Rule
     if (board.fiftyMoveCheck()) {
          return STALEMATE_50;
     }

     // Stalemate: Threefold Repetition
     int repetitionCount = 0;
     for (int i = 0; i < saveIndex; i++) {
          if (board.isEqual_3F(savedBoard[i])) {
               repetitionCount++;
          }
          if (repetitionCount >= 3) {
               return STALEMATE_3F;
          }
     }

     return NOTMATE;
}

void printVariation(std::ostream& stream, Variation& var) {
     for (int i = 0; i < var.length; i++) {
          stream << numberToFilerank(var.moves[i].getInitial())
               << numberToFilerank(var.moves[i].getTerminal())
               << " ";
     }
     stream << std::endl;
}