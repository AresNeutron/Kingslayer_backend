import math
from server.app.alpha_beta_prune.evaluate_state import evaluate_state
from server.app.alpha_beta_prune.get_moves_for_color import get_moves_for_color
from server.app.helpers.move_makers import make_move
from server.app.data.bb_state import BBState

# La función Negamax. 'color_multiplier' es 1 para Blancas, -1 para Negras.
# Siempre devuelve la puntuación desde la perspectiva del *jugador actual* en esta llamada.
def negamax(bb_state: BBState, depth: int, alpha: float, beta: float, color_multiplier: int) -> float:
    """
    Implementación de Negamax con poda alfa-beta.
    Retorna la puntuación de la posición desde la perspectiva del jugador actual (color_multiplier).
    """
    if depth == 0:
        # evaluate_state devuelve la puntuación desde la perspectiva de Blancas.
        # Multiplicamos por color_multiplier para obtener la perspectiva del jugador actual.
        return color_multiplier * evaluate_state(bb_state)

    # El jugador actual (dado por color_multiplier) es el maximizador
    value = -math.inf
    
    # Obtener movimientos para el jugador actual
    current_player_is_white = color_multiplier > 0
    moves = get_moves_for_color(bb_state, current_player_is_white)

    for move_code in moves:
        child_state = bb_state.copy()
        child_state = make_move(child_state, move_code >> 6, move_code & 0b111111)

        # Llamada recursiva para el oponente.
        # Negamos el resultado y invertimos alpha/beta para cambiar la perspectiva.
        score = -negamax(child_state, depth - 1, -beta, -alpha, -color_multiplier)

        value = max(value, score)
        alpha = max(alpha, value) # Actualizamos alpha para la poda

        # Poda beta (si alpha >= beta, podemos podar la rama)
        if alpha >= beta:
            break

    return value


def find_best_move(bb_state: BBState, depth: int, is_engine_white: bool) -> int:
    """
    Función para obtener la mejor jugada desde la posición inicial para el motor.
    Encuentra el movimiento que maximiza la puntuación desde la perspectiva del motor.
    """
    best_move = None
    best_score = -math.inf # El motor siempre busca maximizar su propia puntuación

    # Los valores iniciales de alpha y beta para la llamada a la raíz son -inf y +inf
    # desde la perspectiva del jugador de la raíz (el motor).
    alpha = -math.inf
    beta = math.inf

    engine_color_multiplier = 1 if is_engine_white else -1

    engine_moves = get_moves_for_color(bb_state, is_engine_white)

    for move_code in engine_moves:
        from_sq = move_code >> 6
        to_sq = move_code & 0b111111

        child_state = bb_state.copy()
        child_state = make_move(child_state, from_sq, to_sq)

        opponent_color_multiplier = -engine_color_multiplier
        
        # Llama a negamax con los bounds invertidos y negados, y el multiplicador del oponente.
        # El motor quiere maximizar - (puntuación del oponente).
        opponent_score = negamax(child_state, depth - 1, -beta, -alpha, opponent_color_multiplier)
        
        # Convertir la puntuación del oponente a la perspectiva del motor (negándola)
        engine_score = -opponent_score

        # El motor busca el movimiento que maximiza su propio score
        if engine_score > best_score:
            best_score = engine_score
            best_move = move_code

    return best_move