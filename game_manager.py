import asyncio
from typing import List, Optional, Tuple
import time

PATH = './src/test'

class GameManager:
    """
    Manages a UCI chess engine subprocess, sending commands and parsing responses.
    """
    def __init__(self, color: int):
        self.engine_path = PATH
        self.user_color = color
        self.proc: Optional[asyncio.subprocess.Process] = None
        # Agregar seguimiento de actividad
        self.last_activity = time.time()
        self.created_at = time.time()

    def update_activity(self) -> None:
        """Actualiza el timestamp de la última actividad"""
        self.last_activity = time.time()

    async def start(self) -> None:
        """Launch the engine and initialize UCI protocol."""
        self.proc = await asyncio.create_subprocess_exec(
            self.engine_path,
            stdin=asyncio.subprocess.PIPE,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE
        )
        # Send UCI initialization
        await self._send_line('ucinewgame')
        await self._read_until('readyok')
        # Actualizar actividad después de inicializar
        self.update_activity()

    async def stop(self) -> None:
        """Shut down the engine subprocess."""
        if self.proc:
            try:
                await self._send_line('quit')
                # Esperar con timeout para evitar que se cuelgue
                await asyncio.wait_for(self.proc.wait(), timeout=5.0)
            except asyncio.TimeoutError:
                # Si el proceso no termina, forzar cierre
                if self.proc.returncode is None:
                    self.proc.terminate()
                    try:
                        await asyncio.wait_for(self.proc.wait(), timeout=2.0)
                    except asyncio.TimeoutError:
                        self.proc.kill()
                        await self.proc.wait()
            except Exception as e:
                print(f"Error cerrando engine: {e}")
                # Forzar cierre si hay error
                if self.proc and self.proc.returncode is None:
                    self.proc.terminate()
                    await self.proc.wait()
            finally:
                self.proc = None

    async def _send_line(self, line: str) -> None:
        assert self.proc and self.proc.stdin
        self.proc.stdin.write(f"{line}\n".encode())
        await self.proc.stdin.drain()

    async def _read_line(self) -> str:
        assert self.proc and self.proc.stdout
        raw = await self.proc.stdout.readline()
        return raw.decode().strip()

    async def _read_until(self, keyword: str) -> List[str]:
        """Read lines until a line equals keyword or starts with it."""
        lines = []
        while True:
            line = await self._read_line()
            if line == keyword or line.startswith(keyword):
                break
            
            lines.append(int(line) if line.isdigit() else line)
        return lines
    
    async def game_flow(self, command) -> Tuple[str, int]:
        self.update_activity()

        await self._send_line(command)
        lines = await self._read_until('readyok')

        # print each line of the response
        for line in lines:
            print(line)
        message, num = lines[-1], lines[-2] 
        return message, int(num)

    async def user_moves(self, move_code) -> Tuple[str, int]:
        """Make a move via UCI makemove"""
        return await self.game_flow(f'makemove {move_code}')
    
    async def resolve_promotion(self, promotion):
        """Resolves the promotion via UCI"""
        return await self.game_flow(f'promote {promotion}')

    async def engine_moves(self) -> Tuple[str, int]:
        """Make a move via UCI enginego"""
        return await self.game_flow("enginego")

    async def get_board(self) -> List[int]:
        """Retrieve board state via custom getboard command."""
        await self._send_line('getboard')
        return await self._read_until('readyok')

    async def get_moves(self, square: int) -> List[int]:
        """Retrieve legal moves from a square via custom getmoves command."""
        await self._send_line(f'getmoves {square}')
        return await self._read_until('readyok')
