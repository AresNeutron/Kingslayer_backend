from server.app.data.game_manager import GameManager
from server.app.helpers.move_makers import make_move
from server.app.main.event_detection import is_promotion, is_check, is_game_over
from server.app.alpha_beta_prune.negamax import find_best_move
from typing import Tuple


def user_move(game_manager: GameManager, data: dict) -> Tuple[GameManager, dict]:
    """Procesar movimientos de piezas recibidos por WebSocket."""
    bb_state = game_manager.bb_state
    is_user_white = game_manager.is_user_white

    from_sq: int = data.get("from_sq")
    to_sq: int = data.get("to_sq")

    # Make the move and save
    new_bb_state = make_move(bb_state, from_sq, to_sq)
    game_manager.update_game(new_bb_state) # Here is where we store the changes in board state
    
    # Pasamos el turno al motor
    game_manager.change_turn()
    
    # ok, en la función del usuario verificamos primero si es fin del juego para el motor, 
    # y después si hay alguna promoción, en ambos casos se devuelve el evento
    game_over_detection = is_game_over(bb_state, not is_user_white)
    if game_over_detection != "none":
        return (game_manager ,{"event": game_over_detection, "user_wins": True})

    promotion = is_promotion(bb_state, is_user_white)
    if promotion is not None:
        return (game_manager, {"event": "promotion_required", "square": promotion})

    return (game_manager, {"event": "user_move_made"})
    

def engine_move(game_manager: GameManager) -> Tuple[GameManager, dict]:
    is_engine_white = not game_manager.is_user_white

    # Llamas a tu driver (buscar_mejor_jugada)
    best_move = find_best_move(game_manager.bb_state, 3, is_engine_white)
    
    # best_move viene empaquetado en 12 bits
    from_sq = best_move >> 6
    to_sq   = best_move & 0b111111

    # Aplica el movimiento en modo normal
    new_bb_state = make_move(game_manager.bb_state, from_sq, to_sq)

    # In case of promotion
    promotion_sq = is_promotion(new_bb_state, is_engine_white)
    if promotion_sq is not None:
        # Promotion to queen by defalt, update this later
        new_bb_state.promote_pawn(promotion_sq, f"{"white" if is_engine_white else "black"}_queen")

    game_manager.update_game(new_bb_state)

    # Pasamos el turno de nuevo al usuario
    game_manager.change_turn()

    # En el caso de la función del motor, detectamos si es fin del juego para el usuario, 
    # y luego si es jaque para el usuario, y en ambos casos se devuelve el evento
    game_over_detection = is_game_over(new_bb_state, not is_engine_white)
    if game_over_detection != "none":
        return (game_manager, {"event": game_over_detection, "user_wins": False})
    
    threats, king_bb = is_check(new_bb_state, not is_engine_white)
    if bool(threats):
        return (game_manager, {"event": "check", "threats": threats | king_bb})
    
    return (game_manager, {"event": "engine_move_made"})