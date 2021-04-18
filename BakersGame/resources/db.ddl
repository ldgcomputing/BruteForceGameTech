
--
-- Create the solitaire types table
--
-- The solType *MUST* match to the
-- enumeration value with SolitaireBoard.h
--

create table sol_types (
	solType integer not null primary key,
	solTypeName text not null
);

-- Insert the solitaire types
insert into sol_types ( solType, solTypeName ) values ( 1 , 'Klondike 1' ) ;
insert into sol_types ( solType, solTypeName ) values ( 2 , 'Klondike 3' ) ;
insert into sol_types ( solType, solTypeName ) values ( 3 , 'Bakers Game' ) ;
insert into sol_types ( solType, solTypeName ) values ( 5 , 'Bakers Game Easy' ) ;

--
-- Create the list of primary boards (non-alternates)
--

create table primary_boards (
	boardNumber integer not null primary key,
	solType integer not null,
	compressedBoard text not null
);
create index primary_boards_board on primary_boards ( compressedBoard , solType );

--
-- Create the list of alternate boards
--
-- This table links all of the alternate boards back to a primary
-- board for purposes of finding solutions, etc.
--

create table alternate_boards (
	primaryBoardNumber integer not null,
	solType integer not null,
	compressedBoard text not null,
	clubsMapTo integer not null,
	diamondsMapTo integer not null,
	heartsMapTo integer not null,
	spadesMapTo integer not null
);
create index alternate_boards_board on alternate_boards ( compressedBoard , solType );
create index alternate_boards_id on alternate_boards ( primaryBoardNumber );

--
-- Create the table of solutions (header)
--

create table solutions_header (
	solutionNumber integer not null primary key,
	originSolutionNumber integer,
	numberMoves integer not null,
	dateTimeCreatedUTC text not null,
	solutionTimeMilliSeconds integer not null
);

--
-- Create the table of solutions (detail)
--
-- Values for moveFrom and moveTo are from SolitaireBoard.h:MOVEPOS_T
--

create table solutions_detail (
	solutionNumber integer not null,
	solutionOrder integer not null,
	moveFrom integer not null,
	fromCol integer not null,
	fromRow integer not null,
	moveTo integer not null,
	toCol integer not null,
	toRow integer not null
);
create unique index solutions_detail_moves on solutions_detail ( solutionNumber , solutionOrder );

--
-- Map boards to solutions
--

create table boards_to_solutions (
	primaryBoardNumber integer not null,
	solutionNumber integer not null
);
create unique index boards_to_solutions_boardnum on boards_to_solutions ( primaryBoardNumber , solutionNumber );

-- Scripts to clean up the boards and solutions
delete from boards_to_solutions;
delete from solutions_detail;
delete from solutions_header;
delete from alternate_boards;
delete from primary_boards;
