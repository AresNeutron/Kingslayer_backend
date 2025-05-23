from typing import Dict
import asyncio
from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from contextlib import asynccontextmanager
from fastapi.middleware.cors import CORSMiddleware
from server.app.data.game_manager import GameManager
from server.router import engine_router
from server.app.main.game_flow import engine_move, user_move


# Diccionario para almacenar las instancias de GameManager activas
game_states: Dict[str, GameManager] = {}

# Esencial si tienes workers o múltiples peticiones accediendo al mismo tiempo
game_states_lock = asyncio.Lock()

@asynccontextmanager
async def lifespan(app: FastAPI):
    # Store the instance in the app's state so it can be accessed by dependencies
    app.state.game_states = game_states
    app.state.game_states_lock = game_states_lock

    yield


app = FastAPI(lifespan=lifespan)


# Middleware CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Agregamos las rutas HTTP
app.include_router(engine_router)

""" Web Sockets connections from here"""

@app.websocket("/ws/{game_id}")
async def websocket_endpoint(websocket: WebSocket, game_id: str):
    # Es decir, que esto espera por una llamada del cliente para connectarse
    await websocket.accept()
    
    game_states: Dict[str, GameManager] = websocket.app.state.game_states
    game_states_lock: asyncio.Lock = websocket.app.state.game_states_lock
    
    print("LENGTH OF THE GAME STATES DICT")
    print(len(game_states))
    
    try:
        # Este bloque acepta mensajes del cliente y actua en consecuencia
        while True:
            message: dict = await websocket.receive_json()
            event = message.get("event")
            data: dict = message.get("data")

            async with game_states_lock:
                
                game_manager: GameManager = game_states.get(game_id)
                
                server_message = {}

                if event == "user_moves":
                    game_manager, server_message = user_move(game_manager, data)

                elif event == "engine_moves":
                    game_manager, server_message = engine_move(game_manager)

                elif event == "promotion":
                    promotion, square = data["promotion"], data["pawn_square"]
                    game_manager.bb_state.promote_pawn(square, promotion)
                    server_message = {"event": "promotion_made"}

                game_states[game_id] = game_manager
                await handle_server_events(server_message, websocket)


    except WebSocketDisconnect:
        # In case of disconection, erase the game. Data is lost
        print("WEBSOCKET DISCONNECTION")
        
        async with game_states_lock:
            game_manager: GameManager = game_states.get(game_id)
            if game_manager:
                print("ERASING DATA")
                del game_states[game_id]


async def handle_server_events(message: dict, websocket: WebSocket):
    data_list = [
        ("promotion_required", {"pawn_square": message.get("square")}),
        ("promotion_made", {}),
        ("checkmate", {"user_wins": message.get("user_wins")}),
        ("stalemate", {}),
        ("check", {"threats": message.get("threats")}), 
        ("user_move_made", {}),
        ("engine_move_made", {})
    ]

    current_event = message.get("event")

    for event, data in data_list:
        if event == current_event:
            await websocket.send_json({"event": event, "data": data})