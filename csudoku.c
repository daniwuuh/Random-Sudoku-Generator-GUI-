/* functions to generate a sudoku-puzzle-grid
 *
 *
 * csudoku.c
 *
 *  Created on: 14.12.2018
 *      Author: daniel
 */


#include "sudoku.h"
#include<stdio.h>
#include<time.h>
#include<stdlib.h>


/* this function will create a solved sudoku with random numbers, it will try different solutions till it has one unique solution */
void CreateSudoku(int *field00) {
	int *p[SIZE][SIZE]; /*Pointer-matrix to destination-grid */
	int trys1, trys2 = 0; /* two variables which count the failures for one field */
	_Bool test = 1; /* if the number is inconsitent this variable is set to 1*/
	srand(time(NULL));

	for (int x = 0; x < 9; x++) {
		for (int y = 0; y < 9; y++) {
			p[x][y] = field00+y+SIZE*x;
		}
	}

	for (int x = 0; x < 9; ++x) {
		for (int y = 0; y < 9; ++y) {
			trys1 = 0;

			do {
				/* generate a random number */
				*p[x][y] = (rand() % 9) + 1;
				test = 0;
				/* check if generated number is consistent */
				/* check the current row for consitency */
				for (int xz = 0; xz < x; xz++) {
					if (*p[xz][y] == *p[x][y]) {
						test = 1;
					}
				}

				/*check the column for consistency*/
				if (!test) {
					for (int yz = 0; yz < y; yz++) {
						if (*p[x][yz] == *p[x][y]) {
							test = 1;
						}
					}
				}

				/* check the block for consistency */
				int xb = x/3, yb = y/3;

				if (!test) {
					for (int xz = xb*3; xz < xb*3+3; xz++) {
						for (int yz = yb*3; yz < y; yz++) {
							if (*p[xz][yz] == *p[x][y])
								test = 1;
						}
					}
					for (int xz = xb*3; xz < x; xz++) {
						for (int yz = yb*3; yz < yb*3+3; yz++) {
							if (*p[xz][yz] == *p[x][y])
								test = 1;
						}
					}
				}
				trys1++;


				/* go back to first row in the current block */
				if (trys1 > 50) {
					x = xb*3;
					y = 0;
					trys2++;
				}

				if (trys2 > 20) {
					trys2 = 0;
					x = 0;
					y = 0;
				}

			} while (test);
		}
	}
	printf("\n\nSudoku generated successfully\n");
	printf("Now removing fields...\n\n");
	fflush(stdout);
}







/* this function sequentially clears random fields and refreshes the candmatr, which counts the possible solutions */
/* to check if the grid is solvable, the candmatr will be refreshed and all fields with 1 candidate will be filled. That process is repeated till the grid is fully solved */
void rmfields(int *Feld00, int *matr, int difficulty) {
	int xrand,yrand, counter, trys;

	_Bool distinct;

	int *p[SIZE][SIZE];
	int candmatr[SIZE][SIZE] = { 0 };
	int destmatr[SIZE][SIZE];
	int xemp[81], yemp[81];

	/* the Pointer-matrix gets the adresses of the fully solved input-matrix */
	for (int x = 0; x < 9; x++) {
		for (int y = 0; y < 9; y++) {
			p[x][y] = Feld00+y+SIZE*x;
			destmatr[x][y] = *(Feld00+y+SIZE*x);
		}
	}

	counter = 0; /* counts how much fields are cleared */
	trys = 0;

	/* repeat clearing one field and try to solve it:
	if it is not solvable, try another field or restore destmatr and try again */
	do {
		_Bool t, solved, distinct;
		/* generates a random adress of a field, that will be cleared */
		do {
			/* select random fields to clear*/
			xrand = rand() % 9;
			yrand = rand() % 9;

			/* check if field was already cleared */
			t = 0;
			for (int i = 0; i < counter; i++) {
				if (xemp[i] == xrand && yemp[i] == yrand) {
					t = 1;
				}
			}

		} while (t);

		/* safe the selected field to the "cleared-fields-history" */
		xemp[counter] = xrand;
		yemp[counter] = yrand;

		destmatr[xrand][yrand] = 0;
		counter++;

		/* try to solve the grid with removed fields,
		if there isnt a unique solution distinct will be set to 0 else solved gets 1 */
		do {


/* begin refresh candmatr -------------------------------------------------------*/
			_Bool test;
			for (int k = 0; k < counter; k++) {
				candmatr[xemp[k]][yemp[k]] = 0;

				/* if field is already filled, jump to next field */
				if (destmatr[xemp[k]][yemp[k]] != 0) {
					continue;
				}

				/* Beginning-adress of the current block, used for checking block */
				int xb = (xemp[k]/3)*3, yb = (yemp[k]/3)*3;

				for (int i = 1; i <= 9; i++) {
					if (i == *p[xemp[k]][yemp[k]]) {
						candmatr[xemp[k]][yemp[k]]++;
						continue;
					}

					test = 0;


					/* check if number is contained in column */

					for (int yz = 0; yz < yb; yz++) {
						if (i == destmatr[xemp[k]][yz]) {
							test = 1;
						}
					}
					for (int yz = yb+3; yz < 9; yz++) {
						if (i == destmatr[xemp[k]][yz]) {
							test = 1;
						}
					}

					/* check if number is contained in row */

					if (!test) {
						for (int xz = 0; xz < xb; xz++) {
							if (i == destmatr[xz][yemp[k]]) {
								test = 1;
							}
						}
						for (int xz = xb+3; xz < 9; xz++) {
							if (i == destmatr[xz][yemp[k]]) {
								test = 1;
							}
						}
					}

					/* check if number is contained in block */

					if (!test) {
						for (int yz = yb; yz < yb+3; yz++) {
							for (int xz = xb; xz < xb+3; xz++) {
								if (i == destmatr[xz][yz]) {
									test = 1;
								}
							}
						}
					}

					if (!test) {
						candmatr[xemp[k]][yemp[k]]++;
					}
				}
			}

/* end refresh candmatr ---------------------------------------------------------*/


			distinct = 0;
			solved = 1;
			for (int k = 0; k < counter; k++) {
				if (candmatr[xemp[k]][yemp[k]] == 1) {
					distinct = 1;
				}
				if (candmatr[xemp[k]][yemp[k]] > 0) {
					solved = 0;
				}
			}

			if (solved) {
				break;
			}

			if (distinct) {
				for (int k = 0; k < counter; k++) {
					if (candmatr[xemp[k]][yemp[k]] == 1) {
						destmatr[xemp[k]][yemp[k]] = *p[xemp[k]][yemp[k]];
					}
				}
			}

		} while (distinct);


		/* The destmatr was filled with numbers, now restore previous state */
		for (int k = 0; k < counter; k++) {
			destmatr[xemp[k]][yemp[k]] = 0;
		}

		if (solved) {
			/* if sudoku has one unique solution, do this */
			if (counter < difficulty) {
				trys = 0;
				continue;
			} else {
				break;
			}
		} else if (trys < 20) {
			/* if the grid has no unique solution, do this */
			destmatr[xrand][yrand] = *p[xrand][yrand];
			candmatr[xrand][yrand] = 0;
			counter--;
			trys++;
		} else {
			/* if sudoku has no unique solutions and the last 20 trys did not have one either, do this*/
			for (int k = 0; k < counter; k++) {
				destmatr[xemp[k]][yemp[k]] = *p[xemp[k]][yemp[k]];
				candmatr[xemp[k]][yemp[k]] = 0;
			}
			counter = 0;
			trys = 0;
		}
	} while (1);

	/* return the cleared puzzle to the "output-matrix" */
	for (int x = 0; x < SIZE; x++) {
		for (int y = 0; y < SIZE; y++) {
			*(matr+y+SIZE*x) = destmatr[x][y];
		}
	}
	printf("Fields removed successfully\n\n");
}
