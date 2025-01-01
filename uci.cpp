/******************************************************************************/
/* uci.cpp                                                                    */
/* Seung Jae (Ryan) Lee                                                       */
/******************************************************************************/


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <future>

#include "move.h"
#include "board.h"
#include "evaluation.h"

#include "evaluation.cpp" 
#include "movegen.cpp" 
#include "move.cpp" 
#include "board.cpp" 
#include "timer.cpp"
#include "pgn.cpp"


struct SearchParams {
    int depth;
    int movestogo;
    int whiteTimeMs;
    int blackTimeMs;
    int whiteIncMs;
    int blackIncMs;
};



std::mutex logMutex;
std::atomic<uint64_t> globalNodes{0};

void logInput(std::ostream& ucilog, std::string input) {
    std::lock_guard<std::mutex> lock(logMutex);
    ucilog << "I " << input << std::endl;
    ucilog.flush();
}

void tellUCI(std::ostream& ucilog, std::string output) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::cout << output;
    ucilog << "O " << output;
    ucilog.flush();
}

struct SearchResult {
    int score;
    Variation pv;
    uint64_t nodes;
};

SearchResult parallelSearch(int depth, Board& board, Board* savedBoard, int saveIndex) {
    SearchResult result;
    result.nodes = 0;
    result.score = rootAlphabeta(depth, board, &result.pv, savedBoard, saveIndex);
    globalNodes += result.nodes;
    
    // Output intermediate PV information
    std::ostringstream ss;
    ss << "info depth " << depth 
       << " score cp " << result.score
	   << " time "
       << "nodes " << result.nodes
	   << " nps"
       << " pv ";
    for (int i = 0; i < depth && result.pv.moves[i].getType() != 0; i++) {
        ss << moveToString(result.pv.moves[i]) << " ";
    }
    ss << "\n";
    tellUCI(std::cout, ss.str());
    
    return result;
}


int calculateTimeAllocation(const SearchParams& params, bool isWhite) {
    int baseTime = isWhite ? params.whiteTimeMs : params.blackTimeMs;
    int increment = isWhite ? params.whiteIncMs : params.blackIncMs;
    
    int allocatedTime;
    if (params.movestogo > 0) {
        // If movestogo is specified, divide remaining time accordingly
        allocatedTime = (baseTime / params.movestogo) + increment;
    } else {
        // Default time management
        allocatedTime = baseTime / 160 + increment;
    }
    
    // Ensure reasonable bounds for allocated time
    return std::max(100, std::min(allocatedTime, baseTime / 2));
}



int main() {
    std::ofstream ucilog;
    ucilog.open("uci.log");

    std::string command;
    std::string token;
    std::string moveString;
    command.reserve(4096);
    token.reserve(4096);
    moveString.reserve(4096);
    
    Board board;
    Board savedBoard[MAX_MOVENUMBER];
    int saveIndex = 0;

    int whiteTimeMs = 0;
    int blackTimeMs = 0;
    int whiteIncMs = 0;
    int blackIncMs = 0;

    unsigned int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 2;
    
    
	SearchParams searchParams = {0, 0, 0, 0, 0, 0};
    
    while (1) {
        token.clear();
        std::getline(std::cin, command);
        logInput(ucilog, command);
        std::istringstream is(command);
        is >> std::skipws >> token;

        if (token == "quit") {
            break;
        }
	
        else if (token == "uci") {
            tellUCI(ucilog, "id name Nageune\n");
            tellUCI(ucilog, "id author Seung Jae (Ryan) Lee\n");
            tellUCI(ucilog, "option name Threads type spin default " + 
                   std::to_string(numThreads) + " min 1 max 128\n");
            tellUCI(ucilog, "uciok\n");
        }
        else if (token == "isready") {
            tellUCI(ucilog, "readyok\n");
        }
        
		else if (token == "go") {
            // Reset search parameters
            searchParams = {0, 0, 0, 0, 0, 0};
            
            std::string param;
            while (is >> param) {
                if (param == "depth") {
                    is >> searchParams.depth;
                }
                else if (param == "movestogo") {
                    is >> searchParams.movestogo;
                }
                else if (param == "wtime") {
                    is >> searchParams.whiteTimeMs;						
                }
                else if (param == "btime") {
                    is >> searchParams.blackTimeMs;
                }
                else if (param == "winc") {
                    is >> searchParams.whiteIncMs;
                }
                else if (param == "binc") {
                    is >> searchParams.blackIncMs;
                }
            }
 
      

            int allocatedTime = calculateTimeAllocation(searchParams, board.getTurn() == WHITE);
            Timer timer;
            timer.start();
			
			
            
            std::vector<std::future<SearchResult>> futures;
            Variation bestPV;
            int bestScore = -999999;
            globalNodes = 0;
            
            // Determine maximum search depth
            int maxDepth = (searchParams.depth > 0) ? searchParams.depth : 10;
            
            for (int depth = 1; depth <= maxDepth; depth++) {
                if (timer.elapsed() >= allocatedTime && searchParams.depth == 0) break;

                std::vector<Board> boardCopies(numThreads, board);
                futures.clear();
                
                for (unsigned int i = 0; i < numThreads; i++) {
                    futures.push_back(std::async(std::launch::async, 
                        parallelSearch, depth, std::ref(boardCopies[i]), 
                        savedBoard, saveIndex));
                }

                uint64_t totalNodes = 0;
                for (auto& future : futures) {
                    SearchResult result = future.get();
                    totalNodes += result.nodes;
                    if (result.score > bestScore) {
                        bestScore = result.score;
                        bestPV = result.pv;
                    }
                }
                
                // If we're searching to a fixed depth, stop when we reach it
                if (searchParams.depth > 0 && depth >= searchParams.depth) {
                    break;
                }
            }
			
		
            	
            tellUCI(ucilog, "bestmove " + moveToString(bestPV.moves[0]) + "\n");
        }

        else if (token == "ucinewgame") {
            board.setup();
            whiteTimeMs = blackTimeMs = whiteIncMs = blackIncMs = 0;
        }
        else if (token == "position") {
               std::string option;
               is >> std::skipws >> option;
               
               // Reset saveIndex when starting new position
               saveIndex = 0;
               
               // Find Starting position
               if (option == "startpos") {
                    board.setup();
               }
               else if (option == "fen") {
                    board.import(option);
               }
               else { return 0; }

               // Check if there are any moves given
               is >> std::skipws >> option;
               if (option == "moves") {
                    Move move;
                    while (is >> moveString) {
                         // Check for buffer overflow
                         if (saveIndex >= MAX_MOVENUMBER) {
                              tellUCI(ucilog, "info string Error: Move buffer overflow\n");
                              return 1;
                         }

                         // Save Board
                         savedBoard[saveIndex] = board;
                         saveIndex++;

                         // Interpret String
                         move.setInitial(filerankToNumber(moveString[0], moveString[1] - '0'));
                         move.setTerminal(filerankToNumber(moveString[2], moveString[3] - '0'));

                         if (moveString.length() == 4) {
                              move.setType(NORMAL);
                         }
                         else if (moveString.length() == 5) {
                              switch(moveString[4]) {
                                   case 'n': move.setType(KNIGHT_PROMOTION); break;
                                   case 'b': move.setType(BISHOP_PROMOTION); break;
                                   case 'r': move.setType(ROOK_PROMOTION); break;
                                   case 'q': move.setType(QUEEN_PROMOTION); break;
                                   default: return 0;
                              }
                         }
                         else { return 0; }

                         // Make move on the board
                         board.makeMove(move);
                    }
               }
          }
		  
		 

        else if (token == "debug") {}
        else if (token == "setoption") {}
        else if (token == "register") {}
        else if (token == "stop") {}
        else if (token == "ponderhit") {}
        else if (token == "printfen") {
            std::cout << board.fen() << std::endl;
        }
    }

    return 0;
}
