#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CalendarParser.h"
#include "CalendarHelper.h"
#include "LinkedListAPI.h"

int main(void)
{
    Event *test = calloc(1, sizeof(Event));
    strcpy(test->UID, "Event UID");

    DateTime DTSTART;
    strcpy(DTSTART.date,"19981230");
    strcpy(DTSTART.time,"111111");
    DTSTART.UTC = true;

    DateTime DTSTAMP;
    strcpy(DTSTAMP.date,"12341231");
    strcpy(DTSTART.time,"112211");
    DTSTART.UTC = false;

    test->creationDateTime = DTSTAMP;
    test->startDateTime = DTSTART;

    test->properties = initializeList(printProperty, deleteProperty, compareProperties);
    test->alarms = initializeList(printAlarm, deleteAlarm, compareAlarms);

    Property *organizer = calloc(1, sizeof(Property) + sizeof(char) * 1000);
    Property *location = calloc(1, sizeof(Property) + sizeof(char) * 1000);
    Property *summary = calloc(1, sizeof(Property) + sizeof(char) * 1000);

    strcpy(organizer->propName, "ORGANIZER");
    strcpy(organizer->propDescr, "ORGANIZED ORGANIZATION");

    strcpy(location->propName, "LOCATION");
    strcpy(location->propDescr, "LOCALIZED LOCALE");

    strcpy(summary->propName, "SUMMARY");
    strcpy(summary->propDescr, "SUMMARIZED SUMMARY");

    insertBack(test->properties, organizer);
    insertBack(test->properties, location);
    insertBack(test->properties, summary);

    char *json = eventToJSON(test);
    printf("%s\n", json);
    free(json);
    deleteEvent(test);

    return 0;

}
