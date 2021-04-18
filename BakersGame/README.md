# Bakers Game
The brute-force way to play solitaire!

# Purpose
*Intelligence is for the weak.  Symmetry is for the pretty.*

*Pure force is the way.*

---- Unknown

Playing card based solitaire games is something frequently
done by people, but it is a low-profit activity.  It is
therefore an ideal task to be automated - the computer can
play thousands of solitaire games for you while the human
accomplishes more profitable tasks!

The computing logic has been broken into two pieces -
a library **bakersgame.a** containing the necessary
solving logic and an interactive piece to execute
the logic and build the database of solutions;
the program **bruteForce**.

The computing logic has also been designed to function
as part of a larger solver universe, such as part of a
networked solved like AWS Lambda.

See the "usage.txt" file under the **doc** directory
for additional details.

Also see the "intelligence.md" file for a discussion
of how the solver intelligence is controlled.

# Building
This project uses "make" to build the necessary files
for command line execution, and build-gyp to build files
for NodeJS.

## Building for the command line
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
