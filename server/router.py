from asyncio import Lock
from typing import Dict
from fastapi import APIRouter, Depends, HTTPException, Request
from server.app.data.game_manager import GameManager
from server.app.main.move_getter import get_legal_moves


def get_game_states(request: Request) -> Dict[str, GameManager]:
    return request.app.state.game_states

def get_game_states_lock(request: Request) -> Lock:
    return request.app.state.game_states_lock


engine_router = APIRouter()


@engine_router.post("/create/{is_user_white}/game/{game_id}")
async def create_game(
    game_id: str,
    is_user_white: bool,
    game_states: Dict[str, GameManager] = Depends(get_game_states),
    game_states_lock: Lock = Depends(get_game_states_lock)
):
    new_game_manager = GameManager(is_user_white)

    async with game_states_lock:
        game_states[str(game_id)] = new_game_manager

    return {"message": "New game created"}

@engine_router.get("/bitboards/{game_id}")
async def get_game_state(
    game_id: str,
    game_states: Dict[str, GameManager] = Depends(get_game_states), 
    game_states_lock: Lock = Depends(get_game_states_lock)
):
    """Retrieves the current state of a specific game by ID."""
    async with game_states_lock:
        game_manager: GameManager = game_states.get(game_id)
        types_list = game_manager.bb_state.types_list

    if game_manager is None: # Verifica si se encontró la partida
        raise HTTPException(status_code=404, detail=f"Game with ID {game_id} not found")

    return types_list


@engine_router.get("/game/{game_id}/moves/{square}")
async def get_valid_moves(
    game_id: str,
    square: int,
    game_states: Dict[str, GameManager] = Depends(get_game_states),
    game_states_lock: Lock = Depends(get_game_states_lock)
):
    async with game_states_lock:
        game_manager: GameManager = game_states.get(game_id)
    
    legal_moves = get_legal_moves(game_manager.bb_state, square)

    return legal_moves
