#include "CalendarParser.h"

#define GRN   "\x1B[32m"
#define RED   "\x1B[31m"
#define YEL   "\x1B[33m"

#define RESET "\x1B[0m"

/*
 * Priority vulnerability
 *
 * This was not tested in my validateCalendar() harness unfortunately.
 * The way I ordered my modifications tested for most cases, but there
 * exists one case that is very rare, but could cause issues in the
 * event that the official test harness tests for it.
 *
 * Some people iterate in the Calendar object, iterating through each
 * of the events. In doing so, sometimes they also check the alarms.
 * When this is the case, they might also return the first error they
 * encounter. If you iterate through an event's properties, then its
 * alarms, and then you go to the next event's properties, then that
 * event's alarms, you run the risk of returning an INV_ALARM error
 * when you're not supposed to. For example, take a look at the 
 * following scenario:
 *
 * [CALENDAR START]
 *
 *         [EVENT START]
 *              [ALARM START]
 *                  NULL TRIGGER (INV_ALARM)
 *              [ALARM END]
 *         [EVENT END]
 *
 *         [EVENT START]
 *              NULL PROPERTIES LIST (INV_EVENT)
 *         [EVENT END]
 *
 * [CALENDAR END]
 *
 * The first event has an invalid alarm prop (let's say propDescr is
 * empty). The second event has an invalid event prop. In this case,
 * INV_EVENT has a higher priority than INV_ALARM and you must return
 * that; not INV_ALARM. But due to the way that some people iterate
 * through events, they might do:
 *
 * [look at event one's properties] (possibility of INV_EVENT)
 * [look at event one's alarms]     (possibility of INV_ALARM)
 *
 * [look at event two's properties] (possibility of INV_EVENT)
 * [look at event two's alarms]     (possibility of INV_ALARM)
 *
 * So we see here that the potential error to be returned swaps back
 * and forth between invalid alarm and invalid error. This is fine if
 * you have an error priority handler, but if all you do is return the
 * first error you come across, this creates erratic, unpredictable
 * behaviour that cannot be feasible for rare cases like these:
 * a later event having an invalid event incident after a previous event
 * has an invalid alarm contained within.
 *
 * This test harness will look for this vulnerability.
 */

int main(void)
{
    /* Create the Calendar object */
    Calendar *calendar = calloc(1, sizeof(Calendar));

    if (calendar == NULL) {
        fprintf(stderr, "Calendar could not be allocated\n");
        return 1;
    }

    /* Misc. info */
    calendar->version = 2.0;
    strcpy(calendar->prodID, "PRODID");

    /* Initialize lists */
    calendar->events = initializeList(printEvent, deleteEvent, compareEvents);
    calendar->properties = initializeList(printProperty, deleteProperty, compareProperties);

    if (calendar->events == NULL || calendar->properties == NULL) {
        fprintf(stderr, "Could not allocate calendar's lists\n");
        deleteCalendar(calendar);
        return 1;
    }

    /* Create a valid event*/
    Event *doomedAlarm   = calloc(1, sizeof(Event));
    
    if (doomedAlarm == NULL) {
        fprintf(stderr, "Could not allocate Event\n");
        deleteCalendar(calendar);
        return 1;
    }

    strcpy(doomedAlarm->UID, "doomed alarm");

    strcpy((doomedAlarm->creationDateTime).date, "19981026");
    strcpy((doomedAlarm->creationDateTime).time, "100000");
    (doomedAlarm->creationDateTime).UTC = true;

    strcpy((doomedAlarm->startDateTime).date, "20190202");
    strcpy((doomedAlarm->startDateTime).time, "101212");
    (doomedAlarm->startDateTime).UTC = false;

    doomedAlarm->properties = initializeList(printProperty, deleteProperty, compareProperties);
    doomedAlarm->alarms = initializeList(printAlarm, deleteAlarm, compareAlarms);
    
    if (doomedAlarm->properties == NULL || doomedAlarm->alarms == NULL) {
        fprintf(stderr, "Could not allocate alarm's lists\n");
        deleteCalendar(calendar);
        return 1;
    }
    
    Alarm *badAlarm = calloc(1, sizeof(Alarm));

    if (badAlarm == NULL) {
        fprintf(stderr, "Could not allocate Alarm\n");
        deleteCalendar(calendar);
        return 1;
    }

    strcpy(badAlarm->action, "AUDIO");

    char *trigger = calloc(sizeof(char), strlen("-PT15m") + 1);

    if (trigger == NULL) {
        fprintf(stderr, "Could not allocate alarm's trigger\n");
        deleteCalendar(calendar);
        return 1;
    }

    strcpy(trigger, "-PT15M");
    badAlarm->trigger = trigger;

    badAlarm->properties = initializeList(printProperty, deleteProperty, compareProperties);

    if (badAlarm->properties == NULL) {
        fprintf(stderr, "Could not allocate Alarm's properties list\n");
        deleteCalendar(calendar);
        return 1;
    }

    insertBack(doomedAlarm->alarms, badAlarm);

    /* Create another valid event*/
    Event *doomedEvt = calloc(1, sizeof(Event));

    if (doomedEvt == NULL) {
        fprintf(stderr, "Could not allocate Event\n");
        deleteCalendar(calendar);
        return 1;
    }

    strcpy(doomedEvt->UID, "doomed evt");

    strcpy((doomedEvt->creationDateTime).date, "20141211");
    strcpy((doomedEvt->creationDateTime).time, "102342");
    (doomedEvt->creationDateTime).UTC = true;

    strcpy((doomedEvt->startDateTime).date, "19981025");
    strcpy((doomedEvt->startDateTime).time, "121212");
    (doomedEvt->startDateTime).UTC = false;

    doomedEvt->properties = initializeList(printProperty, deleteProperty, compareProperties);
    doomedEvt->alarms = initializeList(printAlarm, deleteAlarm, compareAlarms);

    if (doomedEvt->alarms == NULL || doomedEvt->properties == NULL) {
        fprintf(stderr, "Could not allocate event's lists\n");
        deleteCalendar(calendar);
        return 1;
    }

    /* Add these two events to the calendar */
    insertBack(calendar->events, doomedAlarm);
    insertBack(calendar->events, doomedEvt);

    /* Calibration -- we need to make sure validateCalender() returns OK first
     * before we do our vulnerability test */
    ICalErrorCode calibrate = validateCalendar(calendar);

    if (calibrate != OK)
    {
        printf(YEL"Fatal: could not get initial OK from validateCalendar()\n"RESET);
        deleteCalendar(calendar);
        return 1;
    }

    /* Perform vulnerability */

    /* First we invalidate the first Event's only alarm by making the
     * trigger = NULL */
    free(((Alarm*)(getFromFront(doomedAlarm->alarms)))->trigger);
    ((Alarm*)getFromFront(doomedAlarm->alarms))->trigger = NULL;

    /* Next we invalidate the second Event by making the properties 
     * list = NULL */
    free(doomedEvt->properties);
    doomedEvt->properties = NULL;

    /* Check results */

    ICalErrorCode result = validateCalendar(calendar);
    
    switch (result)
    {
        case INV_EVENT:
            printf(GRN"INV_EVENT returned.\n\nCongratulations! Your validator is protected from this exploit.\n"RESET);
            break;

        case INV_ALARM:
            printf(RED"INV_ALARM returned.\n\nOh no! Your validator is vulnerable to this exploit. Consider not reading the alarms list until after you're done iterating through all events!\n"RESET);
            break;

        case INV_CAL:
            printf(YEL"INV_CAL returned.\n\nOh no! Your validator returns INV_CAL, which is neither INV_EVENT nor INV_ALARM. Are you sure there's something wrong in the Calendar struct itself?\n"RESET);
            break;

        case OK:
            printf(YEL"OK returned.\n\nOh no! Your validator returns OK, despite there being an invalid Event and an invalid Alarm in TWO separate occasions...? Please take another look at what your validateCalendar() handles.\n"RESET);
            break;

        case OTHER_ERROR:
            printf(YEL"OTHER_ERROR returned.\n\nOh no! This usually isn't a threat for most people as malloc() / calloc() are not usually used in validateCalendar(), and those functions usually don't fail. Perhaps run the test again?\n"RESET);
            break;

        default:
            printf(YEL"Error out of range.\n\nOh no! You returned an error that is outside of what validateCalendar() is supposed to return (only INV_CAL, INV_EVENT, INV_ALARM, and OTHER_ERROR). Please take another look at what your validateCalendar() handles.\n"RESET);
            break;
    }

    deleteCalendar(calendar);
    return 0;
}
