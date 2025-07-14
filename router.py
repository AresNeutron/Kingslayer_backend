from asyncio import Lock
from typing import Dict
from fastapi import APIRouter, Depends, HTTPException, Request
from game_manager import GameManager


def get_game_states(request: Request) -> Dict[str, GameManager]:
    return request.app.state.game_states


def get_game_states_lock(request: Request) -> Lock:
    return request.app.state.game_states_lock

engine_router = APIRouter()

@engine_router.post("/create/{user_color}/game/{game_id}")
async def create_game(
    game_id: str,
    user_color: int, # 0 or 1 for black or white
    game_states: Dict[str, GameManager] = Depends(get_game_states),
    game_states_lock: Lock = Depends(get_game_states_lock)
):
    async with game_states_lock:
        if game_id in game_states:
            raise HTTPException(status_code=400, detail=f"Game {game_id} already exists")
        # Instantiate and start a new engine
        gm = GameManager(user_color)
        await gm.start()
        game_states[game_id] = gm
    return {"game_id": game_id}


@engine_router.get("/board/{game_id}")
async def get_game_state(
    game_id: str,
    game_states: Dict[str, GameManager] = Depends(get_game_states), 
    game_states_lock: Lock = Depends(get_game_states_lock)
):
    async with game_states_lock:
        gm = game_states.get(game_id)
    if not gm:
        raise HTTPException(status_code=404, detail=f"Game {game_id} not found")

    board = await gm.get_board()
    return {"board": board}


@engine_router.get("/game/{game_id}/moves/{square}")
async def get_valid_moves(
    game_id: str,
    square: int,
    game_states: Dict[str, GameManager] = Depends(get_game_states),
    game_states_lock: Lock = Depends(get_game_states_lock)
):
    async with game_states_lock:
        gm = game_states.get(game_id)
    if not gm:
        raise HTTPException(status_code=404, detail=f"Game {game_id} not found")

    # These ones are move codes
    moves = await gm.get_moves(square)
    return {"moves": moves}
