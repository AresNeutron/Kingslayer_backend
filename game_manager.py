import asyncio
from typing import List, Optional, Tuple
import time

PATH = './src/engine'

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
        if self.proc and self.proc.returncode is None:
            print("Engine already running.")
            return

        try:
            print(f"Attempting to start engine from: {self.engine_path}")
            self.proc = await asyncio.create_subprocess_exec(
                self.engine_path,
                stdin=asyncio.subprocess.PIPE,
                stdout=asyncio.subprocess.PIPE,
                stderr=asyncio.subprocess.PIPE
            )
            print("Engine subprocess launched.")

            await self._send_line('uci')
            await self._read_until('uciok')

            await self._send_line('isready')
            await self._read_until('readyok')

            # Ahora sí, el comando de nuevo juego
            await self._send_line('ucinewgame')
            await self._read_until('readyok')

            print("Engine initialized and ready.")
            self.update_activity()

        except FileNotFoundError:
            print(f"ERROR: Engine executable not found at {self.engine_path}")
            if self.proc:
                self.proc.kill()
                await self.proc.wait()
            self.proc = None
            raise RuntimeError(f"Engine not found at {self.engine_path}. Check PATH and permissions.")
        except Exception as e:
            print(f"ERROR starting or initializing engine: {e}")
            if self.proc:
                self.proc.kill()
                await self.proc.wait()
            self.proc = None
            raise RuntimeError(f"Failed to start/initialize engine: {e}")

    async def stop(self) -> None:
        """Shut down the engine subprocess."""
        if self.proc and self.proc.returncode is None:
            try:
                await self._send_line('quit')
                await asyncio.wait_for(self.proc.wait(), timeout=5.0)
                print("Engine quit command sent and process waited.")
            except asyncio.TimeoutError:
                print("Engine did not quit gracefully, terminating...")
                if self.proc.returncode is None:
                    self.proc.terminate()
                    try:
                        await asyncio.wait_for(self.proc.wait(), timeout=2.0)
                    except asyncio.TimeoutError:
                        print("Engine termination failed, killing...")
                        self.proc.kill()
                        await self.proc.wait()
            except Exception as e:
                print(f"Error while trying to quit/stop engine: {e}")
                if self.proc and self.proc.returncode is None:
                    self.proc.terminate()
                    await self.proc.wait()
            finally:
                self.proc = None
                print("Engine process reference cleared.")
        elif self.proc and self.proc.returncode is not None:
            print(f"Engine already stopped with exit code: {self.proc.returncode}")
            self.proc = None
        else:
            print("No engine process to stop.")
            

    async def _send_line(self, line: str) -> None:
        if not self.proc or self.proc.stdin.is_closing() or self.proc.returncode is not None:
            print(f"Engine stream is closed or not running. Attempting to restart for line: '{line}'")
            await self.start()
            if not self.proc or self.proc.stdin.is_closing() or self.proc.returncode is not None:
                raise RuntimeError(f"Engine stream could not be restored to send line: '{line}'")

        print(f"Sending to engine: {line}")
        try:
            self.proc.stdin.write(f"{line}\n".encode())
            await self.proc.stdin.drain() 
        except BrokenPipeError:
            print(f"BrokenPipeError: Engine closed unexpectedly while sending '{line}'.")
            await self.stop() 
            await self.start() 
            raise RuntimeError(f"Engine unexpectedly closed during write operation: '{line}'")
        except Exception as e:
            print(f"Error writing to engine: {e}")
            await self.stop()
            raise RuntimeError(f"Error writing to engine: {e}")


    async def _read_line(self) -> str:
        if not self.proc or self.proc.stdout.at_eof():
            raise RuntimeError("Engine stdout stream is closed or at EOF.")
        try:
            raw = await asyncio.wait_for(self.proc.stdout.readline(), timeout=10) # Timeout para la lectura
            if not raw: # Si readline devuelve vacío, el stream se ha cerrado
                raise EOFError("Engine stdout stream closed unexpectedly (empty read).")
            return raw.decode().strip()
        except asyncio.TimeoutError:
            print("Timeout reading from engine.")
            raise RuntimeError("Timeout reading from engine. Engine might be stuck or closed.")
        except EOFError as e:
            print(f"EOFError reading from engine: {e}")
            raise
        except Exception as e:
            print(f"Error reading line from engine: {e}")
            raise

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
