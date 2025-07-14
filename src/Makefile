CXX = g++
CXXFLAGS = -g -std=c++20 -O2

# Lista de archivos fuente (excluyendo magic_number_generator.cpp)
SRCS = \
    uci.cpp \
    board_state/getters.cpp \
    board_state/setters.cpp \
    game/getters.cpp \
    game/setters.cpp \
    game/events.cpp \
    game/game_flow.cpp \
    game/special_methods.cpp \
    constants/rays.cpp \
    search/search.cpp \
    precomputed_moves/non_sliding_moves/king_knight.cpp \
    precomputed_moves/non_sliding_moves/pawn.cpp \
    precomputed_moves/sliding_moves/masks_blockers.cpp \
    precomputed_moves/sliding_moves/rook_bishop.cpp

OBJS = $(SRCS:.cpp=.o)

test: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) test
