# Program Intelligence
Computing power has advanced tremendously since 2010 when the solving logic
was initially written.  The solving logic uses several optimizations and
heuristics to decide what, and for how long, to search.

The logic control can basically be broken down among these controls:

* Time
* Space
* Futility
* Exhaustion
* Attention

Internally these are controlled in the Solver settings.

### Time
This is perhaps the easiest control to understand.  It simply answers the
question:

	How many seconds to search before concluding there is no solution?
	
This value is set in the solver with the method:

	SetMaximumTime
	
The time is not exact match.  It is simply that when reaching the main loop
the program checks its watch and sees how long it has been working on the
solution and, if the clock is expired, it exits without a solution.

### Space
This is also an easy control to understand:

	How many unique boards should be examined before concluding there is no solution?
	
The value is set in the solver with the method:

	SetBoardsToCheck
	
The larger the value the more unique positions the solver will consider.

The solutions for Bakers Game can easily wind up within a weird kind of
loop where a card is repeatedly moved between tableau and reserve - without
checking to see if the board state had already been visited the solver would
waste time and memory on repeat states.  Unfortunately the paths leading
into loops are not always easy to identify, and therefore sometimes
solutions do include silly useless moves.

### Futility
Futility is a measure of how far along a path to proceed before simply
giving up.  In the solver this is determined by counting the number of
steps that have been performed without a single card making it to the
foundation:

	How many steps have been taken on the current path with no card moved to the foundation?
	
The value is set in the solver with the method:

	SetIntelligence
	
This control will, to some extent, moderate the solver becoming stuck
evaluating particularly long string loops.  However, if the value is set too
low then the solver will abandon paths possibly too early as they do not
look sufficiently promising.

### Exhaustion
The exhaustion metric is a simple measure:

	What is the maximum possible size of a solution?
	
The value is set in the solver with the method:

	SetMaximumMoves
	
A potential solution that has reached this number of moves is simply abandoned.

This differs from the futility metric above in an important point - the futility
metric is the number of steps without a card moving to the foundation - regardless
of what the final solution size would be - while the exhaustion metric is the total
number of steps taken - regardless of the number of steps since the last card was
moved to the foundation.

If this value is set below 52 then no solution is possible; there are 52 cards to
move from the tableau to the foundation!

But it should also be noted that a board arrangement requiring hundreds of moves to
resolve would be *"double-plus-un-fun"* to play and so, when building boards for the
user to play, these boards should be skipped anyway.

### Attention
Although the solver does pursue a depth-first search rather than a breadth-first
search the amount of memory needed to contain all of the potential boards and boards
seen can grow tremendously.  This was a considerable problem in the 32-bit world
of 2010 when most processes were only able to address 2 GB of user space (without
employing tricks) - and even 64-bit machines might lack large amounts of RAM so
it was still necessary to constrain the search space to avoid swapping to disk.

The solver will aggressively begin trimming the queue of pending boards to examine
with this metric, basically asking:

	How many boards to examine should be allowed to accumulate before trimming?

This value is set in the solver with the method:

	SetMaximumStackSize

The larger the value of this metric the more memory the program will consume.  This
makes it, along with Time, one of the most important metrics to control when using
a pay-as-you-go service, such as Amazon Lambda.

# Heuristics
Some solutions are simply better than others.

### Always move a card to the foundation
The game is complete when all cards are moved to the foundation.  There is never
a need (or even a rule) to move a card from the foundation back into play, so if a
card *can* be moved to the foundation then it *should* be moved to the foundation.

This heuristic also allows the solver to shortcut any other potential moves at that
level; there is one move for the current game state and that move is to be taken!

### Never move Reserve to Reserve
This heuristic is rather obvious ... but the rules do permit a card to be moved from
its slot in the reserve to another open reserve slot!  These moves do not advance
the board state to a solution, though - it merely produces a symmetric board.

### Tableau-to-Tableau is better than Reserve-to-Tableau
On Bakers Game the reserve spaces are very important to moving stacks, which is
frequently necessary as part of the solution.  It should also be important to note
that reserve to tableau is "reversible" - the next move can move that card back
to reserve - but tableau to tableau is not.

By prioritizing tableau to tableau the solver is less likely to fall into a loop,
which helps both Easy and Standard Bakers Game, and leaves the reserve spaces open
for stack moves in Standard.
