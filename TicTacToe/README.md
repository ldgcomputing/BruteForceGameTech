# Tic Tac Toe
The brute-force way to play Tic-Tac-Toe!

# Purpose
*Intelligence is for the weak.  Symmetry is for the pretty.*

*Pure force is the way.*

---- Unknown

This program builds a library (**tictactoe.a**) and
sample execution program (**BruteForce**) to enumerate
Tic-Tac-Toe game paths and count the wins and losses
for both players.

The computing logic is designed to function as part
of a larger solver universe, such as in an executable
program or as part of networked solver (such as
AWS Lambda).

See the "usage.txt" file under the **src** directory
for additional details.

# Building
This project uses "make" to build the necessary files
for command line execution, and build-gyp to build files
for NodeJS.

## Building for the command line
The board to process can be changed in the BruteForce.cpp
file, located under the **src** directory.  It's also
possible to alter the *main* function to accept command
line arguments, or read from a file, or anything else
the "student" so desires.

The BruteForce.cpp *main* also runs the unit tests to
ensure that the program's logic is sound.

A specific target can be built by setting the target:

> export TARGET=debug;
> make all

> export TARGET=release;
> make all

## Building for NodeJS
The exports for NodeJS are kept in the nodeExports.cpp and
can be changed there.

The binding.gyp file will use the default **build/Release**
target location with the name of addon.node.  Combined with
the index.js file the pair make an excellent ZIP file for
deployment on AWS Lambda.
 