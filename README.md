# Random-Sudoku-Generator-GUI-
A random sudoku generator written in c for linux with GUI (X11)

I tried to write an generator for random Sudoku-Puzzles using the rand() function in C. This is by far not a perfect algorithm to create Sudokus, but maybe it helps somebody, who wants to create something similar or just wants to play ;).
<br>
The graphical user interface was built with use of the X11 library.



## Guide for compilation
For those who just want to play and are not interrested in C-programming, you can compile this program with the gcc-compiler using this command:
```
gcc main.c csudoku.c export.c -o Sudoku -lX11
```

After that you run it with
```
./Sudoku
```


Note that if you select a difficulty above 57 it's possible, that the Program may take a long time to generate the puzzle. You can force the application to stop with following key combination (in terminal):
```
Ctrl + c
```
