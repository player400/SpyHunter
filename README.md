SpyHunter - clone of an old Atari 2600 game
_____________________________________________________________

Main purpose of the game is to destroy hostile cars, while avoiding neutral (civilian) cars
Hostile cars are red, neutral cars are green and the player's car is purple.

Every second while you stay alive you get +150 points, unless you drive with speed over 600, that it's 250. For destroying a hostile car you get +250 points.
You have 100hp, enemies have 50hp and neutrals have 5hp. Every collision with any car driving over the edge of the road will result in 5hp collision damage.
Default player speed is 500. You can increase and decrease your speed by holding up and down arrow keys. After releasing the keys car will default to normal speed.
Max speed is 10 000, minimum is 150. 
Hostiles' speed is 480 and neutrals go at 250.
During the first 30 seconds of the game you get unlimited cars. Then you have only 3 lives.
Every 5000 points you get additional life.
NPC cars have basic AI (hostiles turn into you, when you're next to them, neutrals will do random turns and try to avoid collision).
Shooting is not implemented into the game. Maybe it will be in the future. Currently only way to destroy an enemy is ramming.


Controls
_______________________________

Arrows left and right - turning
Arrows up and dwon - speed control
n - new game
p - pause game
Esc - exit program
f - finish game and display score
space - shoot (not implemented yet, maybe I will implement it in the future, currently the only way to destroy a car is ramming)


License
_______________________________

Software is thereby released under MIT Licence. See opensource.org for details. Copy of the license below:

Copyright 2023 Mateusz Nurczyński

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.