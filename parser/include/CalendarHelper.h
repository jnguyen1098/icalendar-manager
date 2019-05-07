/**
 * Function that unfolds a string into a single line.
 * A string is folded if it contains any instance of a CR + LF + whitespace
 * substring, where the whitespace can either be a space ' ' or \t tab
 **/
char *unfold(char *toUnfold);

/**
 * Function that determines if a string contains a colon or not
 **/
int containsColon(char *toCheck);

/**
 * Calendar Wrapper Function
 * This takes in a filename, creates the Calendar object, and ensures that
 * it's good. I should be able to return a string called "null" if it does
 * not work, but we'll see where this goes because I'm very tired
 **/
char *createCalendarToJSON(char *fileName);

int wrapCreateCalendar(char *calJSDON, char *evtJSON, char *writePath);

char *test(void);

char *eventListToJSONWrapper(char *filename);
