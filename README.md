# Get Hooked by ch0m5
This is a university project made by a [CITM](https://www.citm.upc.edu/ing/) (Centre de la Imatge i la Tecnologia Multimedia) student of the TTC (Talent Tech Center) in Barcelona, in which I make my own platformer in C & C++. I also used the [Tiled Map Editor](https://www.mapeditor.org/) software, and the [SDL](https://www.libsdl.org/) and [pugixml](https://pugixml.org/) libraries.

Get Hooked is a platformer game showcasing two levels, basic UI, fluid character movement and controls, Save & Load at any point mid-game, and my first attempt at a deep heriarchy-based entity class system.

Feel free to visit our [Webpage](https://scotland-fury.github.io/Hook_Platformer/).

## Installation
No previous installations is required to play the game, just download and execute the .exe file *Get Hooked* from releases.

## Attention Notice
Because of some minor errors, the game might not initialize on the first execution.

## Usage
### General Controls
* ESC: Open settings menu
* P: Pause the game
* Shift + Up/Down: Change master volume
* Shift + Left/Right: Change music volume

### Player 1 Controls
* Keyboard:
	* Run: A/D
	* Jump: W
	* Crouch/Slide: S
	* Space: Attack

### Debug Mode
* Arrow Keys: Manual Camera Movement
* F1: Start from the first level
* F2: Start from beginning of current level
* F3: Advance to next level
* F4: Free camera movement
* F5: Save current state	(Doesn't work with enemies, coins or the timer)
* F6: Load previously saved state	(Doesn't work with enemies, coins or the timer)
* F7: Recieve instant damage (Unless on godmode)
* F8: User Interface Debug view
* F9: Game debug view (Colliders/Logic)
* F10: Godmode (No damage, no collisions, and free movement)
* F11: Activate/Deactivate FPS cap (30)
* 1/2/3/4/5: Change game scale to number chosen

## Innovation
* Living entities accelerate and deaccelerate when moving, on ground they deaccelerate automatically.
* Entities that are outside the player activation radius do not update.
* Player can attack, a collider is created mid-animation when the attack is visually being made, set by a "start" and "finish" frames.
* Player collider changes shape and position depending on his current animation.
* Player can crouch to evade things that could hit his head area.
* Player slides with a different deacceleration when trying to crouch while moving.
* Player can jump in the air once making a somersault.
* Enemies have a detection and attack radius which they use to input movement and other special attacks.
* Enemies are briefly stunned if still alive, backing off from the player. If they die, they dissapear after a set time.
* Button is a template class with a function pointer that allows easy creation of new and different buttons with different functionalities, as you can decide which return and parameters will their allocated function use.
* Some Original Debug Functionalities.

## Contributing
1. Fork it!
2. Create your feature branch: `git checkout -b my-newfeature`
3. Commit your changes: `git commit -am 'Add some
feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request and I might look into it. :D

## Author
* Carles Homs 
  * Github: [ch0m5](https://github.com/ch0m5)

* [Repository](https://github.com/ch0m5/Get_Hooked)

![Team Photo](https://raw.githubusercontent.com/ch0m5/Get_Hooked/gh-pages/Cara.png)

## Tasklist
* Carles Homs
	* Scene Module and Logic
	* Audio Module and Logic
	* Music selection and SFX Original Making
	* FadeSceneModule and Logic
	* Collisions Module and Logic
	* Pathfinding Module and Logic
	* XML Management (Save/Load/Config)
	* Input Interpretation
	* All Debug Functionalities
	* Framerate Logic and Limitations
	* Entitiy System
	* All entities and Logic
	* User Interface System
	* All UI Elements and Logic
	* OPP Design and UML Document
	* Brofiler Implementation
	* Github Webpage and READMEs

## Disclosure
I do not own any of the sprites, tilesets or music presented in this game. Authors credited below.

* Sprites:
	* Character [*Pixel Adventurer*](https://rvros.itch.io/animated-pixel-hero) made by [*rvros*](https://rvros.itch.io/)

* Tilesets:
	* [*Caves of Gallet*](https://adamatomic.itch.io/caves-of-gallet) tileset by [*Adam Saltsman*](https://adamatomic.itch.io/) 

* Music: 
	* MainMenu - [New World](https://www.youtube.com/watch?v=uwgqElzccUE)
		* Composed by [Kevin Penkin](http://www.kpenkmusic.com/)
	* Level 1 - [Mystic Force](https://www.youtube.com/watch?v=pfryxqYSHqA&feature=youtu.be)
	* Level 2 - [Spellbound](https://www.youtube.com/watch?v=FXDThZaMO4c&feature=youtu.be)
		* Composed by [Kevin MacLeod](https://www.youtube.com/channel/UCSZXFhRIx6b0dFX3xS8L1yQ)

* SFX: Orignal mix made from: https://www.bfxr.net/

## License & Copyright 

© ch0m5

Licensed under the [MIT License](LICENSE)
