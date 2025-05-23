def generate_rook_mask(square):
    """Genera la máscara de bloqueadores para una torre en 'square', excluyendo los bordes."""
    rank, file = divmod(square, 8)
    mask = 0

    # Agregar casillas en la misma fila (excluyendo los bordes)
    for f in range(1, 7):  # Itera sobre la fila, excluyendo los bordes
        if f != file:  # Evita la propia posición de la torre
            mask |= (1 << (rank * 8 + f))

    # Agregar casillas en la misma columna (excluyendo los bordes)
    for r in range(1, 7):  # Itera sobre la columna, excluyendo los bordes
        if r != rank:  # Evita la propia posición de la torre
            mask |= (1 << (r * 8 + file))

    return mask

def generate_bishop_mask(square):
    """Genera la máscara de bloqueadores para un alfil en 'square', excluyendo los bordes."""
    rank, file = divmod(square, 8)
    mask = 0

    # Diagonal ↘↖ (hacia la derecha-abajo y la izquierda-arriba, excluyendo los bordes)
    for i in range(1, min(7 - file, 7 - rank)):  # Hacia la esquina inferior derecha, excluyendo el borde
        mask |= (1 << ((rank + i) * 8 + (file + i)))
    for i in range(1, min(file, rank)):  # Hacia la esquina superior izquierda, excluyendo el borde
        mask |= (1 << ((rank - i) * 8 + (file - i)))

    # Diagonal ↙↗ (hacia la izquierda-abajo y la derecha-arriba, excluyendo los bordes)
    for i in range(1, min(file, 7 - rank)):  # Hacia la esquina inferior izquierda, excluyendo el borde
        mask |= (1 << ((rank + i) * 8 + (file - i)))
    for i in range(1, min(7 - file, rank)):  # Hacia la esquina superior derecha, excluyendo el borde
        mask |= (1 << ((rank - i) * 8 + (file + i)))

    return mask
