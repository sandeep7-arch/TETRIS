# TETRIS
This is a minimalistic tetris game , following many standard tetris guidelines.
This doesn't have a pause feature so yeah , and backend of the tetris grid is a array of 16 bit integers  (where the frist 10 bits build the X-axis of the 2D plane)
And regarding scoring each block added gives +25 and for each 'l' lines cleard we get +l*l*100.
## PREREQUISITES
this need raylib , so whatever OS you are on , just install raylib

## clone the repo and compile it by 
```bash
g++ engine.cpp -o tetris -lraylib
```
## controls
```
arrow_Left --> to move left one block
```
```
left_shift + arrow_left --> to move left quick
```
```
arrow_right --> to move right one block
```
```
left_shift + arrow_right --> to move right quick
```
```
arrow_down --> to move down one block
```
```
left_shift --> to move down quick
```
```
right_shift --> to rotate the piece
```
```
Esc --> quit
'''


