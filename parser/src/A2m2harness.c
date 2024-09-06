#include "CalendarParser.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define DAMAGE_TEXT 200

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define CYA   "\x1B[36m"
#define MAG   "\x1B[35m"

#define RESET "\x1B[0m"

/* Global variables are the reason why this test harness exists ;) */
char damage[DAMAGE_TEXT]; // description of the damage I will do
Calendar *calendar; // our Calendar object to validate
ICalErrorCode input; // return value of createCalendar. This shouldn't fail.
ICalErrorCode calibrate; // return value of validateCalendar before damage is done
ICalErrorCode validate; // return value of validateCalendar.
char *firstError = NULL; // createCalendar's error string
char *secondError = NULL; // validateCalendar's return code before damage. Should be OK.
char *thirdError = NULL; // validateCalendar's return code after I do my damage

/* User Score Counters */

int cal_n = 0, cal_d = 0; // INV_CAL score
int evt_n = 0, evt_d = 0; // INV_EVENT score
int alm_n = 0, alm_d = 0; // INV_ALARM score

int cce_n = 0, cce_d = 0; //             INV_CAL + INV_EVENT score
int cca_n = 0, cca_d = 0; //             INV_CAL + INV_ALARM score
int cea_n = 0, cea_d = 0; //           INV_EVENT + INV_ALARM score

int all_n = 0, all_d = 0; // INV_CAL + INV_EVENT + INV_ALARM score

int aok_n = 0, aok_d = 0; //                              OK score

int n = 0, d = 0;         //                           TOTAL score

/* Experimental: error storage */
char *errors[5500];
int errorCount = 0;

/*****************************************************************************
 * Loads the guinea pig file, making sure that createCalendar doesn't choke. *
 *****************************************************************************/
void load(char *fileName)
{
    /* createCalendar shouldn't fail! It'd be bad if it did. */
    input = createCalendar(fileName, &calendar);
    /* Writing out the return code just in case */
    firstError = printError(input);
    /* Seriously, please don't fail at this point lol */
    if (input != OK)
        printf("FATAL:   createCalendar throws %s instead of OK\n", firstError);
    /* Phew, we made it. Time to free this error string */
    free(firstError);
    /* Let's make sure validateCalendar gives us the OK before I do damage */
    //calibrate = validateCalendar(calendar);
    return;
}

/*******************************************************************************
 * Makes sure that validateCalendar returns OK on the file before we do damage *
 *******************************************************************************/
int prime()
{
    /* This is where we make sure we are calibrated / tare'd */
    calibrate = validateCalendar(calendar);
    if (calibrate == OK)
        /* 1 signifies A-OK */
        return 1;
    else
    {
        secondError = printError(calibrate);
        printf(YEL"FATAL: validateCalendar throws %s instead of OK (%s)\n", secondError, damage);
        printf(RESET); // reset text colour
        d++;
        free(secondError);
    }
    /* We are unable to continue testing until validateCalendar returns OK */
    return 0;
}

/*******************************************************************************
 * After the damage is done, we see if validateCalendar throws the right error *
 *******************************************************************************/
void test(ICalErrorCode intent)
{
    /* We run the validateCalendar after the damage is done */
    validate = validateCalendar(calendar);
    /* We create the error string just in case */
    thirdError = printError(validate);
    /* We see if the ICalErrorCode matches the intended error passed */
    if (validate != intent)
    {
        printf(RED"BAD  : %s thrown mistakenly (%s)\n", thirdError, damage);d++;
        errors[errorCount] = calloc(1, sizeof(char) * 250);
        sprintf(errors[errorCount], "BAD: %s thrown mistakenly (%s)\n", thirdError, damage);
        errorCount++;
    }
    else
    {printf(GRN"GOOD : %s thrown successfully (%s)\n", thirdError, damage);d++;n++;}
    /* . . . we free the stuff; rinse and repeat. */
    if (calendar) deleteCalendar(calendar);
    free(thirdError);
    printf(RESET); // reset text colour
    return;
}

/* JUMP TABLE FUNCTIONS */

/* INV_CAL */
void emptyProdid(void)
{
    strcat(damage, "Calendar empty PRODID");
    (calendar->prodID)[0] = '\0';
    return;
}

void prodidExceedsArrayLength(void)
{
    strcat(damage, "Calendar PRODID exceeds array length");
    for (int i = 0; i < 1000; i++)
        (calendar->prodID[i]) = 'A';
    return;
}

void calendarHasNullEventList(void)
{
    strcat(damage, "Calendar NULL event list");
    ListIterator events = createIterator(calendar->events);
    void *event, *alarm;
    event = nextElement(&events);
    while ((event = nextElement(&events)) != NULL)
    {
        clearList(((Event*)event)->properties);
        ListIterator eventAlarms = createIterator(((Event*)event)->alarms);
        while (((alarm = nextElement(&eventAlarms)) != NULL))
            clearList(((Alarm*)alarm)->properties);
        clearList(((Event*)event)->alarms);
    }
    freeList(calendar->events);
    calendar->events = NULL;
    return;
}

void calendarHasNullPropertyList(void)
{
    strcat(damage, "Calendar NULL property list");
    freeList(calendar->properties);
    calendar->properties = NULL;
    return;
}

void calendarHasEmptyEventList(void)
{
    strcat(damage, "Calendar empty event list");
    ListIterator events = createIterator(calendar->events);
    void *event, *alarm;
    event = nextElement(&events);
    while ((event = nextElement(&events)) != NULL)
    {
        clearList(((Event*)event)->properties);
        ListIterator eventAlarms = createIterator(((Event*)event)->alarms);
        while (((alarm = nextElement(&eventAlarms)) != NULL))
            clearList(((Alarm*)alarm)->properties);
        clearList(((Event*)event)->alarms);
    }
    clearList(calendar->events);
    return;
}

void calendarHasIllegalComponentTag(void)
{
    strcat(damage, "Calendar illegal component");
    size_t len = strlen("ILLEGAL");
    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * len + 1);
    strcpy(illegal->propName, "BEGIN");
    strcpy(illegal->propDescr, "ILLEGAL");
    insertBack(calendar->properties, illegal);
    return;
}

void calendarHasDuplicateCalscale(void)
{
    strcat(damage, "Calendar duplicate CALSCALE");
    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(illegal->propName, "CALSCALE");
    strcpy(illegal->propDescr, "GREGORIAN");
    Property *illegal2 = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(illegal2->propName, "CALSCALE");
    strcpy(illegal2->propDescr, "GREGORIAN");
    insertBack(calendar->properties, illegal);
    insertBack(calendar->properties, illegal2);
    return;
}

void calendarHasDuplicateMethod(void)
{
    strcat(damage, "Calendar duplicate METHOD");
    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(illegal->propName, "METHOD");
    strcpy(illegal->propDescr, "GREGORIAN");
    Property *illegal2 = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(illegal2->propName, "METHOD");
    strcpy(illegal2->propDescr, "GREGORIAN");
    insertBack(calendar->properties, illegal);
    insertBack(calendar->properties, illegal2);
    return;
}

void calendarHasDuplicateProdid(void)
{
    strcat(damage, "Calendar duplicate PRODID");
    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(illegal->propName, "PRODID");
    strcpy(illegal->propDescr, "GREGORIAN");
    Property *illegal2 = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(illegal2->propName, "PRODID");
    strcpy(illegal2->propDescr, "GREGORIAN");
    insertBack(calendar->properties, illegal);
    insertBack(calendar->properties, illegal2);

    return;
}

void calendarHasDuplicateVersion(void)
{
    strcat(damage, "Calendar duplicate version");
    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(illegal->propName, "VERSION");
    strcpy(illegal->propDescr, "GREGORIAN");
    Property *illegal2 = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(illegal2->propName, "VERSION");
    strcpy(illegal2->propDescr, "GREGORIAN");
    insertBack(calendar->properties, illegal);
    insertBack(calendar->properties, illegal2);

    return;
}

void calendarPropNameEmpty(void)
{
    strcat(damage, "Calendar empty propName");
    if (getLength(calendar->properties) > 0)
    {
        Property *temp = getFromFront(calendar->properties);
        strcpy(temp->propName, "");
    }
    else
    {
        Property *blank = calloc(1, sizeof(Property) + sizeof(char) * 8);
        strcpy(blank->propName, "");
        strcpy(blank->propDescr, "HELLO");
        insertBack(calendar->properties, blank);
    }
    return;
}

void calendarPropNameExceeds(void)
{
    strcat(damage, "Calendar propName out of bounds");
    if (getLength(calendar->properties) > 0)
    {
        Property *temp = getFromFront(calendar->properties);
        for (int i = 0; i < 200; i++)
            (temp->propName)[i] = 'A';
    }
    else
    {
        Property *blank = calloc(1, sizeof(Property) + sizeof(char) * 8);
        for (int i = 0; i < 200; i++)
            (blank->propName)[i] = 'A';
        strcpy(blank->propDescr, "HELLO");
        insertBack(calendar->properties, blank);
    }
    return;
}

void calendarPropDescrBlank(void)
{
    strcat(damage, "Calendar propDescr blank");
    if (getLength(calendar->properties) > 0)
    {
        Property *temp = getFromFront(calendar->properties);
        strcpy(temp->propDescr, "");
    }
    else
    {
        Property *blank = calloc(1, sizeof(Property) + sizeof(char));
        strcpy(blank->propName, "CALSCALE");
        strcpy(blank->propDescr, "");
        insertBack(calendar->properties, blank);
    }
    return;
}
/* INV_EVENT */

void eventEmptyUID(void)
{
    strcat(damage, "Event empty UID");
    Event *event = (Event*)getFromFront(calendar->events);
    if (event != NULL)
    {
        event->UID[0] = '\0';
    }
    return;   
}

void eventUIDExceeds(void)
{
    strcat(damage, "Event UID exceeds array");
    Event *event = (Event*)getFromFront(calendar->events);
    for (int i = 0; i < 1000; i++)
        (event->UID)[i] = 'A';
    return;
}

void eventDuplicateUID(void)
{
    strcat(damage, "Event duplicate UID");
    Event *event = (Event*)getFromFront(calendar->events);
    Property *duplicateUID = calloc(1, sizeof(Property) + sizeof(char) * 6);
    strcpy(duplicateUID->propName, "UID");
    strcpy(duplicateUID->propDescr, "LMAO");
    insertBack(event->properties, duplicateUID);
    return;
}

void eventDuplicateDTSTART(void)
{
    strcat(damage, "Event duplicate DTSTART");
    Event *event = (Event*)getFromFront(calendar->events);
    Property *duplicateDT = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(duplicateDT->propName, "DTSTART");
    strcpy(duplicateDT->propDescr, "20151002T100000Z");
    insertBack(event->properties, duplicateDT);
    return;
}

void eventDuplicateDTSTAMP(void)
{
    strcat(damage, "Event duplicate DTSTAMP");
    Event *event = (Event*)getFromFront(calendar->events);
    Property *duplicateDT = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(duplicateDT->propName, "DTSTAMP");
    strcpy(duplicateDT->propDescr, "20161002T100000");
    insertBack(event->properties, duplicateDT);
    return;
}

void eventNullPropertyList(void)
{
    strcat(damage, "Event NULL property list");
    Event *event = (Event*)getFromFront(calendar->events);
    freeList(event->properties);
    event->properties = NULL;
    return;
}

void eventNullAlarmList(void)
{
    strcat(damage, "Event NULL alarm list");
    Event *event = (Event*)getFromFront(calendar->events);
    freeList(event->alarms);
    event->alarms = NULL;
    return;
}

void eventIllegalProperty(void)
{
    strcat(damage, "Event illegal property");
    Event *event = (Event*)getFromFront(calendar->events);
    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(illegal->propName, "ILLEGAL");
    strcpy(illegal->propDescr, "ILLEGALSTUFF");
    insertBack(event->properties, illegal);
    return;
}

void eventDuplicateProperty(void)
{
    strcat(damage, "Event duplicate property");
    Event *event = (Event*)getFromFront(calendar->events);
    Property *duplicate1 = NULL;
    Property *duplicate2 = NULL;

    duplicate1 = calloc(1, sizeof(Property) + sizeof(char) * 10);
    duplicate2 = calloc(1, sizeof(Property) + sizeof(char) * 10);

    strcpy(duplicate1->propName, "SUMMARY");
    strcpy(duplicate2->propName, "SUMMARY");
    strcpy(duplicate1->propDescr, "hello");
    strcpy(duplicate2->propDescr, "hello");

    insertBack(event->properties, duplicate1);
    insertBack(event->properties, duplicate2);
    return;
}

void eventViolatesExclusivity(void)
{
    strcat(damage, "Event violates exclusivity");
    Event *event  = (Event*)getFromFront(calendar->events);
    Property *DTEND = calloc(1, sizeof(Property) + sizeof(char) * 20);
    Property *DURATION = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(DTEND->propName, "DTEND");
    strcpy(DURATION->propName, "DURATION");
    strcpy(DTEND->propDescr, "TEST");
    strcpy(DURATION->propDescr, "TEST");

    insertBack(event->properties, DTEND);
    insertBack(event->properties, DURATION);
    return;
}

void eventPropNameBlank(void)
{
    strcat(damage, "Event propName blank");
    Event *event = (Event*)getFromFront(calendar->events);
    Property *blank = calloc(1, sizeof(Property) + sizeof(char) * 8);
    strcpy(blank->propName, "");
    strcpy(blank->propDescr, "HELLO");
    insertBack(event->properties, blank);
    return;
}

void eventPropNameExceeds(void)
{
    strcat(damage, "Event propName exceeds array");
    Event *event = (Event*)getFromFront(calendar->events);
    Property *blankProp = calloc(1, sizeof(Property) + sizeof(char) * 20);
    for (int i = 0; i < 200; i++)
        (blankProp->propName)[i] = 'A';
    strcpy(blankProp->propDescr, "HELLO");
    insertBack(event->properties, blankProp);
    return;
}

void eventPropDescrBlank(void)
{
    strcat(damage, "Event propDescr blank");
    Event *event = (Event*)getFromFront(calendar->events);
    Property *blankProp = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(blankProp->propName, "COMMENT");
    strcpy(blankProp->propDescr, "");
    insertBack(event->properties, blankProp);
    return;
}

void eventDTSTARTEmptyDate(void)
{
    strcat(damage, "Event DTSTART empty date");
    Event *event = (Event*)getFromFront(calendar->events);
    (event->startDateTime.date)[0] = '\0';
    return;
}

void eventDTSTARTDateExceeds(void)
{
    strcat(damage, "Event DTSTART date overflow");
    Event *event = (Event*)getFromFront(calendar->events);
    for (int i = 0; i < 9; i++)
        ((event->startDateTime).date)[i] = 'A';
    return;
}

void eventDTSTARTEmptyTime(void)
{
    strcat(damage, "Event DTSTART empty time");
    Event *event = (Event*)getFromFront(calendar->events);
    (event->startDateTime.time)[0] = '\0';
    return;
}

void eventDTSTARTTimeExceeds(void)
{
    strcat(damage, "Event DTSTART time overflow");
    Event *event = (Event*)getFromFront(calendar->events);
    for (int i = 0; i < 7; i++)
        ((event->startDateTime).time)[i] = 'A';
    return;
}

void eventDTSTAMPEmptyDate(void)
{
    strcat(damage, "Event DTSTAMP empty date");
    Event *event = (Event*)getFromFront(calendar->events);
    (event->creationDateTime.date)[0] = '\0';
    return;
}

void eventDTSTAMPDateExceeds(void)
{
    strcat(damage, "Event DTSTAMP date overflow");
    Event *event = (Event*)getFromFront(calendar->events);
    for (int i = 0; i < 9; i++)
        ((event->creationDateTime).date)[i] = 'A';

    return;
}

void eventDTSTAMPEmptyTime(void)
{
    strcat(damage, "Event DTSTAMP empty time");
    Event *event = (Event*)getFromFront(calendar->events);
    (event->creationDateTime.time)[0] = '\0';
    return;
}

void eventDTSTAMPTimeExceeds(void)
{
    strcat(damage, "Event DTSTAMP time overflow");
    Event *event = (Event*)getFromFront(calendar->events);
    for (int i = 0; i < 7; i++)
        ((event->creationDateTime).time)[i] = 'A';
    return;
}


/* INV_ALARM */

void alarmEmptyAction(void)
{
    strcat(damage, "Alarm empty action");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);
    alarm->action[0] = '\0';
    return;
}

void alarmActionOverflow(void)
{
    strcat(damage, "Alarm action overflow");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);
    for (int i = 0; i < 200; i++)
        alarm->action[i] = 'A';

    return;
}

void alarmInvalidAction(void)
{
    strcat(damage, "Alarm invalid action");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);
    strcpy(alarm->action, "DISPLAY");
    return;
}

void alarmNullTrigger(void)
{
    strcat(damage, "Alarm NULL trigger");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);
    free(alarm->trigger);
    alarm->trigger = NULL;
    return;
}

void alarmEmptyTrigger(void)
{
    strcat(damage, "Alarm empty trigger");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);
    free(alarm->trigger);
    alarm->trigger = calloc(1, sizeof(char) * 2);
    strcpy(alarm->trigger, "");
    return;
}

void alarmNullProperties(void)
{
    strcat(damage, "Alarm NULL properties");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);
    freeList(alarm->properties);
    alarm->properties = NULL;
    return;
}

void alarmDuplicateAction(void)
{
    strcat(damage, "Alarm duplicate ACTION");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);
    Property *toInsert = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(toInsert->propName, "ACTION");
    strcpy(toInsert->propDescr, "AUDIO");
    insertBack(alarm->properties, toInsert);
    return;
}

void alarmDuplicateTrigger(void)
{
    strcat(damage, "Alarm duplicate TRIGGER");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);
    Property *toInsert = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(toInsert->propName, "TRIGGER");
    strcpy(toInsert->propDescr, "-P15M");
    insertBack(alarm->properties, toInsert);
    return;
}

void alarmDuplicateDuration(void)
{
    strcat(damage, "Alarm duplicate DURATION");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);
    clearList(alarm->properties);

    Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);
    Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);

    strcpy(repeat->propName, "REPEAT");
    strcpy(repeat->propDescr, "4");

    strcpy(duration->propName, "DURATION");
    strcpy(duration->propDescr, "PT15M");

    strcpy(illegal->propName, "DURATION");
    strcpy(duration->propDescr, "PT20M");

    insertBack(alarm->properties, repeat);
    insertBack(alarm->properties, duration);
    insertBack(alarm->properties, illegal);
    return;
}

void alarmDuplicateRepeat(void)
{
    strcat(damage, "Alarm duplicate REPEAT");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);

    Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);
    Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);

    strcpy(repeat->propName, "REPEAT");
    strcpy(repeat->propDescr, "4");

    strcpy(duration->propName, "DURATION");
    strcpy(duration->propDescr, "PT15M");

    strcpy(illegal->propName, "REPEAT");
    strcpy(duration->propDescr, "5");

    insertBack(alarm->properties, repeat);
    insertBack(alarm->properties, duration);
    insertBack(alarm->properties, illegal);

    return;
}

void alarmDuplicateAttach(void)
{
    strcat(damage, "Alarm duplicate ATTACH");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);
    clearList(alarm->properties);

    Property *attach1 = calloc(1, sizeof(Property) + sizeof(char) * 100);
    Property *attach2 = calloc(1, sizeof(Property) + sizeof(char) * 100);

    strcpy(attach1->propName, "ATTACH");
    strcpy(attach1->propDescr, "FMTTYPE=audio/basic:ftp://example.com/pub");

    strcpy(attach2->propName, "ATTACH");
    strcpy(attach2->propDescr, "FMTTYPE=audio/basic:ftp://example2.com/pub");

    insertBack(alarm->properties, attach1);
    insertBack(alarm->properties, attach2);

    return;
}

void alarmDurationDisobeysInclusivity(void)
{
    strcat(damage, "Alarm duration disobeys inclusivity");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);
    clearList(alarm->properties);
    Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(duration->propName, "DURATION");
    strcpy(duration->propDescr, "PT15M");
    insertBack(alarm->properties, duration);
    return;
}

void alarmRepeatDisobeysInclusivity(void)
{
    strcat(damage, "Alarm repeat disobeys inclusivity");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);
    clearList(alarm->properties);
    Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(repeat->propName, "REPEAT");
    strcpy(repeat->propDescr, "4");
    insertBack(alarm->properties, repeat);
    return;
}

void alarmIllegalProperty(void)
{
    strcat(damage, "Alarm illegal property");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);
    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);
    strcpy(illegal->propName, "BEGIN");
    strcpy(illegal->propDescr, "NIKITENKO");
    insertBack(alarm->properties, illegal);
    return;
}

void alarmEmptyPropdescr(void)
{
    strcat(damage, "Alarm empty propDescr");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);
    clearList(alarm->properties);

    Property *attach1 = calloc(1, sizeof(Property) + sizeof(char) * 100);

    strcpy(attach1->propName, "");
    strcpy(attach1->propDescr, "FMTTYPE=audio/basic:ftp://example.com/pub");

    insertBack(alarm->properties, attach1);

    return;
}

void alarmPropnameOverflow(void)
{
    strcat(damage, "Alarm propName overflow");
    Event *event = (Event*)getFromFront(calendar->events);
    Alarm *alarm = (Alarm*)getFromFront(event->alarms);
    clearList(alarm->properties);

    Property *attach1 = calloc(1, sizeof(Property) + sizeof(char) * 100);
    for (int i = 0; i < 200; i++)
        (attach1->propName)[i] = 'A';
    strcpy(attach1->propDescr, "FMTTYPE=audio/basic:ftp://example.com/pub");

    insertBack(alarm->properties, attach1);
    return;
}

/* Main function */
int main(void)
{
    printf(CYA"----------INV_CAL----------\n"RESET);

    // empty prodID
    strcpy(damage, "Empty prodID");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        /* Just going to set the first character to 0 to signify empty string */
        (calendar->prodID)[0] = '\0';
        test(INV_CAL);
    }

    // prodID exceeds array length
    strcpy(damage, "ProdID exceeds array length");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        for (int i = 0; i < 1000; i++)
            (calendar->prodID)[i] = 'A';
        test(INV_CAL);
    }

    // Calendar has NULL event list 
    strcpy(damage, "Calendar has NULL event list");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        // this is my deleteCalendar code. Don't MOSS me fools
        ListIterator events = createIterator(calendar->events);
        void *event, *alarm;
        event = nextElement(&events);
        while ((event = nextElement(&events)) != NULL)
        {
            clearList(((Event*)event)->properties);
            ListIterator eventAlarms = createIterator(((Event*)event)->alarms);
            while (((alarm = nextElement(&eventAlarms)) != NULL))
                clearList(((Alarm*)alarm)->properties);
            clearList(((Event*)event)->alarms);
        }
        freeList(calendar->events);
        calendar->events = NULL;
        test(INV_CAL);
    }


    // Calendar has NULL property list
    strcpy(damage, "Calendar has NULL property list");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        freeList(calendar->properties);
        calendar->properties = NULL;
        test(INV_CAL);
    }

    // Calendar has empty event list
    strcpy(damage, "Calendar has empty event list");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        ListIterator events = createIterator(calendar->events);
        void *event, *alarm;
        event = nextElement(&events);
        while ((event = nextElement(&events)) != NULL)
        {
            clearList(((Event*)event)->properties);
            ListIterator eventAlarms = createIterator(((Event*)event)->alarms);
            while (((alarm = nextElement(&eventAlarms)) != NULL))
                clearList(((Alarm*)alarm)->properties);
            clearList(((Event*)event)->alarms);
        }
        clearList(calendar->events);
        test(INV_CAL);
    }

    // Calendar has illegal component tag: (various)
    char *contraband[] =
    {
        "VTODO", "VJOURNAL", "VFREEBUSY", "VTIMEZONE", "VALARM", "TRASH", "VCALENDAR", "VEVENT", "INTINTINTINTINT", "NIKITENKO", "ROXANNE", NULL
    };
    for (int i = 0; contraband[i] != NULL; i++)
    {
        sprintf(damage, "Calendar has illegal component tag: BEGIN:%s", contraband[i]);
        load("TEST/OK/validation.ics");
        if (prime() == 1) 
        {
            size_t len = strlen(contraband[i]);
            Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * len + 1);
            strcpy(illegal->propName, "BEGIN");
            strcpy(illegal->propDescr, contraband[i]);
            insertBack(calendar->properties, illegal);
            test(INV_CAL);
        }

        sprintf(damage, "Calendar has illegal component tag: END:%s", contraband[i]);
        load("TEST/OK/validation.ics");
        if (prime() == 1) 
        {
            size_t len = strlen(contraband[i]);
            Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * len + 1);
            strcpy(illegal->propName, "END");
            strcpy(illegal->propDescr, contraband[i]);
            insertBack(calendar->properties, illegal);
            test(INV_CAL);
        }

    }

    // Calendar has duplicate (various) property
    char *legalProps[] =
    {
        "CALSCALE", "METHOD", "PRODID", "VERSION", NULL
    };

    for (int i = 0; legalProps[i] != NULL; i++)
    {
        /* Independent duplicates: I add two of each of the above. */
        sprintf(damage, "Calendar has duplicate %s property", legalProps[i]);
        load("TEST/OK/validation.ics");
        if (prime() == 1) 
        {
            Property *duplicate = calloc(1, sizeof(Property) + sizeof(char) * 6);
            strcpy(duplicate->propName, legalProps[i]);
            strcpy(duplicate->propDescr, "TEST");

            /* I understand it's a little weird to have a VERSION:TEST and
             * that it's invalid in A1, but as long as propDescr is not an
             * empty string, we will treat such a struct as valid. */

            Property *duplicate2 = calloc(1, sizeof(Property) + sizeof(char) * 6);
            strcpy(duplicate2->propName, legalProps[i]);
            strcpy(duplicate2->propDescr, "TEST");

            insertBack(calendar->properties, duplicate);
            insertBack(calendar->properties, duplicate2);

            test(INV_CAL);
        }
    }


    // Calendar has internal duplicate (various) property
    char *internalDuplicates[] =
    {
        "VERSION", "PRODID", NULL
    };

    for (int i = 0; internalDuplicates[i] != NULL; i++)
    {
        /* Internal duplicates: assuming there's already a VERSION
         * and PRODID, this should be harder to detect (albeit possible) */
        sprintf(damage, "Calendar has internal duplicate %s property", internalDuplicates[i]);
        load("TEST/OK/validation.ics");
        if (prime() == 1) 
        {
            Property *duplicate = calloc(1, sizeof(Property) + sizeof(char) * 6);
            strcpy(duplicate->propName, internalDuplicates[i]);
            strcpy(duplicate->propDescr, "TEST");

            insertBack(calendar->properties, duplicate);

            test(INV_CAL);
        }
    }

    // Calendar has blank propName (front)
    strcpy(damage, "Calendar has blank propName (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        if (getLength(calendar->properties) > 0)
        {
            Property *temp = getFromFront(calendar->properties);
            strcpy(temp->propName, "");
        }
        else
        {
            Property *blank = calloc(1, sizeof(Property) + sizeof(char) * 8);
            strcpy(blank->propName, "");
            strcpy(blank->propDescr, "HELLO");
            insertBack(calendar->properties, blank);
        }
        test(INV_CAL);
    }

    // Calendar has blank propName (back)
    strcpy(damage, "Calendar has blank propName (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        if (getLength(calendar->properties) > 0)
        {
            Property *temp = getFromBack(calendar->properties);
            strcpy(temp->propName, "");
        }
        else
        {
            Property *blank = calloc(1, sizeof(Property) + sizeof(char) * 8);
            strcpy(blank->propName, "");
            strcpy(blank->propDescr, "HELLO");
            insertBack(calendar->properties, blank);
        }
        test(INV_CAL);
    }

    // Calendar propName exceeds array length (front)
    strcpy(damage, "Calendar propName exceeds array length (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        if (getLength(calendar->properties) > 0)
        {
            Property *temp = getFromFront(calendar->properties);
            for (int i = 0; i < 200; i++)
                (temp->propName)[i] = 'A';
        }
        else
        {
            Property *blank = calloc(1, sizeof(Property) + sizeof(char) * 8);
            for (int i = 0; i < 200; i++)
                (blank->propName)[i] = 'A';
            strcpy(blank->propDescr, "HELLO");
            insertBack(calendar->properties, blank);
        }
        test(INV_CAL);
    }

    // Calendar propname exceeds array length (back)
    strcpy(damage, "Calendar propName exceeds array length (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        if (getLength(calendar->properties) > 0)
        {
            Property *temp = getFromBack(calendar->properties);
            for (int i = 0; i < 200; i++)
                (temp->propName)[i] = 'A';
        }
        else
        {
            Property *blank = calloc(1, sizeof(Property) + sizeof(char) * 8);
            for (int i = 0; i < 200; i++)
                (blank->propName)[i] = 'A';
            strcpy(blank->propDescr, "HELLO");
            insertBack(calendar->properties, blank);
        }
        test(INV_CAL);
    }

    // Calendar has blank propDescr (front)
    strcpy(damage, "Calendar has blank propDescr (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        if (getLength(calendar->properties) > 0)
        {
            Property *temp = getFromFront(calendar->properties);
            strcpy(temp->propDescr, "");
        }
        else
        {
            Property *blank = calloc(1, sizeof(Property) + sizeof(char));
            strcpy(blank->propName, "CALSCALE");
            strcpy(blank->propDescr, "");
            insertBack(calendar->properties, blank);
        }
        test(INV_CAL);
    }

    // Calendar has blank propDescr (back)
    strcpy(damage, "Calendar has blank propDescr (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        if (getLength(calendar->properties) > 0)
        {
            Property *temp = getFromBack(calendar->properties);
            strcpy(temp->propDescr, "");

        }
        else
        {
            Property *blank = calloc(1, sizeof(Property) + sizeof(char));
            strcpy(blank->propName, "CALSCALE");
            strcpy(blank->propDescr, "");
            insertBack(calendar->properties, blank);
        }
        test(INV_CAL);
    }
    
    /* Creating INV_CAL score */
    cal_n = n; n = 0;
    cal_d = d; d = 0;
    printf(CYA"----------INV_EVENT----------\n"RESET);

    // Empty event UID (front)
    strcpy(damage, "Empty event UID (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            event->UID[0] = '\0';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Empty event UID (back)
    strcpy(damage, "Empty event UID (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            event->UID[0] = '\0';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event UID exceeds length (front)
    strcpy(damage, "Event UID exceeds length (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            for (int i = 0; i < 1000; i++)
                (event->UID)[i] = 'A';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event UID exceeds length (back)
    strcpy(damage, "Event UID exceeds length (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            for (int i = 0; i < 1000; i++)
                (event->UID)[i] = 'A';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Duplicate UID (front)
    strcpy(damage, "Duplicate UID (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Property *duplicateUID = calloc(1, sizeof(Property) + sizeof(char) * 6);
            strcpy(duplicateUID->propName, "UID");
            strcpy(duplicateUID->propDescr, "LMAO");
            insertBack(event->properties, duplicateUID);
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Duplicate UID (back)
    strcpy(damage, "Duplicate UID (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Property *duplicateUID = calloc(1, sizeof(Property) + sizeof(char) * 6);
            strcpy(duplicateUID->propName, "UID");
            strcpy(duplicateUID->propDescr, "LMAO");
            insertBack(event->properties, duplicateUID);
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Duplicate DTSTART (front)
    strcpy(damage, "Duplicate DTSTART (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Property *duplicateDT = calloc(1, sizeof(Property) + sizeof(char) * 20);
            strcpy(duplicateDT->propName, "DTSTART");
            strcpy(duplicateDT->propDescr, "20151002T100000Z");
            insertBack(event->properties, duplicateDT);
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Duplicate DTSTART (back)
    strcpy(damage, "Duplicate DTSTART (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Property *duplicateDT = calloc(1, sizeof(Property) + sizeof(char) * 20);
            strcpy(duplicateDT->propName, "DTSTART");
            strcpy(duplicateDT->propDescr, "20151002T100000Z");
            insertBack(event->properties, duplicateDT);
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Duplicate DTSTAMP (front)
    strcpy(damage, "Duplicate DTSTAMP (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Property *duplicateDT = calloc(1, sizeof(Property) + sizeof(char) * 20);
            strcpy(duplicateDT->propName, "DTSTAMP");
            strcpy(duplicateDT->propDescr, "20161002T100000");
            insertBack(event->properties, duplicateDT);
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Duplicate DTSTAMP (back)
    strcpy(damage, "Duplicate DTSTAMP (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Property *duplicateDT = calloc(1, sizeof(Property) + sizeof(char) * 20);
            strcpy(duplicateDT->propName, "DTSTAMP");
            strcpy(duplicateDT->propDescr, "20161002T100000");
            insertBack(event->properties, duplicateDT);
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event has NULL property list (front)
    strcpy(damage, "Event has NULL property list (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            freeList(event->properties);
            event->properties = NULL;
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event has NULL property list (back)
    strcpy(damage, "Event has NULL property list (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            freeList(event->properties);
            event->properties = NULL;
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event has NULL alarm list (front)
    strcpy(damage, "Event has NULL alarm list (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            freeList(event->alarms);
            event->alarms = NULL;
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event has NULL alarm list (back)
    strcpy(damage, "Event has NULL alarm list (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            freeList(event->alarms);
            event->alarms = NULL;
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event has illegal property: (various)
    char *illegalEventProps[] =
    {
        "CUMDUMPSTER", "AIDSWAFFLE", "BEGIN", "END", "LONGBOY", NULL
    };

    char *illegalEventPropsDescrs[] =
    {
        "VCALENDAR", "VALARM", "VEVENT", "VTODO", "VJOURNAL", NULL
    };

    for (int i = 0; illegalEventProps[i] != NULL; i++)
    {
        for(int j = 0; illegalEventPropsDescrs[j] != NULL; j++)
        {
            sprintf(damage, "Event has illegal property: %s:%s (front)",
                    illegalEventProps[i], illegalEventPropsDescrs[j]);
            load("TEST/OK/validation.ics");
            if (prime() == 1) 
            {
                Event *event = (Event*)getFromFront(calendar->events);
                if (event != NULL)
                {
                    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    strcpy(illegal->propName, illegalEventProps[i]);
                    strcpy(illegal->propDescr, illegalEventPropsDescrs[j]);
                    insertBack(event->properties, illegal);
                    test(INV_EVENT);
                }
                else
                {
                    printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
            }

            sprintf(damage, "Event has illegal property: %s:%s (back)",
                    illegalEventProps[i], illegalEventPropsDescrs[j]);
            load("TEST/OK/validation.ics");
            if (prime() == 1) 
            {
                Event *event = (Event*)getFromBack(calendar->events);
                if (event != NULL)
                {
                    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    strcpy(illegal->propName, illegalEventProps[i]);
                    strcpy(illegal->propDescr, illegalEventPropsDescrs[j]);
                    insertBack(event->properties, illegal);
                    test(INV_EVENT);
                }
                else
                {
                    printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
            }
        }
    }

    // Event has duplicate (various) property (front) / (back)
    char* restrictedEventProps[] =
    {
        "CLASS", "CREATED", "DESCRIPTION", "GEO", "LAST-MODIFIED", "LOCATION",
        "ORGANIZER", "PRIORITY", "SEQUENCE", "STATUS", "SUMMARY", "TRANSP",
        "URL", "RECURRENCE-ID", NULL
    };

    for (int i = 0; restrictedEventProps[i] != NULL; i++)
    {
        sprintf(damage, "Event has duplicate %s property (front)", restrictedEventProps[i]);
        load("TEST/OK/validation.ics");
        if (prime() == 1) 
        {
            Event *event = (Event*)getFromFront(calendar->events);
            if (event != NULL)
            {
                Property *duplicate1 = NULL;
                Property *duplicate2 = NULL;

                duplicate1 = calloc(1, sizeof(Property) + sizeof(char) * 10);
                duplicate2 = calloc(1, sizeof(Property) + sizeof(char) * 10);

                strcpy(duplicate1->propName, restrictedEventProps[i]);
                strcpy(duplicate2->propName, restrictedEventProps[i]);
                strcpy(duplicate1->propDescr, "hello");
                strcpy(duplicate2->propDescr, "hello");

                insertBack(event->properties, duplicate1);
                insertBack(event->properties, duplicate2);

                test(INV_EVENT);
            }
            else
            {
                printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
                d++;

            }
        }

        sprintf(damage, "Event has duplicate %s property (back)", restrictedEventProps[i]);
        load("TEST/OK/validation.ics");
        if (prime() == 1) 
        {
            Event *event = (Event*)getFromBack(calendar->events);
            if (event != NULL)
            {
                Property *duplicate1 = NULL;
                Property *duplicate2 = NULL;

                duplicate1 = calloc(1, sizeof(Property) + sizeof(char) * 10);
                duplicate2 = calloc(1, sizeof(Property) + sizeof(char) * 10);

                strcpy(duplicate1->propName, restrictedEventProps[i]);
                strcpy(duplicate2->propName, restrictedEventProps[i]);
                strcpy(duplicate1->propDescr, "hello");
                strcpy(duplicate2->propDescr, "hello");

                insertBack(event->properties, duplicate1);
                insertBack(event->properties, duplicate2);

                test(INV_EVENT);
            }
            else
            {
                printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
                d++;

            }
        }
    }

    // Event violates DTEND / DURATION exclusivity
    strcpy(damage, "Event violates DTEND / DURATION exclusivity (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Property *DTEND = calloc(1, sizeof(Property) + sizeof(char) * 20);
            Property *DURATION = calloc(1, sizeof(Property) + sizeof(char) * 20);
            strcpy(DTEND->propName, "DTEND");
            strcpy(DURATION->propName, "DURATION");
            strcpy(DTEND->propDescr, "TEST");
            strcpy(DURATION->propDescr, "TEST");

            insertBack(event->properties, DTEND);
            insertBack(event->properties, DURATION);

            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event violates DTEND / DURATION exclusivity
    strcpy(damage, "Event violates DTEND / DURATION exclusivity (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Property *DTEND = calloc(1, sizeof(Property) + sizeof(char) * 20);
            Property *DURATION = calloc(1, sizeof(Property) + sizeof(char) * 20);
            strcpy(DTEND->propName, "DTEND");
            strcpy(DURATION->propName, "DURATION");
            strcpy(DTEND->propDescr, "TEST");
            strcpy(DURATION->propDescr, "TEST");

            insertBack(event->properties, DTEND);
            insertBack(event->properties, DURATION);

            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event propName blank (front)
    strcpy(damage, "Event propName blank (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Property *blankProp = calloc(1, sizeof(Property) + sizeof(char) * 20);
            strcpy(blankProp->propName, "");
            strcpy(blankProp->propDescr, "HELLO");
            insertBack(event->properties, blankProp);
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event propName blank (back)
    strcpy(damage, "Event propName blank (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Property *blankProp = calloc(1, sizeof(Property) + sizeof(char) * 20);
            for (int i = 0; i < 200; i++)
                (blankProp->propName)[i] = 'A';
            strcpy(blankProp->propDescr, "HELLO");
            insertBack(event->properties, blankProp);
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event propName exceeds array (front)
    strcpy(damage, "Event propName exceeds array (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Property *blankProp = calloc(1, sizeof(Property) + sizeof(char) * 20);
            for (int i = 0; i < 200; i++)
                (blankProp->propName)[i] = 'A';
            strcpy(blankProp->propDescr, "HELLO");
            insertBack(event->properties, blankProp);
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event propName exceeds array (back)
    strcpy(damage, "Event propName exceeds array (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Property *blankProp = calloc(1, sizeof(Property) + sizeof(char) * 20);
            strcpy(blankProp->propName, "");
            strcpy(blankProp->propDescr, "HELLO");
            insertBack(event->properties, blankProp);
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event propDescr blank (front)
    strcpy(damage, "Event propDescr blank (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Property *blankProp = calloc(1, sizeof(Property) + sizeof(char) * 20);
            strcpy(blankProp->propName, "COMMENT");
            strcpy(blankProp->propDescr, "");
            insertBack(event->properties, blankProp);
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event propDescr blank (front)
    strcpy(damage, "Event propDescr blank (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Property *blankProp = calloc(1, sizeof(Property) + sizeof(char) * 20);
            strcpy(blankProp->propName, "COMMENT");
            strcpy(blankProp->propDescr, "");
            insertBack(event->properties, blankProp);
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event DTSTART empty date (front)
    strcpy(damage, "Event DTSTART empty date (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            (event->startDateTime.date)[0] = '\0';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event DTSTART empty date (back)
    strcpy(damage, "Event DTSTART empty date (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            (event->startDateTime.date)[0] = '\0';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event DTSTART date exceeds array (front)
    strcpy(damage, "Event DTSTART date exceeds array (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            for (int i = 0; i < 9; i++)
                ((event->startDateTime).date)[i] = 'A';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event DTSTART date exceeds array (back)
    strcpy(damage, "Event DTSTART date exceeds array (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            for (int i = 0; i < 9; i++)
                ((event->startDateTime).date)[i] = 'A';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }
    
    // Event DTSTART empty time (front)
    strcpy(damage, "Event DTSTART empty time (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            (event->startDateTime.time)[0] = '\0';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event DTSTART empty time (back)
    strcpy(damage, "Event DTSTART empty time (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            (event->startDateTime.time)[0] = '\0';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event DTSTART time exceeds array (front)
    strcpy(damage, "Event DTSTART time exceeds array (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            for (int i = 0; i < 7; i++)
                ((event->startDateTime).time)[i] = 'A';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event DTSTART time exceeds array (back)
    strcpy(damage, "Event DTSTART time exceeds array (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            for (int i = 0; i < 7; i++)
                ((event->startDateTime).time)[i] = 'A';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }
    
    // Event DTSTAMP empty date (front)
    strcpy(damage, "Event DTSTAMP empty date (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            (event->creationDateTime.date)[0] = '\0';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event DTSTAMP empty date (back)
    strcpy(damage, "Event DTSTAMP empty date (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            (event->creationDateTime.date)[0] = '\0';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event DTSTAMP date exceeds array (front)
    strcpy(damage, "Event DTSTAMP date exceeds array (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            for (int i = 0; i < 9; i++)
                ((event->creationDateTime).date)[i] = 'A';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event DTSTAMP date exceeds array (back)
    strcpy(damage, "Event DTSTAMP date exceeds array (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            for (int i = 0; i < 9; i++)
                ((event->creationDateTime).date)[i] = 'A';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event DTSTAMP empty time (front)
    strcpy(damage, "Event DTSTAMP empty time (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            (event->creationDateTime.time)[0] = '\0';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event DTSTAMP empty time (back)
    strcpy(damage, "Event DTSTAMP empty time (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            (event->creationDateTime.time)[0] = '\0';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event DTSTAMP time exceeds array (front)
    strcpy(damage, "Event DTSTAMP time exceeds array (front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            for (int i = 0; i < 7; i++)
                ((event->creationDateTime).time)[i] = 'A';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Event DTSTAMP time exceeds array (back)
    strcpy(damage, "Event DTSTAMP time exceeds array (back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            for (int i = 0; i < 7; i++)
                ((event->creationDateTime).time)[i] = 'A';
            test(INV_EVENT);
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    /* Creating INV_EVENT score */
    evt_n = n; n = 0;
    evt_d = d; d = 0;

    printf(CYA"----------INV_ALARM----------\n"RESET);

    // Empty alarm action (front, front)
    strcpy(damage, "Empty alarm action (front, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                strcpy(alarm->action, "");
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Empty alarm action (front, back)
    strcpy(damage, "Empty alarm action (front, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                strcpy(alarm->action, "");
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Empty alarm action (back, back)
    strcpy(damage, "Empty alarm action (back, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                strcpy(alarm->action, "");
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Empty alarm action (back, front)
    strcpy(damage, "Empty alarm action (back, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                strcpy(alarm->action, "");
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm action exceeds array (front, front)
    strcpy(damage, "Alarm action exceeds array (front, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                for (int i = 0; i < 200; i++)
                    alarm->action[i] = 'A';
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }
    
    // Alarm action exceeds array (front, back)
    strcpy(damage, "Alarm action exceeds array (front, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                for (int i = 0; i < 200; i++)
                    alarm->action[i] = 'A';
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm action exceeds array (back, back)
    strcpy(damage, "Alarm action exceeds array (back, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                for (int i = 0; i < 200; i++)
                    alarm->action[i] = 'A';
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm action exceeds array (back, front)
    strcpy(damage, "Alarm action exceeds array (back, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                for (int i = 0; i < 200; i++)
                    alarm->action[i] = 'A';
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    /*
    // Alarm action not audioprop (front, front)
    strcpy(damage, "Alarm action not audioprop (front, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                strcpy(alarm->action, "DISPLAY");
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm action not audioprop (front, back)
    strcpy(damage, "Alarm action not audioprop (front, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                strcpy(alarm->action, "DISPLAY");
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm action not audioprop (back, back)
    strcpy(damage, "Alarm action not audioprop (back, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                strcpy(alarm->action, "DISPLAY");
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm action not audioprop (back, front)
    strcpy(damage, "Alarm action not audioprop (back, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                strcpy(alarm->action, "DISPLAY");
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }
    */

    // NULL Alarm trigger (front, front)
    strcpy(damage, "NULL alarm trigger (front, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->trigger == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm trigger...createCalendar defective. Test aborted.\n"RESET);
                }
                else
                {
                    free(alarm->trigger);
                    alarm->trigger = NULL;
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // NULL Alarm trigger (front, back)
    strcpy(damage, "NULL alarm trigger (front, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->trigger == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm trigger...createCalendar defective. Test aborted.\n"RESET);
                }
                else
                {
                    free(alarm->trigger);
                    alarm->trigger = NULL;
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // NULL Alarm trigger (back, back)
    strcpy(damage, "NULL alarm trigger (back, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->trigger == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm trigger...createCalendar defective. Test aborted.\n"RESET);
                }
                else
                {
                    free(alarm->trigger);
                    alarm->trigger = NULL;
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // NULL Alarm trigger (back, front)
    strcpy(damage, "NULL alarm trigger (back, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->trigger == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm trigger...createCalendar defective. Test aborted.\n"RESET);
                }
                else
                {
                    free(alarm->trigger);
                    alarm->trigger = NULL;
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // EMPTY Alarm trigger (front, front)
    strcpy(damage, "EMPTY alarm trigger (front, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->trigger == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm trigger...createCalendar defective. Test aborted.\n"RESET);
                }
                else
                {
                    free(alarm->trigger);
                    alarm->trigger = calloc(1, sizeof(char));
                    strcpy(alarm->trigger, "");
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // EMPTY Alarm trigger (front, back)
    strcpy(damage, "EMPTY alarm trigger (front, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->trigger == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm trigger...createCalendar defective. Test aborted.\n"RESET);
                }
                else
                {
                    free(alarm->trigger);
                    alarm->trigger = calloc(1, sizeof(char));
                    strcpy(alarm->trigger, "");
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // EMPTY Alarm trigger (back, back)
    strcpy(damage, "EMPTY alarm trigger (back, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->trigger == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm trigger...createCalendar defective. Test aborted.\n"RESET);
                }
                else
                {
                    free(alarm->trigger);
                    alarm->trigger = calloc(1, sizeof(char));
                    strcpy(alarm->trigger, "");
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // EMPTY Alarm trigger (back, front)
    strcpy(damage, "EMPTY alarm trigger (back, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->trigger == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm trigger...createCalendar defective. Test aborted.\n"RESET);
                }
                else
                {
                    free(alarm->trigger);
                    alarm->trigger = calloc(1, sizeof(char));
                    strcpy(alarm->trigger, "");
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }
    
    // NULL alarm properties list (front, front)
    strcpy(damage, "NULL alarm properties list (front, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    freeList(alarm->properties);
                    alarm->properties = NULL;
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // NULL alarm properties list (front, back)
    strcpy(damage, "NULL alarm properties list (front, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    freeList(alarm->properties);
                    alarm->properties = NULL;
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // NULL alarm properties list (back, back)
    strcpy(damage, "NULL alarm properties list (back, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    freeList(alarm->properties);
                    alarm->properties = NULL;
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // NULL alarm properties list (back, front)
    strcpy(damage, "NULL alarm properties list (back, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    freeList(alarm->properties);
                    alarm->properties = NULL;
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }
    
    // Alarm has duplicate ACTION (front, front)
    strcpy(damage, "Alarm has duplicate ACTION (front, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    Property *toInsert = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    strcpy(toInsert->propName, "ACTION");
                    strcpy(toInsert->propDescr, "AUDIO");
                    insertBack(alarm->properties, toInsert);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate ACTION (front, back)
    strcpy(damage, "Alarm has duplicate ACTION (front, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    Property *toInsert = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    strcpy(toInsert->propName, "ACTION");
                    strcpy(toInsert->propDescr, "AUDIO");
                    insertBack(alarm->properties, toInsert);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate ACTION (back, back)
    strcpy(damage, "Alarm has duplicate ACTION (back, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    Property *toInsert = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    strcpy(toInsert->propName, "ACTION");
                    strcpy(toInsert->propDescr, "AUDIO");
                    insertBack(alarm->properties, toInsert);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate ACTION (back, front)
    strcpy(damage, "Alarm has duplicate ACTION (back, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    Property *toInsert = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    strcpy(toInsert->propName, "ACTION");
                    strcpy(toInsert->propDescr, "AUDIO");
                    insertBack(alarm->properties, toInsert);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }
    
    // Alarm has duplicate TRIGGER (front, front)
    strcpy(damage, "Alarm has duplicate TRIGGER (front, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    Property *toInsert = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    strcpy(toInsert->propName, "TRIGGER");
                    strcpy(toInsert->propDescr, "-P15M");
                    insertBack(alarm->properties, toInsert);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate TRIGGER (front, back)
    strcpy(damage, "Alarm has duplicate TRIGGER (front, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    Property *toInsert = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    strcpy(toInsert->propName, "TRIGGER");
                    strcpy(toInsert->propDescr, "-P15M");
                    insertBack(alarm->properties, toInsert);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate TRIGGER (back, back)
    strcpy(damage, "Alarm has duplicate TRIGGER (back, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    Property *toInsert = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    strcpy(toInsert->propName, "TRIGGER");
                    strcpy(toInsert->propDescr, "-P15M");
                    insertBack(alarm->properties, toInsert);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate TRIGGER (back, front)
    strcpy(damage, "Alarm has duplicate TRIGGER (back, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    Property *toInsert = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    strcpy(toInsert->propName, "TRIGGER");
                    strcpy(toInsert->propDescr, "-P15M");
                    insertBack(alarm->properties, toInsert);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate DURATION (front, front)
    strcpy(damage, "Alarm has duplicate DURATION (front, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);
                    
                    Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);

                    strcpy(repeat->propName, "REPEAT");
                    strcpy(repeat->propDescr, "4");

                    strcpy(duration->propName, "DURATION");
                    strcpy(duration->propDescr, "PT15M");

                    strcpy(illegal->propName, "DURATION");
                    strcpy(duration->propDescr, "PT20M");

                    insertBack(alarm->properties, repeat);
                    insertBack(alarm->properties, duration);
                    insertBack(alarm->properties, illegal);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate DURATION (front, back)
    strcpy(damage, "Alarm has duplicate DURATION (front, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);

                    strcpy(repeat->propName, "REPEAT");
                    strcpy(repeat->propDescr, "4");

                    strcpy(duration->propName, "DURATION");
                    strcpy(duration->propDescr, "PT15M");

                    strcpy(illegal->propName, "DURATION");
                    strcpy(duration->propDescr, "PT20M");

                    insertBack(alarm->properties, repeat);
                    insertBack(alarm->properties, duration);
                    insertBack(alarm->properties, illegal);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate DURATION (back, back)
    strcpy(damage, "Alarm has duplicate DURATION (back, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);

                    strcpy(repeat->propName, "REPEAT");
                    strcpy(repeat->propDescr, "4");

                    strcpy(duration->propName, "DURATION");
                    strcpy(duration->propDescr, "PT15M");

                    strcpy(illegal->propName, "DURATION");
                    strcpy(duration->propDescr, "PT20M");

                    insertBack(alarm->properties, repeat);
                    insertBack(alarm->properties, duration);
                    insertBack(alarm->properties, illegal);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate DURATION (back, front)
    strcpy(damage, "Alarm has duplicate DURATION (back, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);

                    strcpy(repeat->propName, "REPEAT");
                    strcpy(repeat->propDescr, "4");

                    strcpy(duration->propName, "DURATION");
                    strcpy(duration->propDescr, "PT15M");

                    strcpy(illegal->propName, "DURATION");
                    strcpy(duration->propDescr, "PT20M");

                    insertBack(alarm->properties, repeat);
                    insertBack(alarm->properties, duration);
                    insertBack(alarm->properties, illegal);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate REPEAT (front, front)
    strcpy(damage, "Alarm has duplicate REPEAT (front, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);

                    strcpy(repeat->propName, "REPEAT");
                    strcpy(repeat->propDescr, "4");

                    strcpy(duration->propName, "DURATION");
                    strcpy(duration->propDescr, "PT15M");

                    strcpy(illegal->propName, "REPEAT");
                    strcpy(duration->propDescr, "5");

                    insertBack(alarm->properties, repeat);
                    insertBack(alarm->properties, duration);
                    insertBack(alarm->properties, illegal);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate REPEAT (front, back)
    strcpy(damage, "Alarm has duplicate REPEAT (front, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);

                    strcpy(repeat->propName, "REPEAT");
                    strcpy(repeat->propDescr, "4");

                    strcpy(duration->propName, "DURATION");
                    strcpy(duration->propDescr, "PT15M");

                    strcpy(illegal->propName, "REPEAT");
                    strcpy(duration->propDescr, "5");

                    insertBack(alarm->properties, repeat);
                    insertBack(alarm->properties, duration);
                    insertBack(alarm->properties, illegal);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate REPEAT (back, back)
    strcpy(damage, "Alarm has duplicate REPEAT (back, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);

                    strcpy(repeat->propName, "REPEAT");
                    strcpy(repeat->propDescr, "4");

                    strcpy(duration->propName, "DURATION");
                    strcpy(duration->propDescr, "PT15M");

                    strcpy(illegal->propName, "REPEAT");
                    strcpy(duration->propDescr, "5");

                    insertBack(alarm->properties, repeat);
                    insertBack(alarm->properties, duration);
                    insertBack(alarm->properties, illegal);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate REPEAT (back, front)
    strcpy(damage, "Alarm has duplicate REPEAT (back, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
                    Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);

                    strcpy(repeat->propName, "REPEAT");
                    strcpy(repeat->propDescr, "4");

                    strcpy(duration->propName, "DURATION");
                    strcpy(duration->propDescr, "PT15M");

                    strcpy(illegal->propName, "REPEAT");
                    strcpy(duration->propDescr, "6");

                    insertBack(alarm->properties, repeat);
                    insertBack(alarm->properties, duration);
                    insertBack(alarm->properties, illegal);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate ATTACH (front, front)
    strcpy(damage, "Alarm has duplicate ATTACH (front, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *attach1 = calloc(1, sizeof(Property) + sizeof(char) * 100);
                    Property *attach2 = calloc(1, sizeof(Property) + sizeof(char) * 100);

                    strcpy(attach1->propName, "ATTACH");
                    strcpy(attach1->propDescr, "FMTTYPE=audio/basic:ftp://example.com/pub");

                    strcpy(attach2->propName, "ATTACH");
                    strcpy(attach2->propDescr, "FMTTYPE=audio/basic:ftp://example2.com/pub");

                    insertBack(alarm->properties, attach1);
                    insertBack(alarm->properties, attach2);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate ATTACH (front, back)
    strcpy(damage, "Alarm has duplicate ATTACH (front, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *attach1 = calloc(1, sizeof(Property) + sizeof(char) * 100);
                    Property *attach2 = calloc(1, sizeof(Property) + sizeof(char) * 100);

                    strcpy(attach1->propName, "ATTACH");
                    strcpy(attach1->propDescr, "FMTTYPE=audio/basic:ftp://example.com/pub");

                    strcpy(attach2->propName, "ATTACH");
                    strcpy(attach2->propDescr, "FMTTYPE=audio/basic:ftp://example2.com/pub");

                    insertBack(alarm->properties, attach1);
                    insertBack(alarm->properties, attach2);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate ATTACH (back, back)
    strcpy(damage, "Alarm has duplicate ATTACH (back, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *attach1 = calloc(1, sizeof(Property) + sizeof(char) * 100);
                    Property *attach2 = calloc(1, sizeof(Property) + sizeof(char) * 100);

                    strcpy(attach1->propName, "ATTACH");
                    strcpy(attach1->propDescr, "FMTTYPE=audio/basic:ftp://example.com/pub");

                    strcpy(attach2->propName, "ATTACH");
                    strcpy(attach2->propDescr, "FMTTYPE=audio/basic:ftp://example2.com/pub");

                    insertBack(alarm->properties, attach1);
                    insertBack(alarm->properties, attach2);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has duplicate ATTACH (back, front)
    strcpy(damage, "Alarm has duplicate ATTACH (back, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *attach1 = calloc(1, sizeof(Property) + sizeof(char) * 100);
                    Property *attach2 = calloc(1, sizeof(Property) + sizeof(char) * 100);

                    strcpy(attach1->propName, "ATTACH");
                    strcpy(attach1->propDescr, "FMTTYPE=audio/basic:ftp://example.com/pub");

                    strcpy(attach2->propName, "ATTACH");
                    strcpy(attach2->propDescr, "FMTTYPE=audio/basic:ftp://example2.com/pub");

                    insertBack(alarm->properties, attach1);
                    insertBack(alarm->properties, attach2);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }
    
    // Alarm DURATION disobeys inclusivity (front, front)
    strcpy(damage, "Alarm DURATION disobeys inclusivity (front, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                clearList(alarm->properties);
                Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
                strcpy(duration->propName, "DURATION");
                strcpy(duration->propDescr, "PT15M");
                insertBack(alarm->properties, duration);
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm DURATION disobeys inclusivity (front, back)
    strcpy(damage, "Alarm DURATION disobeys inclusivity (front, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                clearList(alarm->properties);
                Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
                strcpy(duration->propName, "DURATION");
                strcpy(duration->propDescr, "PT15M");
                insertBack(alarm->properties, duration);
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm DURATION disobeys inclusivity (back, back)
    strcpy(damage, "Alarm DURATION disobeys inclusivity (back, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                clearList(alarm->properties);
                Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
                strcpy(duration->propName, "DURATION");
                strcpy(duration->propDescr, "PT15M");
                insertBack(alarm->properties, duration);
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm DURATION disobeys inclusivity (back, front)
    strcpy(damage, "Alarm DURATION disobeys inclusivity (back, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                clearList(alarm->properties);
                Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
                strcpy(duration->propName, "DURATION");
                strcpy(duration->propDescr, "PT15M");
                insertBack(alarm->properties, duration);
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm REPEAT disobeys inclusivity (front, front)
    strcpy(damage, "Alarm REPEAT disobeys inclusivity (front, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                clearList(alarm->properties);
                Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);
                strcpy(repeat->propName, "REPEAT");
                strcpy(repeat->propDescr, "4");
                insertBack(alarm->properties, repeat);
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm REPEAT disobeys inclusivity (front, back)
    strcpy(damage, "Alarm REPEAT disobeys inclusivity (front, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                clearList(alarm->properties);
                Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);
                strcpy(repeat->propName, "REPEAT");
                strcpy(repeat->propDescr, "4");
                insertBack(alarm->properties, repeat);
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm REPEAT disobeys inclusivity (back, back)
    strcpy(damage, "Alarm REPEAT disobeys inclusivity (back, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                clearList(alarm->properties);
                Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);
                strcpy(repeat->propName, "REPEAT");
                strcpy(repeat->propDescr, "4");
                insertBack(alarm->properties, repeat);
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm REPEAT disobeys inclusivity (back, front)
    strcpy(damage, "Alarm REPEAT disobeys inclusivity (back, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                clearList(alarm->properties);
                Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);
                strcpy(repeat->propName, "REPEAT");
                strcpy(repeat->propDescr, "4");
                insertBack(alarm->properties, repeat);
                test(INV_ALARM);
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    for (int i = 0; contraband[i] != NULL; i++)
    {
        // Alarm has illegal property: (various) (front, front)
        sprintf(damage, "Alarm has illegal property: %s (front, front)", contraband[i]);
        load("TEST/OK/validation.ics");
        if (prime() == 1) 
        {
            Event *event = (Event*)getFromFront(calendar->events);
            if (event != NULL)
            {
                Alarm *alarm = (Alarm*)getFromFront(event->alarms);
                if (alarm != NULL)
                {
                    if (alarm->properties == NULL)
                    {
                        printf(YEL"FATAL: Could not extract alarm properties...createCalendar defective. Test aborted.\n"RESET);
                        d++;
                    }
                    else
                    {
                        Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);
                        strcpy(illegal->propName, "BEGIN");
                        strcpy(illegal->propDescr, contraband[i]);
                        insertBack(alarm->properties, illegal);
                        test(INV_ALARM);
                    }
                }
                else
                {
                    printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }

        // Alarm has illegal property: (various) (front, back)
        sprintf(damage, "Alarm has illegal property: %s (front, back)", contraband[i]);
        load("TEST/OK/validation.ics");
        if (prime() == 1) 
        {
            Event *event = (Event*)getFromFront(calendar->events);
            if (event != NULL)
            {
                Alarm *alarm = (Alarm*)getFromBack(event->alarms);
                if (alarm != NULL)
                {
                    if (alarm->properties == NULL)
                    {
                        printf(YEL"FATAL: Could not extract alarm properties...createCalendar defective. Test aborted.\n"RESET);
                        d++;
                    }
                    else
                    {
                        Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);
                        strcpy(illegal->propName, "BEGIN");
                        strcpy(illegal->propDescr, contraband[i]);
                        insertBack(alarm->properties, illegal);
                        test(INV_ALARM);
                    }
                }
                else
                {
                    printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }

        // Alarm has illegal property: (various) (back, back)
        sprintf(damage, "Alarm has illegal property: %s (back, back)", contraband[i]);
        load("TEST/OK/validation.ics");
        if (prime() == 1) 
        {
            Event *event = (Event*)getFromBack(calendar->events);
            if (event != NULL)
            {
                Alarm *alarm = (Alarm*)getFromBack(event->alarms);
                if (alarm != NULL)
                {
                    if (alarm->properties == NULL)
                    {
                        printf(YEL"FATAL: Could not extract alarm properties...createCalendar defective. Test aborted.\n"RESET);
                        d++;
                    }
                    else
                    {
                        Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);
                        strcpy(illegal->propName, "BEGIN");
                        strcpy(illegal->propDescr, contraband[i]);
                        insertBack(alarm->properties, illegal);
                        test(INV_ALARM);
                    }
                }
                else
                {
                    printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }

        // Alarm has illegal property: (various) (back, front)
        sprintf(damage, "Alarm has illegal property: %s (back, front)", contraband[i]);
        load("TEST/OK/validation.ics");
        if (prime() == 1) 
        {
            Event *event = (Event*)getFromBack(calendar->events);
            if (event != NULL)
            {
                Alarm *alarm = (Alarm*)getFromFront(event->alarms);
                if (alarm != NULL)
                {
                    if (alarm->properties == NULL)
                    {
                        printf(YEL"FATAL: Could not extract alarm properties...createCalendar defective. Test aborted.\n"RESET);
                        d++;
                    }
                    else
                    {
                        Property *illegal = calloc(1, sizeof(Property) + sizeof(char) * 20);
                        strcpy(illegal->propName, "BEGIN");
                        strcpy(illegal->propDescr, contraband[i]);
                        insertBack(alarm->properties, illegal);
                        test(INV_ALARM);
                    }
                }
                else
                {
                    printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
    }

    // Alarm has empty propDescr (front, front)
    strcpy(damage, "Alarm has empty propDescr (front, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *attach1 = calloc(1, sizeof(Property) + sizeof(char) * 100);

                    strcpy(attach1->propName, "");
                    strcpy(attach1->propDescr, "FMTTYPE=audio/basic:ftp://example.com/pub");

                    insertBack(alarm->properties, attach1);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has empty propDescr (front, back)
    strcpy(damage, "Alarm has empty propDescr (front, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *attach1 = calloc(1, sizeof(Property) + sizeof(char) * 100);

                    strcpy(attach1->propName, "");
                    strcpy(attach1->propDescr, "FMTTYPE=audio/basic:ftp://example.com/pub");

                    insertBack(alarm->properties, attach1);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has empty propDescr (back, back)
    strcpy(damage, "Alarm has empty propDescr (back, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *attach1 = calloc(1, sizeof(Property) + sizeof(char) * 100);

                    strcpy(attach1->propName, "");
                    strcpy(attach1->propDescr, "FMTTYPE=audio/basic:ftp://example.com/pub");

                    insertBack(alarm->properties, attach1);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm has empty propDescr (back, front)
    strcpy(damage, "Alarm has empty propDescr (back, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *attach1 = calloc(1, sizeof(Property) + sizeof(char) * 100);

                    strcpy(attach1->propName, "");
                    strcpy(attach1->propDescr, "FMTTYPE=audio/basic:ftp://example.com/pub");

                    insertBack(alarm->properties, attach1);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm propName exceeds array (front, front)
    strcpy(damage, "Alarm propName exceeds array (front, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *attach1 = calloc(1, sizeof(Property) + sizeof(char) * 100);
                    for (int i = 0; i < 200; i++)
                        (attach1->propName)[i] = 'A';
                    strcpy(attach1->propDescr, "FMTTYPE=audio/basic:ftp://example.com/pub");

                    insertBack(alarm->properties, attach1);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm propName exceeds array (front, back)
    strcpy(damage, "Alarm propName exceeds array (front, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *attach1 = calloc(1, sizeof(Property) + sizeof(char) * 100);
                    for (int i = 0; i < 200; i++)
                        (attach1->propName)[i] = 'A';
                    strcpy(attach1->propDescr, "FMTTYPE=audio/basic:ftp://example.com/pub");

                    insertBack(alarm->properties, attach1);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm propName exceeds array (back, back)
    strcpy(damage, "Alarm propName exceeds array (back, back)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromBack(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *attach1 = calloc(1, sizeof(Property) + sizeof(char) * 100);
                    for (int i = 0; i < 200; i++)
                        (attach1->propName)[i] = 'A';
                    strcpy(attach1->propDescr, "FMTTYPE=audio/basic:ftp://example.com/pub");

                    insertBack(alarm->properties, attach1);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // Alarm propName exceeds array (back, front)
    strcpy(damage, "Alarm propName exceeds array (back, front)");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromBack(calendar->events);
        if (event != NULL)
        {
            Alarm *alarm = (Alarm*)getFromFront(event->alarms);
            if (alarm != NULL)
            {
                if (alarm->properties == NULL)
                {
                    printf(YEL"FATAL: Could not extract alarm properties list...createCalendar defective. Test aborted.\n"RESET);
                    d++;
                }
                else
                {
                    clearList(alarm->properties);

                    Property *attach1 = calloc(1, sizeof(Property) + sizeof(char) * 100);
                    for (int i = 0; i < 200; i++)
                        (attach1->propName)[i] = 'A';
                    strcpy(attach1->propDescr, "FMTTYPE=audio/basic:ftp://example.com/pub");

                    insertBack(alarm->properties, attach1);
                    test(INV_ALARM);
                }
            }
            else
            {
                printf(YEL"FATAL: Could not extract alarm...createCalendar defective. Test aborted.\n"RESET);
                d++;
            }
        }
        else
        {
            printf(YEL"FATAL: Could not extract event...createCalendar defective. Test aborted.\n"RESET);
            d++;
        }
    }

    // cce cca cea
    /* Creating INV_ALM score */
    alm_n = n; n = 0;
    alm_d = d; d = 0;

    printf(CYA"----INV_CAL + INV_EVENT----\n"RESET);
 
    for (int i = 0; i < 13; i++)
    {
        for (int j = 0; j < 21; j++)
        {
            strcpy(damage, "");
            load("TEST/OK/validation.ics");
            if (prime() == 1) {
            switch(i)
            {
                case 0:
                    emptyProdid();
                    break;

                case 1:
                    prodidExceedsArrayLength();
                    break;

                case 2:
                    calendarHasNullEventList();
                    break;

                case 3:
                    calendarHasNullPropertyList();
                    break;

                case 4:
                    calendarHasEmptyEventList();
                    break;

                case 5:
                    calendarHasIllegalComponentTag();
                    break;

                case 6:
                    calendarHasDuplicateCalscale();
                    break;

                case 7:
                    calendarHasDuplicateMethod();
                    break;

                case 8:
                    calendarHasDuplicateProdid();
                    break;

                case 9:
                    calendarHasDuplicateVersion();
                    break;

                case 10:
                    calendarPropNameEmpty();
                    break;

                case 11:
                    calendarPropNameExceeds();
                    break;

                case 12:
                    calendarPropDescrBlank();
                    break;
            }
            strcat(damage, " + ");

            if (calendar->events == NULL || getLength(calendar->events) < 1) {
                strcat(damage, "<event test skipped>");
                test(INV_CAL);
                continue;}

            switch (j)
            {

                case 0:
                    eventEmptyUID();
                    break;

                case 1:
                    eventUIDExceeds();
                    break;

                case 2:
                    eventDuplicateUID();
                    break;

                case 3:
                    eventDuplicateDTSTART();
                    break;

                case 4:
                    eventDuplicateDTSTAMP();
                    break;

                case 5:
                    eventNullPropertyList();
                    break;

                case 6:
                    eventNullAlarmList();
                    break;

                case 7:
                    eventIllegalProperty();
                    break;

                case 8:
                    eventDuplicateProperty();
                    break;

                case 9:
                    eventViolatesExclusivity();
                    break;

                case 10:
                    eventPropNameBlank();
                    break;

                case 11:
                    eventPropNameExceeds();
                    break;

                case 12:
                    eventPropDescrBlank();
                    break;

                case 13:
                    eventDTSTARTEmptyDate();
                    break;

                case 14:
                    eventDTSTARTDateExceeds();
                    break;

                case 15:
                    eventDTSTARTEmptyTime();
                    break;

                case 16:
                    eventDTSTARTTimeExceeds();
                    break;

                case 17:
                    eventDTSTAMPEmptyDate();
                    break;

                case 18:
                    eventDTSTAMPDateExceeds();
                    break;

                case 19:
                    eventDTSTAMPEmptyTime();
                    break;

                case 20:
                    eventDTSTAMPTimeExceeds();
                    break;
            }
            test(INV_CAL);}
        }
    }

    cce_n = n; n = 0;
    cce_d = d; d = 0;

    printf(CYA"----INV_CAL + INV_ALARM----\n"RESET);

    for (int i = 0; i < 13; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            strcpy(damage, "");
            load("TEST/OK/validation.ics");
            if (prime() == 1)
            {
                switch(i)
                {
                    case 0:
                        emptyProdid();
                        break;

                    case 1:
                        prodidExceedsArrayLength();
                        break;

                    case 2:
                        calendarHasNullEventList();
                        break;

                    case 3:
                        calendarHasNullPropertyList();
                        break;

                    case 4:
                        calendarHasEmptyEventList();
                        break;

                    case 5:
                        calendarHasIllegalComponentTag();
                        break;

                    case 6:
                        calendarHasDuplicateCalscale();
                        break;

                    case 7:
                        calendarHasDuplicateMethod();
                        break;

                    case 8:
                        calendarHasDuplicateProdid();
                        break;

                    case 9:
                        calendarHasDuplicateVersion();
                        break;

                    case 10:
                        calendarPropNameEmpty();
                        break;

                    case 11:
                        calendarPropNameExceeds();
                        break;

                    case 12:
                        calendarPropDescrBlank();
                        break;
                }

                strcat(damage, " + ");

                if (calendar->events == NULL || getLength(calendar->events) < 1) {
                    strcat(damage, "<event test skipped>");
                    test(INV_CAL);
                    continue;}

                switch(j)
                {
                    case 0:
                        alarmEmptyAction();
                        break;

                    case 1:
                        alarmActionOverflow();
                        break;

                    case 2:
                        alarmInvalidAction();
                        break;

                    case 3:
                        alarmNullTrigger();
                        break;

                    case 4:
                        alarmEmptyTrigger();
                        break;

                    case 5:
                        alarmNullProperties();
                        break;

                    case 6:
                        alarmDuplicateAction();
                        break;

                    case 7:
                        alarmDuplicateTrigger();
                        break;

                    case 8:
                        alarmDuplicateDuration();
                        break;

                    case 9:
                        alarmDuplicateRepeat();
                        break;

                    case 10:
                        alarmDuplicateAttach();
                        break;

                    case 11:
                        alarmDurationDisobeysInclusivity();
                        break;

                    case 12:
                        alarmRepeatDisobeysInclusivity();
                        break;

                    case 13:
                        alarmIllegalProperty();
                        break;

                    case 14:
                        alarmEmptyPropdescr();
                        break;

                    case 15:
                        alarmPropnameOverflow();
                        break;

                }
                test(INV_CAL);
            }
        }
    }

    cca_n = n; n = 0;
    cca_d = d; d = 0;

    printf(CYA"---INV_EVENT + INV_ALARM---\n"RESET);
    for (int i = 0; i < 21; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            strcpy(damage, "");
            load("TEST/OK/validation.ics");
            if (prime() == 1)
            {
                switch(i)
                {
                case 0:
                    eventEmptyUID();
                    break;

                case 1:
                    eventUIDExceeds();
                    break;

                case 2:
                    eventDuplicateUID();
                    break;

                case 3:
                    eventDuplicateDTSTART();
                    break;

                case 4:
                    eventDuplicateDTSTAMP();
                    break;

                case 5:
                    eventNullPropertyList();
                    break;

                case 6:
                    eventNullAlarmList();
                    break;

                case 7:
                    eventIllegalProperty();
                    break;

                case 8:
                    eventDuplicateProperty();
                    break;

                case 9:
                    eventViolatesExclusivity();
                    break;

                case 10:
                    eventPropNameBlank();
                    break;

                case 11:
                    eventPropNameExceeds();
                    break;

                case 12:
                    eventPropDescrBlank();
                    break;

                case 13:
                    eventDTSTARTEmptyDate();
                    break;

                case 14:
                    eventDTSTARTDateExceeds();
                    break;

                case 15:
                    eventDTSTARTEmptyTime();
                    break;

                case 16:
                    eventDTSTARTTimeExceeds();
                    break;

                case 17:
                    eventDTSTAMPEmptyDate();
                    break;

                case 18:
                    eventDTSTAMPDateExceeds();
                    break;

                case 19:
                    eventDTSTAMPEmptyTime();
                    break;

                case 20:
                    eventDTSTAMPTimeExceeds();
                    break;
                }

                strcat(damage, " + ");

                Event *event = (Event*)getFromFront(calendar->events);
                if (event->alarms == NULL || getLength(event->alarms) < 1) {
                    strcat(damage, "<alarm test skipped>");
                    test(INV_EVENT);
                    continue;}

                switch(j)
                {
                    case 0:
                        alarmEmptyAction();
                        break;

                    case 1:
                        alarmActionOverflow();
                        break;

                    case 2:
                        alarmInvalidAction();
                        break;

                    case 3:
                        alarmNullTrigger();
                        break;

                    case 4:
                        alarmEmptyTrigger();
                        break;

                    case 5:
                        alarmNullProperties();
                        break;

                    case 6:
                        alarmDuplicateAction();
                        break;

                    case 7:
                        alarmDuplicateTrigger();
                        break;

                    case 8:
                        alarmDuplicateDuration();
                        break;

                    case 9:
                        alarmDuplicateRepeat();
                        break;

                    case 10:
                        alarmDuplicateAttach();
                        break;

                    case 11:
                        alarmDurationDisobeysInclusivity();
                        break;

                    case 12:
                        alarmRepeatDisobeysInclusivity();
                        break;

                    case 13:
                        alarmIllegalProperty();
                        break;

                    case 14:
                        alarmEmptyPropdescr();
                        break;

                    case 15:
                        alarmPropnameOverflow();
                        break;

                }
                test(INV_EVENT);
            }
        }
    }

    cea_n = n; n = 0;
    cea_d = d; d = 0;

    printf(CYA"---INV_CAL, _EVENT, _CAL---\n"RESET);
    
    for (int i = 0; i < 13; i++)
    {
        for (int j = 0; j < 21; j++)
        {
            for (int k = 0; k < 16; k++)
            {
                strcpy(damage, "");
                load("TEST/OK/validation.ics");
                if (prime() == 1)
                {
                    switch(i)
                    {
                    case 0:
                        emptyProdid();
                        break;

                    case 1:
                        prodidExceedsArrayLength();
                        break;

                    case 2:
                        calendarHasNullEventList();
                        break;

                    case 3:
                        calendarHasNullPropertyList();
                        break;

                    case 4:
                        calendarHasEmptyEventList();
                        break;

                    case 5:
                        calendarHasIllegalComponentTag();
                        break;

                    case 6:
                        calendarHasDuplicateCalscale();
                        break;

                    case 7:
                        calendarHasDuplicateMethod();
                        break;

                    case 8:
                        calendarHasDuplicateProdid();
                        break;

                    case 9:
                        calendarHasDuplicateVersion();
                        break;

                    case 10:
                        calendarPropNameEmpty();
                        break;

                    case 11:
                        calendarPropNameExceeds();
                        break;

                    case 12:
                        calendarPropDescrBlank();
                        break;
                    }

                strcat(damage, " + ");

                if (calendar->events == NULL || getLength(calendar->events) < 1) {
                    strcat(damage, "<event test skipped>");
                    test(INV_CAL);
                    continue;}

                    switch(j)
                    {
                case 0:
                    eventEmptyUID();
                    break;

                case 1:
                    eventUIDExceeds();
                    break;

                case 2:
                    eventDuplicateUID();
                    break;

                case 3:
                    eventDuplicateDTSTART();
                    break;

                case 4:
                    eventDuplicateDTSTAMP();
                    break;

                case 5:
                    eventNullPropertyList();
                    break;

                case 6:
                    eventNullAlarmList();
                    break;

                case 7:
                    eventIllegalProperty();
                    break;

                case 8:
                    eventDuplicateProperty();
                    break;

                case 9:
                    eventViolatesExclusivity();
                    break;

                case 10:
                    eventPropNameBlank();
                    break;

                case 11:
                    eventPropNameExceeds();
                    break;

                case 12:
                    eventPropDescrBlank();
                    break;

                case 13:
                    eventDTSTARTEmptyDate();
                    break;

                case 14:
                    eventDTSTARTDateExceeds();
                    break;

                case 15:
                    eventDTSTARTEmptyTime();
                    break;

                case 16:
                    eventDTSTARTTimeExceeds();
                    break;

                case 17:
                    eventDTSTAMPEmptyDate();
                    break;

                case 18:
                    eventDTSTAMPDateExceeds();
                    break;

                case 19:
                    eventDTSTAMPEmptyTime();
                    break;

                case 20:
                    eventDTSTAMPTimeExceeds();
                    break;
                    }

                    strcat(damage, " + ");

                    Event *event = (Event*)getFromFront(calendar->events);
                if (event->alarms == NULL || getLength(event->alarms) < 1) {
                    strcat(damage, "<alarm test skipped>");
                    test(INV_CAL);
                    continue;}

                    switch(k)
                    {
                    case 0:
                        alarmEmptyAction();
                        break;

                    case 1:
                        alarmActionOverflow();
                        break;

                    case 2:
                        alarmInvalidAction();
                        break;

                    case 3:
                        alarmNullTrigger();
                        break;

                    case 4:
                        alarmEmptyTrigger();
                        break;

                    case 5:
                        alarmNullProperties();
                        break;

                    case 6:
                        alarmDuplicateAction();
                        break;

                    case 7:
                        alarmDuplicateTrigger();
                        break;

                    case 8:
                        alarmDuplicateDuration();
                        break;

                    case 9:
                        alarmDuplicateRepeat();
                        break;

                    case 10:
                        alarmDuplicateAttach();
                        break;

                    case 11:
                        alarmDurationDisobeysInclusivity();
                        break;

                    case 12:
                        alarmRepeatDisobeysInclusivity();
                        break;

                    case 13:
                        alarmIllegalProperty();
                        break;

                    case 14:
                        alarmEmptyPropdescr();
                        break;

                    case 15:
                        alarmPropnameOverflow();
                        break;

                    }

                    test(INV_CAL);
                }
            }
        }
    }

    all_n = n; n = 0;
    all_d = d; d = 0;
    
    printf(CYA"-------------OK------------\n"RESET);

    /* CALENDAR SPECIFIC */

    // version changed
    strcpy(damage, "Calendar version change");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        // this is legal
        calendar->version = 1.0;
        test(OK);
    }

    // prodID changed
    strcpy(damage, "Calendar version change");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        // this is also legal
        strcpy(calendar->prodID, "something different");
        test(OK);
    }

    // added CALSCALE
    strcpy(damage, "Calendar new CALSCALE");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        clearList(calendar->properties);
        Property *calscale = calloc(1, sizeof(Property) + sizeof(char) * 20);
        strcpy(calscale->propName, "CALSCALE");
        strcpy(calscale->propDescr, "GREGORIAN");
        insertBack(calendar->properties, calscale);
        test(OK);
    }

    // added METHOD
    strcpy(damage, "Calendar new METHOD");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        clearList(calendar->properties);
        Property *method = calloc(1, sizeof(Property) + sizeof(char) * 20);
        strcpy(method->propName, "METHOD");
        strcpy(method->propDescr, "REQUEST");
        insertBack(calendar->properties, method);
        test(OK);
    }

    /* EVENT SPECIFIC */

    // changing UID
    strcpy(damage, "Event changed UID");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        strcpy(event->UID, "HELLO WORLD");
        test(OK);
    }

    // changing creationDateTime
    strcpy(damage, "Event changed DTSTART");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        strcpy((event->creationDateTime).date, "19981026");
        strcpy((event->creationDateTime).time, "100000");
        (event->creationDateTime).UTC == false ? (event->creationDateTime).UTC = true : false;
        test(OK);
    }

    // changing startDateTime
    strcpy(damage, "Event changed DTSTART");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        strcpy((event->startDateTime).date, "19981026");
        strcpy((event->startDateTime).time, "100000");
        (event->startDateTime).UTC == false ? (event->startDateTime).UTC = true : false;
        test(OK);
    }

    // clearing event property list
    strcpy(damage, "Cleared event property list");
    load("TEST/OK/validation.ics");
    if (prime() == 1)
    {
        Event *event = (Event*)getFromFront(calendar->events);
        clearList(event->properties);
        test(OK);
    }

    // clearing Event->Properties list, then adding one of these
    char *okOnlyOnce[] =
    {
        "CLASS", "CREATED", "DESCRIPTION", "GEO", "LAST-MODIFIED", "LOCATION",
        "ORGANIZER", "PRIORITY", "SEQUENCE", "STATUS", "SUMMARY", "TRANSP", 
        "URL", "RECURRENCE-ID", NULL
    };

    for (int i = 0; okOnlyOnce[i] != NULL; i++)
    {
        sprintf(damage, "Adding one restricted eventProp: %s", okOnlyOnce[i]);
        load("TEST/OK/validation.ics");
        Event *event = (Event*)getFromFront(calendar->events);
        if (prime() == 1)
        {
            clearList(event->properties);
            Property *newProp = calloc(1, sizeof(Property) + sizeof(char) * 20);
            strcpy(newProp->propName, okOnlyOnce[i]);
            strcpy(newProp->propDescr, "TESTEST");
            insertBack(event->properties, newProp);
            test(OK);
        }
    }

    // added DTEND alone
    strcpy(damage, "Event new DTEND");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        clearList(event->properties);
        Property *dtend = calloc(1, sizeof(Property) + sizeof(char) * 20);
        strcpy(dtend->propName, "DTEND");
        strcpy(dtend->propDescr, "19981026T100000Z");
        insertBack(event->properties, dtend);
        test(OK);
    }

    // added DURATION alone
    strcpy(damage, "Event new DURATION");
    load("TEST/OK/validation.ics");
    if (prime() == 1) 
    {
        Event *event = (Event*)getFromFront(calendar->events);
        clearList(event->properties);
        Property *dur = calloc(1, sizeof(Property) + sizeof(char) * 20);
        strcpy(dur->propName, "DURATION");
        strcpy(dur->propDescr, "PT15M");
        insertBack(event->properties, dur);
        test(OK);
    }

    // adding unlimited event properties
    char *okUnlimited[] = 
    {
        "ATTACH", "ATTENDEE", "CATEGORIES", "COMMENT", "CONTACT", "EXDATE", "REQUEST-STATUS",
        "RELATED-TO", "RESOURCES", "RDATE", "RRULE" , NULL
    };

    for (int i = 0; okUnlimited[i] != NULL; i++)
    {
        sprintf(damage, "Adding multiple unlimited event property: %s", okUnlimited[i]);
        load("TEST/OK/validation.ics");
        if (prime() == 1)
        {
            Event *event = (Event*)getFromFront(calendar->events);
            clearList(event->properties);

            Property *newProp = calloc(1, sizeof(Property) + sizeof(char) * 20);
            strcpy(newProp->propName, okUnlimited[i]);
            strcpy(newProp->propDescr, "TESTEST");

            Property *newProp2 = calloc(1, sizeof(Property) + sizeof(char) * 20);
            strcpy(newProp2->propName, okUnlimited[i]);
            strcpy(newProp2->propDescr, "TESTEST");

            insertBack(event->properties, newProp);
            insertBack(event->properties, newProp2);
            test(OK);
        }
    }

    // clearing event alarm list
    strcpy(damage, "Cleared event alarm list");
    load("TEST/OK/validation.ics");
    if (prime() == 1)
    {
        Event *event = (Event*)getFromFront(calendar->events);
        clearList(event->alarms);
        test(OK);
    }

    /* ALARM SPECIFIC */
     
    // changing alarm trigger
    strcpy(damage, "Alarm action trigger");
    load("TEST/OK/validation.ics");
    if (prime() == 1)
    {
        Event *event = (Event*)getFromFront(calendar->events);
        Alarm *alarm = (Alarm*)getFromFront(event->alarms);
        free(alarm->trigger);
        alarm->trigger = calloc(1, sizeof(char) * 20);
        strcpy(alarm->trigger, "-P15M");
        test(OK);
    }

    // clearing alarm properties, then adding DURATION and REPEAT
    strcpy(damage, "Adding DURATION and REPEAT");
    load("TEST/OK/validation.ics");
    if (prime() == 1)
    {
        Event *event = (Event*)getFromFront(calendar->events);
        Alarm *alarm = (Alarm*)getFromFront(event->alarms);
        clearList(alarm->properties);

        Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 20);
        Property *repeat = calloc(1, sizeof(Property) + sizeof(char) * 20);

        strcpy(duration->propName, "DURATION");
        strcpy(repeat->propName, "REPEAT");

        strcpy(duration->propDescr, "PT15M");
        strcpy(repeat->propDescr, "4");

        insertBack(alarm->properties, duration);
        insertBack(alarm->properties, repeat);
        test(OK);
    }

    // clearing alarm properties, then adding ATTACH
    strcpy(damage, "Adding ATTACH");
    load("TEST/OK/validation.ics");
    if (prime() == 1)
    {
        Event *event = (Event*)getFromFront(calendar->events);
        Alarm *alarm = (Alarm*)getFromFront(event->alarms);
        clearList(alarm->properties);

        Property *duration = calloc(1, sizeof(Property) + sizeof(char) * 200);

        strcpy(duration->propName, "ATTACH");

        strcpy(duration->propDescr, "FMTTYPE=audio/basic:ftp://something.basic");

        insertBack(alarm->properties, duration);
        test(OK);
    }

    aok_n = n; n = 0;
    aok_d = d; d = 0;
   
    if (errorCount > 0)
    {
        printf(RED"Repeating previous errors:\n"RESET); 
        for (int i = 0; i < errorCount; i++)
        {
            printf(RED"%s\n"RESET, errors[i]);
            free(errors[i]);
        }
    }

    printf("\033[1;32m");
    printf("------------------------------------------------------\n");
    printf("INV_CAL               SCORE: %d / %d (%.1f)%%\n", cal_n, cal_d, 100.0*(float)cal_n/(float)cal_d);
    printf("INV_EVT               SCORE: %d / %d (%.1f)%%\n", evt_n, evt_d, 100.0*(float)evt_n/(float)evt_d); 
    printf("INV_ALM               SCORE: %d / %d (%.1f)%%\n", alm_n, alm_d, 100.0*(float)alm_n/(float)alm_d);
    printf("------------------------------------------------------\n");
    printf("INV_CAL   x INV_EVENT SCORE: %d / %d (%.1f)%%\n", cce_n, cce_d, 100.0*(float)cce_n/(float)cce_d);
    printf("INV_CAL   x INT_ALARM SCORE: %d / %d (%.1f)%%\n", cca_n, cca_d, 100.0*(float)cca_n/(float)cca_d);
    printf("INV_EVENT x INV_ALARM SCORE: %d / %d (%.1f)%%\n", cea_n, cea_d, 100.0*(float)cea_n/(float)cea_d);
    printf("------------------------------------------------------\n");
    printf("INV CAL x EVT x ALM   SCORE: %d / %d (%.1f)%%\n", all_n, all_d, 100.0*(float)all_n/(float)all_d);
    printf("------------------------------------------------------\n");
    printf("OK                    SCORE: %d / %d (%.1f)%%\n", aok_n, aok_d, 100.0*(float)aok_n/(float)aok_d);
    printf("------------------------------------------------------\n");

    printf("Congratulations: your total score was %d / %d ", cal_n + evt_n + alm_n + cce_n + cca_n + cea_n + all_n + aok_n, cal_d + evt_d + alm_d + cce_d + cca_d + cea_d + all_d + aok_d);
    printf("(%.1f%%)\n"RESET, 100.0*(float)(cal_n + evt_n + alm_n + cce_n + cca_n + cea_n + all_n + aok_n)/(float)(cal_d + evt_d + alm_d + cce_d + cca_d + cea_d + all_d + aok_d));
    return errorCount > 0 ? 1 : 0;
}
