# Parallax Demo

### About

Some time around 2007 I was bored and decided to write an *old skool* demo, like those I use to see on the Atari ST.

This abomination is the result.

### Concepts

This was a pure OpenGL immediate mode affair; no shaders were used so blending between background images was done completely via the blending units on the GPU.

There is a degree of structure to the project in that every layer of the scene got it's own self contained class to do things which inheriated from a simple *IDemo* interface to do things like start up, update and render.

The font was made using AngelCode's Bitmap font generator with support code written by *Promit* and lifted from the Gamedev.net forms. (URl in the source and readme.txt in the project).

This all resulted in nicely kerned text for the scroll text, which was nice.

### Notes

The original project used a screamtracker mod as the background music; a Smells Like Teen Spirit cover.

Bass was the original sound system used, the lib, dll and header need to be in the same directory as the source code.

Neither of the two above are included in this source drop as I lack the rights to do so.
(I'm not 100% sure on the rights for the images included as well as they were just pulled from the internet a long time ago - I've included those however.)

As noted in the readme.txt, the maths library used was taken from Game Programming Gems 1.
The camera code is, afaik, largely my own and not really used beyond a simple setup in this project.

All code is released under MIT - see license text for details.
