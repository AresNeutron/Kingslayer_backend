#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include "./game/Game.h"
#include "./board_state/BoardState.h"
#include "./search/Search.h"

Game game;
Search search;

enum class Command {
    UCINEWGAME, ENGINEMOVES, GETBOARD, GETMOVES, USERMOVES, PROMOTE, QUIT, UNKNOWN
};

Command obtain_command(const std::string& token) {
    static const std::unordered_map<std::string, Command> command_map = {
        {"ucinewgame", Command::UCINEWGAME},
        {"enginego", Command::ENGINEMOVES},
        {"getboard", Command::GETBOARD},
        {"getmoves", Command::GETMOVES},
        {"promote", Command::PROMOTE},
        {"makemove", Command::USERMOVES},
        {"quit", Command::QUIT}
    };

    auto it = command_map.find(token);
    return it != command_map.end() ? it->second : Command::UNKNOWN;
}

void uci_loop() {
    std::string line;

    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        switch (obtain_command(token)) {
            case Command::UCINEWGAME:
                game = Game();
                search = Search();
                std::cout << "New Game Started\n";
                std::cout << "readyok\n";
                break;

                // disabled by now
            case Command::ENGINEMOVES:{
                search.engine_moves(game);
                break;
            }

            case Command::GETBOARD: {
                game.get_board_state().printBoardArray();
                std::cout << "readyok\n";
                break;
            }

            case Command::GETMOVES: {
                int square;
                iss >> square;
                if (square < 0 || square > 63) {
                    std::cout << "Invalid square\n";
                    std::cout << "error" << std::endl;
                    break;
                }

                std::vector<uint16_t> movesVector = game.get_legal_moves(square);

                for (uint16_t moveCode : movesVector) {
                    std::cout << moveCode << std::endl;
                }

                std::cout << "readyok\n";
                break;
            }

            case Command::USERMOVES: {
                uint16_t move_code;
                iss >> move_code;

                game.user_moves(move_code);
                break;
            }

            case Command::PROMOTE: {
                int promotion;
                iss >> promotion;
                
                game.resolve_promotion(promotion);
                break;
            }


            case Command::QUIT:
                return;

            default:
                std::cout << "Unknown command: " << token << "\n";
                break;
        }
    }
}

int main() {
    init_king_knight_lookups();
    init_pawn_lookups();
    init_ray_tables();
    generate_magic_bitboards();

    uci_loop();
    return 0;
}
