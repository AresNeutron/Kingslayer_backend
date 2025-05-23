def generate_blocker_combinations(mask) -> list:
    """Genera todas las combinaciones posibles de bloqueadores dentro de la máscara."""
    combinations = []
    blocker_positions = [bit_pos for bit_pos in range(64) if (mask >> bit_pos) & 1]  # Casillas con posibles bloqueadores
    num_blockers = len(blocker_positions)  # Número total de bits relevantes en la máscara

    # Iterar sobre todas las combinaciones posibles (desde 0 hasta 2^num_blockers - 1)
    for i in range(1 << num_blockers):
        combination = 0
        for j in range(num_blockers):
            if (i >> j) & 1:  # Si el bit 'j' está activado en la combinación actual
                combination |= (1 << blocker_positions[j])
        combinations.append(combination)

    return combinations

