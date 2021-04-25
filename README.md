# ABCg

Development framework accompanying the course [MCTA008-17 Computer Graphics](http://professor.ufabc.edu.br/~harlen.batagelo/cg/) at [UFABC](https://www.ufabc.edu.br/).

## Original Author

Harlen Batagelo

## Student

Igor Neres Trindade - 11030416

## CG Applications

### [Flappy ~~Bird~~ Pacman](https://igornerest.github.io/abcg/public/flappybird/index.html)
Tap the screen to make your bird fly. If you hit a pipe, your bird falls and you restart the game. The further you go, the higher your score will become (but the game also gets harder).

<p align="center">
  <img src="flappy-pacman.gif" alt="animated" />
</p>

### [Haunted Maze 3D](https://igornerest.github.io/abcg/public/maze3d/index.html)
You woke up alone in a haunted maze. It is dark outside... strange noises everywhere. Are you afraid? Don't be. Find the black flag to escape (and have a little surprise muahahaha)

Controls:
- Click anywhere to start moving the camera
- Press ESC to lose screen focus
- Press F to turn on/off the flashlight (and eventually pay some respect :D)
- Press WASD to move around

Some nice features:
- 3D camera movement
- Spotlight
- Simple dynamic weather (moving moon)
- Customizable maze
- Sound effects

<p align="center">
  <img src="maze3d.gif" alt="animated" />
</p>

## About this repo

All the projects can be found in ``./examples`` directory. You can compile them using ``./build.sh`` on Linux or ``./build.bat`` on Windows, which will generate executable files in ``./build/bin``

Some projects were compiled to generate WebAssembly binaries. They can be found in ``/public`` directory

## License

MIT
