# Chessboard

A chess board built in c++ that can be used as the gui for UCI engines. To play against an engine that comes with this build, press e and type `./engines/stockfish`, it should be set to stockfish by default.

## Contians :

- A board that shows all the pieces in symbols
- Pieces can be moved with the keyboard
- No protection against illegal moves (but engines may crash if one happens)
- Moves shown in alg. notation (Only last 3)
- Works with UCI engines
- You can switch sides (it will restart the game!)
- Comes with Stockfish pre-installed

## Requires:

- `g++`, `make`, `git`
- ncurses development files (`libncurses-dev`)
- A terminal that supports color

## Setup

### Linux

```
sudo apt install -y g++ make git libncurses-dev    # Debian/Ubuntu
git clone https://github.com/YOUR-USERNAME/Chessboard.git
cd Chessboard
sh setup.sh
./chess
```

`setup.sh` downloads Stockfish from source, builds it into an `engines/` folder, and compiles the game.
It may take some time for stockfish to download!

I cannot guarantee this 100% works because I do not own a linux machine but it worked on Google Shell (instructions for it underneath) so it should work.

### Windows

Use WSL: download it via `wsl --install` in PowerShell, restart your pachine, open "Ubuntu" and then use the Linux steps above.

### In browser

Google Cloud Shell (shell.cloud.google.com) should function as a online free Linux terminal, to use it go to the website and follow the Linux steps above.

## Controls

- Arrow Keys / IJKL - Move the cursor 
- Space / Enter - pick up and put down pieces (used to move them)
- `E` - Pick a new enginee
- `N` - Start a new game
- `S` - Swith the side
- `Q` - Quit

To move a piece select it with Space, move the cursor (yellow highlighted square) to the intended position, and press space again.

If the engine is (engine failed), you didn't link the engine's path correctly or the engine does not exist, reuse setup.sh if that is the case!

## Licences 
Stockfish is free software under the GNU GPL v3 (https://stockfishchess.org). This
repository does not include a Stockfish binary: `setup.sh` builds it from the
official source at https://github.com/official-stockfish/Stockfish.