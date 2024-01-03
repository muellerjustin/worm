// A simple variant of the game Snake
//
// Used for teaching in classes
//
// Author:
// Franz Regensburger
// Ingolstadt University of Applied Sciences
// (C) 2011
//
// The worm model
#include <curses.h>
#include "worm.h"
#include "board_model.h"
#include "worm_model.h"


//Functions//
// START WORM_DETAIL
// The following functions all depend on the model of the worm

// Initialize the worm
extern enum ResCodes initializeWorm(struct worm* aworm, int len_max, int len_cur,
                  struct pos headpos, enum WormHeading dir, enum ColorPairs color) 

{
    //Local variables for loops etc.
    int i;

    //Initialize last usable index to len_max - 1
    aworm->maxindex = len_max - 1;

    // Current last usable index in array. May grow upto maxindex
    aworm->cur_lastindex = len_cur;
    //Initialize headindex
    aworm->headindex = 0;
  
    // Mark all elements as unused in the array of positions.
    // This allows for the effect that the worm appears element by element at
    // the start of each level
    // wir sind in initialize worm, somit sind alle IndexplÃ¤tze leer, auÃŸer der Erste.
    // for (inizialisierungsStatement; tesetAussage; updateStatement)
    // i ist der aktuelle headindex +1, damit wird die forLoop gestartet und 
    // und der nÃ¤chste ausstehende Index(Platz) untersucht
    // an dem als nÃ¤chstes der headindex gespeichert werden soll.
    // Ist dieser Platz kleiner oder gleich dem maxindex,
    // dann markiere ihn als unused und schau dir den nÃ¤chsten platz an (i +1).
    // Das geschieht solang bis die testAussage nichtmehr stimmt.
    
    for (i = 0; i <= aworm->maxindex; i++){
      aworm->wormpos[i].y = UNUSED_POS_ELEM;
      aworm->wormpos[i].x = UNUSED_POS_ELEM;
    }
    //Initialize position of the worms head
    aworm->wormpos[aworm->headindex] = headpos;
    //Initialize the heading of the worm
    setWormHeading(aworm,dir);
    //Initialize color of thw worm
    aworm->wcolor = color;
    return RES_OK;
}

void growWorm(struct worm* aworm, enum Boni growth) {
    // Play it safe and inhibit surpassing the bound
    if (aworm->cur_lastindex + growth <= aworm->maxindex) {
          aworm->cur_lastindex += growth;
    } else {
          aworm->cur_lastindex = aworm->maxindex;
    }
}

// Show the worms's elements on the display
// Simple version
void showWorm(struct board* aboard, struct worm* aworm) {
    // Place new tail
    int tailindex = ( aworm->headindex + 1) % (aworm->cur_lastindex + 1);
    if(aworm->wormpos[tailindex].y == UNUSED_POS_ELEM){
      tailindex = 0;
    }
    placeItem(aboard, aworm->wormpos[tailindex].y,
            aworm->wormpos[tailindex].x,
            BC_USED_BY_WORM,
            SYMBOL_WORM_TAIL_ELEMENT,
            aworm->wcolor);
    // Place new head
    placeItem(aboard, aworm->wormpos[aworm->headindex].y,
            aworm->wormpos[aworm->headindex].x,
            BC_USED_BY_WORM,
            SYMBOL_WORM_HEAD_ELEMENT,
            aworm->wcolor);

    // Replace old head
    int headafterindex = aworm->headindex - 1;
    if (headafterindex == -1){
      headafterindex = aworm->cur_lastindex;
    }
    placeItem(aboard, aworm->wormpos[headafterindex].y,
            aworm->wormpos[headafterindex].x,
            BC_USED_BY_WORM,
            SYMBOL_WORM_INNER_ELEMENT,
            aworm->wcolor);
}

void cleanWormTail(struct board* aboard, struct worm* aworm) {
  int tailindex;
  // Compute tailindex
  tailindex = (aworm->headindex + 1) % (aworm->cur_lastindex + 1);
  // Check the array of worm elements.
  // Is the array element at tailindex already in use?
  // Checking either array wormpos_y or wormpos_x is enough.
  if ( aworm->wormpos[tailindex].x != UNUSED_POS_ELEM ) {
    // YES: 
    placeItem(aboard, aworm->wormpos[tailindex].y, aworm->wormpos[tailindex].x,
    BC_FREE_CELL,SYMBOL_FREE_CELL, COLP_FREE_CELL);
  } 
}

void moveWorm(struct board* aboard, struct worm* aworm, enum GameStates* agame_state) {
  struct pos headpos;
  // Get the current position of the worm's head element and
  headpos = aworm->wormpos[aworm->headindex];
  // compute the new head position according to current heading.
  // Do not store the new head position in the array of positions, yet.
  headpos.x += aworm->dx;
  headpos.y += aworm->dy;
  // Check if we would hit something (for good or bad) or are going to leave
  // the display if we move the worm's head according to worm's last
  // direction. We are not allowed to leave the display's window.
  if (headpos.x < 0) {
      *agame_state = WORM_OUT_OF_BOUNDS;
  } else if (headpos.x > getLastColOnBoard(aboard) ) {
      *agame_state = WORM_OUT_OF_BOUNDS;
  } else if (headpos.y < 0) {
      *agame_state = WORM_OUT_OF_BOUNDS;
  } else if (headpos.y > getLastRowOnBoard(aboard) ) {
      *agame_state = WORM_OUT_OF_BOUNDS;
  } else {
      // We will stay within bounds.
      // Check if the worms head hits any items at the new position on the board.
      // Hitting food is good, hitting barriers or worm elements is bad.
      switch ( getContentAt(aboard,headpos) ) {
          case BC_FOOD_1:
              *agame_state = WORM_GAME_ONGOING;
              // Grow worm according to food item digested
              growWorm(aworm, BONUS_1);
              decrementNumberOfFoodItems(aboard);
              break;
          case BC_FOOD_2:
              *agame_state = WORM_GAME_ONGOING;
              // Grow worm according to food item digested
              growWorm(aworm, BONUS_2);
              decrementNumberOfFoodItems(aboard);
              break;
          case BC_FOOD_3:
              *agame_state = WORM_GAME_ONGOING;
              // Grow worm according to food item digested
              growWorm(aworm, BONUS_3);
              decrementNumberOfFoodItems(aboard);
              break;
          case BC_BARRIER:
                // That's bad: stop game
                *agame_state = WORM_CRASH;
              break;
          case BC_USED_BY_WORM:
                // That's bad: stop game
                *agame_state = WORM_CROSSING;
              break;
          default:
                // Without default case we get a warning message.
                {;} // Do nothing. C syntax dictates some statement, here.
      }
  } 

  // Check the status of *agame_state
  // Go on if nothing bad happened
  if ( *agame_state == WORM_GAME_ONGOING ) {
        // So all is well: we did not hit anything bad and did not leave the
        // window. --> Update the worm structure.
        // Increment headindex
        // Go round if end of worm is reached (ring buffer)
        aworm->headindex++;
        if (aworm->headindex > aworm->cur_lastindex) {
            aworm->headindex = 0;
        }
        // Store new coordinates of head element in worm structure
        aworm->wormpos[aworm->headindex] = headpos;
  }
}

// Setters
void setWormHeading(struct worm* aworm, enum WormHeading dir) {
    switch(dir) {
        case WORM_UP :// User wants up
            aworm->dx=0;
            aworm->dy=-1;
            break;
        case WORM_DOWN :// User wants down
            aworm->dx=0;
            aworm->dy=1;
            break;
        case WORM_LEFT: // User wants left
            aworm->dx=-1;
            aworm->dy=0;
            break;
        case WORM_RIGHT:// User wants right
            aworm->dx=1;
            aworm->dy=0;
            break;
    }
}

// Getters
struct pos getWormHeadPos(struct worm* aworm) {
    // Structures are passed by value!
    // -> we return a copy here
    return aworm->wormpos[aworm->headindex];
}

int getWormLength(struct worm* aworm) {
    return aworm->cur_lastindex +1;
}
