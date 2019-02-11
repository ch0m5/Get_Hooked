# Get Hooked by Scotland Fury

This is a university project made by a [CITM](https://www.citm.upc.edu/ing/) (Centre de la Imatge i la Tecnologia Multimedia)
student of the TTC (Talent Tech Center) in Barcelona, in which I make my own platformer in C & C++.
I also used the [Tiled Map Editor](https://www.mapeditor.org/) software, and the [SDL](https://www.libsdl.org/)
and [pugixml](https://pugixml.org/) libraries.

Get Hooked is a platformer game featuring great freedom of movement achieved specially by the hook tool
that the player possesses and a great focus on the player's inertia. (Hook planned, not implemented yet!)

Feel free to visit our Webpage: https://scotland-fury.github.io/Hook_Platformer/

## Installation
No previous installations is required to play the game, just download and execute the .exe file *Get Hooked* from releases.

## Attention Notice
Because of some minor errors, the game might not initialize on the first execution.

## Usage
### General Controls
- ESC: Open settings menu
- P: Pause the game
- Shift + Up/Down: Change master volume
- Shift + Left/Right: Change music volume

### Player 1 Controls
- Keyboard:
	- Run: A/D
	- Jump: W
	- Crouch/Slide: S
	- Space: Attack
	
- XBOX Controller: (Planned!)

### Player 2 (Considering implementation!)

### Debug Mode
- Arrow Keys: Manual Camera Movement
- F1: Start from the first level
- F2: Start from beginning of current level
- F3: Advance to next level
- F4: Free camera movement
- F5: Save current state	(Doesn't work with enemies, coins or the timer)
- F6: Load previously saved state	(Doesn't work with enemies, coins or the timer)
- F7: Recieve instant damage (Unless on godmode)
- F8: User Interface Debug view
- F9: Game debug view (Colliders/Logic)
- F10: Godmode (No damage, no collisions, and free movement)
- F11: Activate/Deactivate FPS cap (30)
- 1/2/3/4/5: Change game scale to number chosen

## Innovation
- Living entities accelerate and deaccelerate when moving, on ground they deaccelerate automatically.
- Entities that are outside the player activation radius do not update.
- Player can attack, a collider is created mid-animation when the attack is visually being made, set by a "start" and "finish" frames.
- Player collider changes shape and position depending on his current animation.
- Player can crouch to evade things that could hit his head area.
- Player slides with a different deacceleration when trying to crouch while moving.
- Player can jump in the air once making a somersault.
- Enemies have a detection and attack radius which they use to input movement and other special attacks.
- Enemies are briefly stunned if still alive, backing off from the player. If they die, they dissapear after a set time.
- Button is a template class with a function pointer that allows easy creation of new and different buttons with different functionalities, as you can decide which return and parameters will their allocated function use.
- Some Original Debug Functionalities.

## Contributing
1. Fork it!
2. Create your feature branch: `git checkout -b my-newfeature`
3. Commit your changes: `git commit -am 'Add some
feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request and we might look into it. :D

## Author
- Carles Homs 
  - Github: https://github.com/ch0m5

## Tasklist
- Carles Homs
	- Scene Module and Logic
	- Audio Module and Logic
	- Music selection and SFX Original Making
	- FadeSceneModule and Logic
	- Collisions Module and Logic
	- Pathfinding Module and Logic
	- XML Management (Save/Load/Config)
	- Input Interpretation
	- All Debug Functionalities
	- Framerate Logic and Limitations
	- Entitiy System
	- All entities and Logic
	- User Interface System
	- All UI Elements and Logic
	- OPP Design and UML Document
	- Brofiler Implementation
	- Github Webpage and READMEs

- Repository: https://github.com/Scotland-Fury/Hook_Platformer

## Disclosure
We do not own any of the sprites, tilesets or music presented in this game. Authors credited below.

- Sprites: 
	- Character *Pixel Adventurer* made by *rvros*: https://rvros.itch.io/
		Downloaded from https://rvros.itch.io/animated-pixel-hero

- Tilesets:
	- *Caves of Gallet* tileset by *Adam Saltsman*: https://adamatomic.itch.io/
		Downloaded from https://adamatomic.itch.io/caves-of-gallet

- Music: 
	- MainMenu - New World: https://www.youtube.com/watch?v=uwgqElzccUE
		composed by Kevin Penkin: http://www.kpenkmusic.com/
	- Level 1 - Mystic Force: https://www.youtube.com/watch?v=pfryxqYSHqA&feature=youtu.be
	- Level 2 - Spellbound: https://www.youtube.com/watch?v=FXDThZaMO4c&feature=youtu.be
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
