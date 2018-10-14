# Get Hooked by Scotland Fury

This is a university project made by [CITM](https://www.citm.upc.edu/ing/) (Centre de la Imatge i la Tecnologia Multimedia)
students of the TTC (Talent Tech Center) in Barcelona, in which we make our own platformer in C & C++.
We also used the [Tiled Map Editor](https://www.mapeditor.org/) software, and the [SDL](https://www.libsdl.org/)
and [pugixml](https://pugixml.org/) libraries.

Get Hooked is a platformer game featuring great freedom of movement achieved specially by the hook tool
that the player possesses and a great focus on the player's inertia. (Hook planned, not implemented yet!)

Feel free to visit our Webpage: https://scotland-fury.github.io/Hook_Platformer/

## Installation
No previous installations is required to play the game, just download and execute the .exe file *Get Hooked* from releases.

## Usage
### General Controls
- ESC: Quit the game
- F5: Save current state
- F6: Load previously saved state

### Player 1 Controls
- Keyboard:
	- Run: A/D
	- Jump: W
	- Crouch: S
	
- XBOX Controller: (Planned!)

### Player 2 (Considering implementation!)

### Debug Mode
- F1: Start from the first level
- F2: Start from beginning of current level
- F3: Recieve instant damage (Unless on godmode)
- F9: Debug view (Colliders/Logic)
- F10: Godmode (No damage recieved)

## Innovation
- Player accelerates when moving and deaccelerates when trying to stop
- Player can crouch to evade things that could hit his head area
- Player slides with a different deacceleration when trying to crouch while moving
- Player can jump in the air once making a somersault

## Contributing
1. Fork it!
2. Create your feature branch: `git checkout -b my-newfeature`
3. Commit your changes: `git commit -am 'Add some
feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request and we might look into it. :D

## The Team: Scotland Fury (https://github.com/Scotland-Fury)
* Carles Homs 
  * Role: Code
  * Github: https://github.com/ch0m5

* Samuel Kurt
  * Role: Code
  * Github: https://github.com/samuelkurtdavidson

## Disclosure
We do not own any of the sprites, tilesets or music presented in this game. Authors credited below.

- Sprites: 
	- Character *Pixel Adventurer* made by *rvros*: https://rvros.itch.io/
		Downloaded from https://rvros.itch.io/animated-pixel-hero

- Tilesets:
	- *Caves of Gallet* tileset by *Adam Saltsman*: https://adamatomic.itch.io/
		Downloaded from https://adamatomic.itch.io/caves-of-gallet

- Music: 
	- Track 1 - Mystic Force: https://www.youtube.com/watch?v=pfryxqYSHqA&feature=youtu.be
	- Track 2 - Spellbound: https://www.youtube.com/watch?v=FXDThZaMO4c&feature=youtu.be
		Composed by Kevin MacLeod: https://www.youtube.com/channel/UCSZXFhRIx6b0dFX3xS8L1yQ

- SFX: Orignal mix made from: https://www.bfxr.net/
	
## License & Copyright 
MIT License

Copyright (c) 2018 Scotland Fury

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
