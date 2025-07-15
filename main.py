from typing import Dict
import asyncio
from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from contextlib import asynccontextmanager
from fastapi.middleware.cors import CORSMiddleware
from router import engine_router
from game_manager import GameManager
import time


# Diccionario para almacenar las instancias de GameManager activas
game_states: Dict[str, GameManager] = {}

# Esencial si tienes workers o múltiples peticiones accediendo al mismo tiempo
game_states_lock = asyncio.Lock()

""" ADVICE: THIS SERVER DOESN'T SAVE GAMES, DELETES THEM AFTER A PERIOD OF INACTIVITY"""
async def cleanup_inactive_games():
    while True:
        await asyncio.sleep(1800)  # 30 minutos
        current_time = time.time()
        
        async with game_states_lock:
            to_remove = []
            for game_id, game in game_states.items():
                # Eliminar juegos inactivos por más de 1 hora
                if current_time - game.last_activity > 3600:  # 1 hora inactivo
                    to_remove.append(game_id)
            
            for game_id in to_remove:
                game_manager = game_states[game_id]
                # Cerrar el engine antes de eliminar
                await game_manager.stop()
                del game_states[game_id]
                print(f"Juego {game_id} eliminado por inactividad")


@asynccontextmanager
async def lifespan(app: FastAPI):
    # Store the instance in the app's state so it can be accessed by dependencies
    app.state.game_states = game_states
    app.state.game_states_lock = game_states_lock
    
    # Iniciar tarea de limpieza automática
    cleanup_task = asyncio.create_task(cleanup_inactive_games())
    
    yield
    
    # Limpiar al cerrar el servidor
    cleanup_task.cancel()
    try:
        await cleanup_task
    except asyncio.CancelledError:
        pass
    
    # Cerrar todos los engines activos
    async with game_states_lock:
        for game_manager in game_states.values():
            await game_manager.stop()


app = FastAPI(lifespan=lifespan)


# Middleware CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=[
        "http://localhost:3000",
        "https://kingslayer-frontend-fabio-quevedos-projects.vercel.app",
        "https://kingslayer-frontend.vercel.app",
        "https://kingslayer-frontend-git-main-fabio-quevedos-projects.vercel.app",
        "https://vercel.com/fabio-quevedos-projects/kingslayer-frontend/6MEisVT5ooqrUXjUbgExjkAeWBrA"
        ],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Agregamos las rutas HTTP
app.include_router(engine_router)

""" Web Sockets connections from here"""

# These ones are the possible engine messages: "none", "check", "checkmate", "stalemate", "promotion"
# Always from the user's perspective

@app.websocket("/ws/{game_id}")
async def websocket_endpoint(websocket: WebSocket, game_id: str):
    await websocket.accept()
    
    game_states: Dict[str, GameManager] = websocket.app.state.game_states
    game_states_lock: asyncio.Lock = websocket.app.state.game_states_lock
    
    try:
        while True:
            ui_message: dict = await websocket.receive_json()
            event = ui_message.get("event")
            data = ui_message.get("data") 

            print("Message from UI:")
            print(event)

            message = None
            num = None

            async with game_states_lock:
                game_manager: GameManager = game_states.get(game_id)
                
                if event == "user_moves":
                    message, num = await game_manager.user_moves(data) # here "data" is the move code

                elif event == "engine_moves":
                    message, num = await game_manager.engine_moves() # data is not needed here

                elif event == "promotion":
                    message, num = await game_manager.resolve_promotion(data) # here "data" is the promotion

            if message is not None and num is not None:
                await websocket.send_json({
                    "event": message,
                    "data": num
                })

    except WebSocketDisconnect:
        # In case of disconection, erase the game. Data is lost
        print("WEBSOCKET DISCONNECTION")
        
        async with game_states_lock:
            game_manager: GameManager = game_states.get(game_id)
            if game_manager:
                print("ERASING DATA")
                del game_states[game_id]
        
    except Exception as e:
        print(f"An error occurred in websocket_endpoint: {e}")
        await websocket.send_json({"event": "error", "data": str(e)})
