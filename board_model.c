// The board model

#include <curses.h>
#include "worm.h"
#include "board_model.h"
#include "messages.h"

// Place an item onto the curses display.
void placeItem(struct board* aboard, int y, int x, enum BoardCodes board_code,
               chtype symbol, enum ColorPairs color_pair) {

    aboard->cells[y][x] = board_code;  // Store Codes on the board
    //  Store item on the display (symbol code)
    move(y, x);          // Move cursor to (y,x)
    attron(COLOR_PAIR(color_pair));     // Start writing in selected color
    addch(symbol);   // Store symbol on the virtual display
    attroff(COLOR_PAIR(color_pair));    // Stop writing in selected color
}

enum ResCodes initializeBoard(struct board* aboard) {
    // Check dimensions of the board
    if ( COLS < MIN_NUMBER_OF_COLS ||
    LINES < MIN_NUMBER_OF_ROWS + ROWS_RESERVED ) {
        char buf[100];
        sprintf(buf,"Das Fenster ist zu klein: wir brauchen %dx%d",
        MIN_NUMBER_OF_COLS , MIN_NUMBER_OF_ROWS + ROWS_RESERVED );
        showDialog(buf,"Bitte eine Taste druecken");
        return RES_FAILED;
        }
        // Maximal index of a row
        aboard->last_row = MIN_NUMBER_OF_ROWS - 1;
        // Maximal index of a column
        aboard->last_col = MIN_NUMBER_OF_COLS - 1;
        return RES_OK;
}

enum ResCodes initializeLevel(struct board* aboard) {
    int y;  // define local variables for loops etc
    int x;
    // Fill board and screen buffer with empty cells.
    for (y = 0; y <= aboard->last_row; y++) {
        for (x = 0; x <= aboard->last_col; x++) {
            placeItem(aboard,y,x,BC_FREE_CELL,SYMBOL_FREE_CELL,COLP_FREE_CELL);
        }
    }

    // Draw a line in order to separate the message area
    // Note: we cannot use function placeItem() since the message area
    // is outside the board!
    y = aboard->last_row + 1;
    for (x=0; x <= aboard->last_col ; x++) {
        move(y, x);
        attron(COLOR_PAIR(COLP_BARRIER));
        addch(SYMBOL_BARRIER);
        attroff(COLOR_PAIR(COLP_BARRIER));
    }
    // Draw a line to signal the rightmost column of the board.
    for (y=0; y <= aboard->last_row ; y++) {
        placeItem(aboard,y,aboard->last_col,
        BC_BARRIER,SYMBOL_BARRIER,COLP_BARRIER);    
    }

    // Barriers: use a loop
    for ( y=12; y<25; y++ ) {
        x=15;
        placeItem(aboard,y,x,BC_BARRIER,SYMBOL_BARRIER,COLP_BARRIER);
    }
    for ( y=4; y<15; y++ ) {
        x=50;
        placeItem(aboard,y,x,BC_BARRIER,SYMBOL_BARRIER,COLP_BARRIER);
    }
    // Food
    placeItem(aboard, 3, 3,BC_FOOD_1,SYMBOL_FOOD_1,COLP_FOOD_1);
    placeItem(aboard, 20, 55,BC_FOOD_1,SYMBOL_FOOD_1,COLP_FOOD_1);

    placeItem(aboard, 5, 45,BC_FOOD_2,SYMBOL_FOOD_2,COLP_FOOD_2);
    placeItem(aboard, 3, 26,BC_FOOD_2,SYMBOL_FOOD_2,COLP_FOOD_2);
    placeItem(aboard, 24, 48,BC_FOOD_2,SYMBOL_FOOD_2,COLP_FOOD_2);
    placeItem(aboard, 10, 11,BC_FOOD_2,SYMBOL_FOOD_2,COLP_FOOD_2);
   
    placeItem(aboard, 20, 2,BC_FOOD_3,SYMBOL_FOOD_3,COLP_FOOD_3);
    placeItem(aboard, 22, 12,BC_FOOD_3,SYMBOL_FOOD_3,COLP_FOOD_3);
    placeItem(aboard, 14, 57,BC_FOOD_3,SYMBOL_FOOD_3,COLP_FOOD_3);
    placeItem(aboard, 9, 9,BC_FOOD_3,SYMBOL_FOOD_3,COLP_FOOD_3);
    // Initialize number of food items
    // Attention: must match number of items placed on the board above
    aboard->food_items = 10;
    return RES_OK;
}

// Getters
// Get the last usable row on the display
int getLastRowOnBoard(struct board* aboard) {
    return aboard->last_row;
}
// Get the last usable collum on the display
int getLastColOnBoard(struct board* aboard) {
    return aboard->last_col;
// Get the number of Items that are food for the worm
}
int getNumberOfFoodItems(struct board* aboard) {
    return aboard->food_items;
}
// Get the Content of the Array cells of position.y and position.x
enum BoardCodes getContentAt(struct board* aboard, struct pos position) {
    return aboard->cells[position.y][position.x];
}


// SETTERS

// Set the parameter n as the number of Fooditems (food_items)
void setNumbersOfFoodItems(struct board* aboard, int n) {
    aboard->food_items = n;
}
// Decrement the number of food items on he board by 1
void decrementNumberOfFoodItems(struct board* aboard) {
    aboard->food_items = aboard->food_items - 1;
}




