from server.app.data.bb_state import BBState
from typing import List

class GameManager:  
    def __init__(self, is_user_white: bool):
        self.bb_state = BBState() # Bitboard lists instanciated within this current instance
        self.is_user_white = is_user_white
        self.whites_turn: bool = True  # white starts always
        self.moves_tracker: List[BBState] = [] # To store the last three BBState

    def __str__(self):
        return "THIS IS THE GAME MANAGER PRINT MODE"

    def change_turn(self):
        self.whites_turn = not self.whites_turn

    # stores the current bb_state in list and changes to new state
    def update_game(self, modified_bb_list: BBState):
        self.moves_tracker.append(self.bb_state)

        if len(self.moves_tracker) > 3:
            self.moves_tracker.pop(0) # Remove the oldest BBState

        self.bb_state = modified_bb_list
