/*
 * sudoku.h
 *
 *  Created on: 14.12.2018
 *      Author: daniel
 */

#ifndef SUDOKU_H_
#define SUDOKU_H_

#define SIZE 9

/* creates a fully filled grid of a 9x9 Sudoku-matrix */
extern void CreateSudoku(
		int*	/* First adress of first field e.g. "&grid[0][0]"" */
);

/* clears fields till sudoku has one unique solution */
extern void rmfields(
		int* 	/* beginning-adress of fully filled grid */,
		int*	/* beginning-adress of the grid with removed Fields */,
		int		/* Amount of empty fields (difficulty-level) */
);

/* export current sudoku as html */
extern int Exporthtml (
		int* 	/* beginning-adress of the grid */
);


#endif /* SUDOKU_H_ */
