//ldd: -lncurses
#include <string.h>
#include <ncursesw/ncurses.h>
#include <time.h>
#include <stdlib.h>

void INIT () {
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  srand(time(NULL));
}

void DONE() {
  endwin();
  exit(0);
}

typedef int sudoku[9][9];
sudoku grid, solu, init, good;
char cursor[9][9];

sudoku solution = {
  {5, 3, 4, 6, 7, 8, 9, 1, 2},
  {6, 7, 2, 1, 9, 5, 3, 4, 8},
  {1, 9, 8, 3, 4, 2, 5, 6, 7},
  {8, 5, 9, 7, 6, 1, 4, 2, 3},
  {4, 2, 6, 8, 5, 3, 7, 9, 1},
  {7, 1, 3, 9, 2, 4, 8, 5, 6},
  {9, 6, 1, 5, 3, 7, 2, 8, 4},
  {2, 8, 7, 4, 1, 9, 6, 3, 5},
  {3, 4, 5, 2, 8, 6, 1, 7, 9}
};

int cursor_row = 0;
int cursor_col = 0;
int init_pos[9][2];
int row_error, col_error;

char* message = NULL;

void copy (sudoku src, sudoku tgt) {
 for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      tgt[i][j] = src[i][j];
    }
  }
}

int is_valid (sudoku g, int row, int col, int val){
    for (int i = 0; i < 9; i++) {
        if (g[row][i] == val && i!=col){
            row_error = row;
            col_error = i;
            return 0;
        }
        if (g[i][col] == val && i!=row){
            row_error = i;
            col_error = col;
            return 0;
        }
    }

  int deb_ligne_3x3 = (row / 3) * 3;
  int deb_col_3x3 = (col / 3) * 3;

  for(int i = deb_ligne_3x3; i< deb_ligne_3x3 + 3; i++){
    for(int j = deb_col_3x3; j< deb_col_3x3 + 3; j++){
        if(g[i][j]==val && i!=row && j!=col){
            row_error = i;
            col_error = j;
            return 0;
        }
    }
  }

  return 1;
}

int validate (sudoku src, sudoku tgt){
  for(int row = 0; row < 9; row++){
    for(int col = 0; col < 9; col++){
      // Regarde si une case est vide si c'est le cas alors la grille n'est pas valide
      if(src[row][col] == 0){
        return 0;
      }
      // Parcours chaque case du jeu pour savoir si elles sont chacune valide
      tgt[row][col] = is_valid(src, row, col, src[row][col]);// Stocker le resultat dans la grille tgt
      // Vérifie si la grille est toujours valide
      if(tgt[row][col]==0){
        return 0;
      }
    }
  }
  return 1;
}

int win(sudoku g) {
  // grille remplie ?
  for (int row = 0; row < 9; row++) {
    for (int col = 0; col < 9; col++) {
      if (g[row][col] == 0) {
        return 0; // 1 case vide = 0
      }
    }
  }

  // valides ?
  for (int row = 0; row < 9; row++) {
    for (int col = 0; col < 9; col++) {
      int val = g[row][col];
      if (!is_valid(g, row, col, val)) {
        return 0; // 1 case invalide = 0
      }
    }
  }

  // si remplies et valides, return 1.
  return 1;
}

int solve (sudoku src, sudoku tgt){
  copy(src, tgt);
  int row = 10, col = 10;
  for(int i=0; i<9; i++){
    for(int j=0; j<9; j++){
      if(tgt[i][j]==0){
        row = i;
        col = j;
        break;
      }
    }
    if(row != 10 && col != 10){
      break;
    }
  }

  if(row == 10 && col == 10 && validate(tgt, good)){
    copy(tgt, solu);
    return 1;
  }

  //Si row et col différent de 10 alors modifier la case par une bonne valeur
  for(int num=1; num<10; num++){
    if(is_valid(tgt, row, col, num)){
      //Insère la valeur valide dans la grille
      tgt[row][col] = num;
      //Résoud récursivement la grille
      if(solve(tgt, tgt)){
        copy(tgt, solu);
        return 1;
      }
      // Si la résolution échoue, réinitialise la case et continue la boucle
      tgt[row][col] = 0;
    }
  }
  // Si aucun nombre ne peut être placé dans la case actuelle, la grille est insoluble
  return 0;
}


void init_grid (){
  //Remplie la grille "grid" par des 0
  for(int i=0; i<9; i++){
    for(int j=0; j<9; j++){
      grid[i][j]=0;
    }
  }
  //Place aléatoirement les chiffres de 0 à 9
  for(int num=1; num<10; num++){
    grid[rand()%9][rand()%9] = num;
  }

  if(!solve(grid, solu)){
    init_grid();
  }
  else{
    copy(grid, init);
  }
}


int cursor_at_init(){
  for(int i=0; i<9; i++){
    for(int j=0; j<9; j++){
        //Regarde si le curseur est sur une case pré-remplie
        if(init[i][j]!=0){
            if(cursor_row == i && cursor_col == j){
                return 1;
            }
        }
    }
  }
  return 0;
}

void cursor_pos(){
    for(int i=0; i<9; i++){
        for(int j=0; j<9; j++){
            //Initie toute les position vide
            cursor[i][j] = ' ';
        }
    }
    if(cursor_at_init()){
        cursor[cursor_row][cursor_col] = ':';
    }
    //Check si une case n'entre pas en conflit avec une autre
    else if(grid[cursor_row][cursor_col]!=0){
        if(is_valid(grid, cursor_row, cursor_col, grid[cursor_row][cursor_col])!=1){
                cursor[cursor_row][cursor_col] = '#';
                cursor[row_error][col_error] = '*';
        }
        else{
            cursor[cursor_row][cursor_col] = '=';
        }
    }
    else{
        cursor[cursor_row][cursor_col] = '=';
    }
}

void display (){
  clear();
  printw("\n");
  printw("\n");
  //Affichage de la bar horizontal au dessus de la grille
  for(int i = 0; i <= 20; i++){
    printw("=");
  } 
  printw(" SUDOKU ");
  for(int i = 0; i <= 10; i++){
    printw("=");
  } 
  printw("\n");
  printw("\n");
  //Debut de l'affichage de la grille);
  for(int x=0; x<9; x++){
    if(x==0){
        //Corner du haut
        addch(ACS_ULCORNER);
      }
    else if(x == 1 || x == 2 || x == 4 || x == 5 || x == 7 || x == 8){
        addch(ACS_LTEE);
    }
    else if(x==3 || x==6){
        addch(ACS_PLUS);
    }
    else{
        addch(ACS_VLINE);
    }
    for(int i=0; i<3; i++){
        if(x==0 || x ==9){
            printw("%c%c%c%c", addch(ACS_HLINE), addch(ACS_HLINE), addch(ACS_HLINE));
            addch(ACS_TTEE);
            printw("%c%c%c", addch(ACS_HLINE), addch(ACS_HLINE), addch(ACS_HLINE));
            addch(ACS_TTEE);
            printw("%c%c%c", addch(ACS_HLINE), addch(ACS_HLINE), addch(ACS_HLINE));
            if(i!=2){
                addch(ACS_PLUS);
            }
        }
        else{
            printw("%c%c%c%c", addch(ACS_HLINE), addch(ACS_HLINE), addch(ACS_HLINE));
            addch(ACS_PLUS);
            printw("%c%c%c", addch(ACS_HLINE), addch(ACS_HLINE), addch(ACS_HLINE));
            addch(ACS_PLUS);
            printw("%c%c%c", addch(ACS_HLINE), addch(ACS_HLINE), addch(ACS_HLINE));
            if(i!=2){
                addch(ACS_PLUS);
            }
        }  
    }
    if(x==0){
        //Corner du haut droite
        addch(ACS_URCORNER);
      }
    else if(x == 1 || x == 2 || x == 4 || x == 5 || x == 7 || x == 8){
        addch(ACS_RTEE);
    }
    else if(x==3 || x==6){
        addch(ACS_PLUS);
    }
    else{
        addch(ACS_VLINE);
    }
    printw("\n");

    for(int y=0; y<1; y++){
    addch(ACS_VLINE);
      for(int j=0; j<9; j++){
        if(grid[x][j]==0){
            printw("%c", cursor[x][j]);
            printw(" ");
            printw("%c", cursor[x][j]);
        }
        else{
            printw("%c", cursor[x][j]);
            printw("%i", grid[x][j]);
            printw("%c", cursor[x][j]);
        }
        addch(ACS_VLINE);
      }
    }
    printw("\n");
  }
    addch(ACS_LLCORNER);
    for(int i=0; i<3; i++){
        printw("%c%c%c%c", addch(ACS_HLINE), addch(ACS_HLINE), addch(ACS_HLINE));
        addch(ACS_BTEE);
        printw("%c%c%c", addch(ACS_HLINE), addch(ACS_HLINE), addch(ACS_HLINE));
        addch(ACS_BTEE);
        printw("%c%c%c", addch(ACS_HLINE), addch(ACS_HLINE), addch(ACS_HLINE));
        if(i!=2){
            addch(ACS_PLUS);
        }
    }
    addch(ACS_LRCORNER);
    //Affiche un message si message != NULL
    if(message != NULL){
        printw("\n");

        for(int i = 0; i <= 20; i++){
            printw("=");
        } 
        printw("%s", message);
        for(int i = 0; i <= 10; i++){
            printw("=");
        } 
    }
    else{
        printw("\n");
        for(int i = 0; i < 40; i++){
        printw("=");
        }
    }
  refresh();
}

int play(){
    int k = getch();

    //Si une case n'est pas bonne alors remettre 0 si on la quitte
    if(k==KEY_RIGHT || k==KEY_LEFT || k==KEY_DOWN || k==KEY_UP){
        if(is_valid(grid, cursor_row, cursor_col, grid[cursor_row][cursor_col])!=1){
            grid[cursor_row][cursor_col]=0;
        }
    }
    //Allez à droite
    if(k == KEY_RIGHT){
        if(cursor_col<8){
            cursor_col++;
        }
        else{
            cursor_col=0;
            if(cursor_row<8){
                cursor_row++;
            }
            else{
                cursor_row=0;
            }
        }  
    }
    //Allez à GAUCHE
    if (k == (KEY_LEFT)) {
        if(cursor_col>0){
            cursor_col--;
        }
        else{
            cursor_col=8;
            if(cursor_row<8){
                cursor_row++;
            }
            else{
                cursor_row=0;
            }
        }
    }
    //Allez en BAS
    if (k == (KEY_DOWN)) {
        if(cursor_row<8){
            cursor_row++;
        }
        else{
            cursor_row=0;
            if(cursor_col<8){
                cursor_col++;
            }
            else{
                cursor_col=0;
            }
        }
    }
    //Allez en HAUT
    if(k == (KEY_UP)){
        if(cursor_row>0){
            cursor_row--;
        }
        else{
            cursor_row=9;
            if(cursor_col<8){
                cursor_col--;
            }
            else{
                cursor_col=9;
            }
        }
    }

    //Barre Espace
    if(k == 32){
        if(!cursor_at_init()){
            if(grid[cursor_row][cursor_col]<9){
                    grid[cursor_row][cursor_col]++;
            }
            else{
                grid[cursor_row][cursor_col]=0;
            }
        }
    }

    //Supprime ce qu'il y a dans la case si on appuye sur 0 ou X
    if(k == 120 || k == 48){
        if(init[cursor_row][cursor_col] == 0){
            grid[cursor_row][cursor_col]=0;
        }
    }

    //Remettre la grille a l'état initial si on appuye sur r
    if(k == 114){
        copy(init, grid);
    }

    //Résoud la case si on appuye sur h
    if(k == 104){
        grid[cursor_row][cursor_col]=solu[cursor_row][cursor_col];
    }

    //Résolution avec la touche "s"
    if(k == 115){
        for(int i=0; i<9; i++){
            for(int j=0; j<9; j++){
                grid[i][j] = solu[i][j];
            }
        }
    }

    //Quitter le jeu
    if (k == (KEY_BACKSPACE)){
        return 0;
    }
    cursor_pos();
}

int main () {
  INIT();
  init_grid();
  cursor_at_init();
  cursor_pos();
  do {
    display();
    if (win(grid)) {
      message = "YOU WIN :)";
      display();
      getch();
      break;
    }
  } while (play());
  DONE();
}