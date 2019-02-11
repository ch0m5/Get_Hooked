# Get Hooked by ch0m5
This is a university project made by a [CITM](https://www.citm.upc.edu/ing/) (Centre de la Imatge i la Tecnologia Multimedia) student of the TTC (Talent Tech Center) in Barcelona, in which I made my own 2D platformer in C & C++. I also used the [Tiled Map Editor](https://www.mapeditor.org/) software, and the [SDL](https://www.libsdl.org/) and [pugixml](https://pugixml.org/) libraries.

## Installation
No previous installations is required to play the game, just download and execute the .exe file *Get Hooked* from releases.

Lastest Release: [Get Hooked v0.4](https://github.com/Scotland-Fury/Hook_Platformer/releases/tag/0.4)

## Attention Notice
Because of some minor errors, the game might not initialize on the first execution.

## How to play
### General Controls
* ESC: Open settings menu
* P: Pause the game
* Shift + Up/Down: Change master volume
* Shift + Left/Right: Change music volume

### Player Controls
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
* F5: Save current state	(Doesn't work on enemies, coins, nor the timer)
* F6: Load previously saved state	(Doesn't work on enemies, coins, nor the timer)
* F7: Recieve instant damage (Unless on godmode)
* F8: User Interface Debug view
* F9: Game debug view (Colliders/Logic)
* F10: Godmode (No damage, no collisions, and free movement)
* F11: Activate/Deactivate FPS cap (30)
* 1/2/3/4/5: Change game scale to number chosen

## Author
* Carles Homs 
  * Github: [ch0m5](https://github.com/ch0m5)

<p align="center">
  <img src="https://raw.githubusercontent.com/Scotland-Fury/Hook_Platformer/gh-pages/Cara.png"/>
</p>

## Subsystems
* The game features several modules that manage the entire game workflow, like textures, renderization, window setup, fonts, etc.
* Both entities and GUI elements are managed by a detailed object tree and modules that handle their logic, fabrication and destruction.
* Object polymorphism has been exploited to maximum possible efficiency, meaning that there's several layers so that each class has only has the members and methods that it really needs so new types of objects with similar characteristics can be created with ease. For example, a PhysicalEntity has all data related to colliders, while his child Creature has "alive" data.
* All modules, entities, and GUI elements follow a similar step-by-step workflow. The last two have a second workflow layer, where the conditions like position, movement and inputs are read, a new state is decided, and then all changes are applied. Inputs can come from player or other sources, enemies' "inputs" are based on the player's character.
* All entities are related to the player by radius. Attack, movement, pathfinding, and their logic workflow are conditioned to the player to skip uneeded processes, like updating if far away from the player.
* User Interface Elements have a parent and children class so they can be linked through each other for different functions, like cursor dynamic moving or positioning.
* The Button template class is an image with a function pointer which return and parameters can be decided on the creation of the element. This makes the creation of buttons with different functionalities very easy. A Button is something clickable that calls a function on press, and that's it, while its child ActionBox includes 4 sprites, one per button state.
* A delta time is embedded into the game, and allows to change from a cap of 30 fps to unlimited, this applies to both logic and animations.

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

## Gameplay Showcase
<html>
<body>

<iframe width="645" height="520" src="https://www.youtube.com/embed/TCNyWyyBZjc" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

</body>
</html>

## Disclosure
We do not own any of the sprites, tilesets or music presented in this game. Authors credited below.

* Sprites: 
	* Character [*Pixel Adventurer*](https://rvros.itch.io/animated-pixel-hero) made by [*rvros*](https://rvros.itch.io/)

* Tilesets:
	* [*Caves of Gallet*](https://adamatomic.itch.io/caves-of-gallet) tileset by [*Adam Saltsman*](https://adamatomic.itch.io/) 

* Music: 
	* Track 1 - [Mystic Force](https://www.youtube.com/watch?v=pfryxqYSHqA&feature=youtu.be)
	* Track 2 - [Spellbound](https://www.youtube.com/watch?v=FXDThZaMO4c&feature=youtu.be)
	* Composed by [Kevin MacLeod](https://www.youtube.com/channel/UCSZXFhRIx6b0dFX3xS8L1yQ)

* SFX: Orignal mix made from: [https://www.bfxr.net/](https://www.bfxr.net/)

## License & Copyright 

© ch0m5

Licensed under the [MIT License](https://github.com/Scotland-Fury/Hook_Platformer/blob/master/LICENSE)
