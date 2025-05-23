import math
from server.app.alpha_beta_prune.evaluate_state import evaluate_state
from server.app.alpha_beta_prune.get_moves_for_color import get_moves_for_color
from server.app.helpers.move_makers import make_move
from server.app.data.bb_state import BBState

def alpha_beta(bb_state: BBState, depth: int, alpha: float, beta: float, is_white: bool) -> float:
    """
    Implementación de la poda alpha-beta con bitboards.
    :param bb_state: Estado actual del tablero (bitboards y metadatos).
    :param depth: Profundidad restante.
    :param alpha: Cota inferior para poda.
    :param beta: Cota superior para poda.
    :param is_white: True si es turno del jugador blanco (maximizador).
    :return: Valor de evaluación de la posición.
    """
    # Caso base: evaluamos la posición
    if depth == 0:
        return evaluate_state(bb_state)

    if is_white:
        value = -math.inf
        # Generamos y recorremos movimientos del maximizador (blancas)
        for move_code in get_moves_for_color(bb_state, True):
            # Decodificar movimiento
            from_sq = move_code >> 6
            to_sq = move_code & 0b111111

            # Simulamos el movimiento sin afectar el historial principal
            child_state = bb_state.copy()
            child_state = make_move(child_state, from_sq, to_sq)

            # Llamada recursiva: ahora es turno del minimizador (negro)
            score = alpha_beta(child_state, depth - 1, alpha, beta, False)
            value = max(value, score)
            alpha = max(alpha, value)

            # Poda beta
            if beta <= alpha:
                break

        return value

    else:
        # Turno del minimizador (negras)
        value = math.inf
        for move_code in get_moves_for_color(bb_state, False):
            from_sq = move_code >> 6
            to_sq = move_code & 0b111111

            child_state = bb_state.copy()
            child_state = make_move(child_state, from_sq, to_sq)

            # Siguiente turno: maximizador (blancas)
            score = alpha_beta(child_state, depth - 1, alpha, beta, True)
            value = min(value, score)
            beta = min(beta, value)

            # Poda alpha
            if beta <= alpha:
                break

        return value
    

def find_best_move(bb_state: BBState, depth: int, is_white: bool) -> int:
    """
    Función para obtener la mejor jugada desde la posición inicial.
    """
    best_move = None
    best_value = -math.inf
    alpha = -math.inf
    beta = math.inf

    for move_code in get_moves_for_color(bb_state, is_white):
        from_sq = move_code >> 6
        to_sq = move_code & 0b111111

        child_state = bb_state.copy()
        child_state = make_move(child_state, from_sq, to_sq)

        score = alpha_beta(child_state, depth - 1, alpha, beta, not is_white)

        if score > best_value:
            best_value = score
            best_move = move_code
        alpha = max(alpha, best_value)

    return best_move