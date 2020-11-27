// A header file that defines the data structure and functions used to
// represent a progress bar.
//
//      Connor Shugg

#ifndef BAR_H
#define BAR_H

// ========================= Constants and Macros ========================== //
// Progress bar drawing definitions
// https://www.utf8-chartable.de/unicode-utf8-table.pl?start=9472
// progress bar blocks
#define PROGBAR_FILLED "\u2588"     // for a filled progress bar block
#define PROGBAR_EMPTY "\u2500"      // for an empty progress bar block
// bar edges
#define PROGBAR_L_BORDER "\u251c"   // the left border of the progress bar
#define PROGBAR_R_BORDER "\u2524"   // the right border of the progress bar
// other
#define PROGBAR_CHARACTER_SIZE 3    // number of bytes that make up each char


// ========================== Progress Bar Struct ========================== //
typedef struct _ProgressBar
{

} ProgressBar;

#endif
