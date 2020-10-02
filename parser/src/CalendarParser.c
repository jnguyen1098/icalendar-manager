#include "CalendarParser.h"
#include "CalendarHelper.h"
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 10000
#define MAX_LINE 100000
#define MAX_LINE_COUNT 10000
#define MAX_CALENDAR 4
#define MAX_EVENT 5
#define MAX_ALARM 3

enum level {CALENDAR = 0, EVENT = 1, ALARM = 2};

/* Isolates JSON value 
 * Returns either a dynamically allocated string 
 * containing the value of the key or returns
 * NULL if anything fails. */
char *findValue(const char *str, const char *key)
{
    /* NULL checks */
    if (str == NULL || key == NULL)
        return NULL;

    /* Variables */
    size_t len      = 0;    // length of str
    char *value     = NULL; // our result
    char *string    = NULL; // copy of str
    char *needle    = NULL; // formatted needle from key
    char *needleStr = NULL; // the result of strstr + needle

    /* Create length of string */
    len = strlen(str);

    /* Allocate copy of str */
    string = calloc(len + 1, sizeof(char));

    /* Check that calloc() succeeded */
    if (string == NULL) return NULL;

    /* Copy str */
    strcpy(string, str);

    /* Create our needle */
    needle = calloc(strlen(key) + 3 + 1, sizeof(char));

    /* Check that calloc() succeeded */
    if (needle == NULL) { free(string); return NULL; }

    /* Create our needle without colon */
    if (sprintf(needle, "\"%s\"", key) < 0)
    {free(string); free(needle); return NULL;}

    /* Find the instance of the key */
    if ((needleStr = strstr(string, needle)) == NULL)
    {free(string); free(needle); return NULL;}

    /* Naughty pointer stuff */
    needleStr = &(needleStr[strlen(needle)]);

    /* Eliminate any trailing whitespace before the colon */
    while ((*needleStr) == ' ' || (*needleStr) == '\t') 
        needleStr++;

    /* Advance the separator colon */
    if ((*needleStr) == ':')
        needleStr++;
    else
    {free(string); free(needle); return NULL;}

    /* Eliminate any trailing whitespace after the colon */
    while ((*needleStr) == ' ' || (*needleStr) == '\t')
        needleStr++;

    /* Check for blank value */
    if ((*needleStr) == ',' || (*needleStr) == '}')
    {free(string); free(needle); return NULL;}

    /* Check for null, true, false */
    // "null"
    char nullTest[5] = {0, 0, 0, 0, 0};
    strncpy(nullTest, needleStr, 4);
    if (strcmp(nullTest, "null") == 0)
    {   // bingo
        value = calloc(5, sizeof(char));
        strcpy(value, "null");
        free(string);
        free(needle);
        return value;
    }

    // "true"
    char trueTest[5] = {0, 0, 0, 0, 0};
    strncpy(trueTest, needleStr, 4);
    if (strcmp(trueTest, "true") == 0)
    {   // bingo
        value = calloc(5, sizeof(char));
        strcpy(value, "true");
        free(string);
        free(needle);
        return value;
    }
    
    // "false"
    char falseTest[6] = {0, 0, 0, 0, 0, 0};
    strncpy(falseTest, needleStr, 5);
    if (strcmp(falseTest, "false") == 0)
    {   // bingo
        value = calloc(6, sizeof(char));
        strcpy(value, "false");
        free(string);
        free(needle);
        return value;
    }

    /* Check for array */
    if (needleStr[0] == '[')
    {
        /* nested bracket balance
         * looping ends with = 0 */
        int balance = 1;
        size_t arrLen = 1;

        /* Finding out when array ends */
        for (; balance != 0; arrLen++)
        {
            if (needleStr[arrLen] == '[')
                balance++;
            else if (needleStr[arrLen] == ']')
                balance--;
        }

        /* Allocating the result */
        value = calloc(arrLen + 1, sizeof(char));

        // TODO consider whitespace removal
        /* Copying over what's necessary */
        for (int i = 0; i < arrLen; i++)
            value[i] = needleStr[i];
    }

    /* Check for object (TERRIBLE COUPLING) */
    else if (needleStr[0] == '{')
    {
        /* nested bracket balance
         * looping ends with = 0 */
        int balance = 1;
        size_t arrLen = 1;

        /* Finding out when array ends */
        for (; balance != 0; arrLen++)
        {
            if (needleStr[arrLen] == '{' && needleStr[arrLen - 1] != '\\')
                balance++;
            else if (needleStr[arrLen] == '}' && needleStr[arrLen - 1] != '\\')
                balance--;
        }

        /* Allocating the result */
        value = calloc(arrLen + 1, sizeof(char));

        // TODO consider whitespace removal
        /* Copying over what's necessary */
        for (int i = 0; i < arrLen; i++)
            value[i] = needleStr[i];
    }


    /* Check for string */
    else if (needleStr[0] == '"')
    {
        /* nested quote balance
         * looping ends with = 0 */
        int balance = 1;
        size_t arrLen = 1;

        /* Finding out when array ends */
        for (; balance != 0; arrLen++)
        {
            if (needleStr[arrLen] == '"' &&
                needleStr[arrLen - 1] != '\\')
                break;
        }

        /* Allocating the result */
        value = calloc(arrLen + 1, sizeof(char));

        // TODO consider whitespace removal
        /* Copying over what's necessary */
        for (int i = 0; i < arrLen - 1; i++)
            value[i] = needleStr[i + 1];
    }

    /* Else we assume it's a number */
    else
    {
        size_t arrLen = 1;
        for (; needleStr[arrLen] != ','; arrLen++);
        value = calloc(arrLen + 1, sizeof(char));
        // TODO consider whitespace removal
        for (int i = 0; i < arrLen; i++)
           value[i] = needleStr[i]; 
    }
    
    /* Housekeeping */
    free(string);
    free(needle);

    /* Finally, return value */
    return value;
}

/* Converts DateTime object into JSON string
 * without changing the original object. */
char *dtToJSON(DateTime prop)
{
    char *toReturn = NULL;
    /* Size of our skeleton */
    size_t len = strlen("{\"date\":\"\",\"time\":\"\",\"isUTC\":}");
    /* + date val size */
    len += strlen(prop.date);
    /* + time val size */
    len += strlen(prop.time);
    /* +  UTC val size (assumed 5) */
    len += 5;
    /* Allocate */
    toReturn = calloc(1, sizeof(char) * (len) + 1);
    if (toReturn == NULL) return NULL;
    sprintf(toReturn, "{\"date\":\"%s\",\"time\":\"%s\",\"isUTC\":%s}",
            prop.date, prop.time, prop.UTC == true ? "true" : "false");
    return toReturn;
}

/* Converts a property into a JSON string
 **/
char *propToJSON(const Property *property)
{
    char *toReturn = calloc(10000, sizeof(char));

    if (property == NULL)
	{
		strcpy(toReturn, "{}");
		return toReturn;
	}

    sprintf(toReturn, "{\"%s\":\"%s\"}", property->propName, property->propDescr);

    return toReturn;
}



/* Converts a List * of properties into
 * a JSON string*/
char *propListToJSON(List *propList)
{
    char *toReturn = calloc(5000, sizeof(char));
    if (propList == NULL || getLength(propList) == 0) {
        strcpy(toReturn, "[]");
	    return toReturn;
    }

    int numProp = getLength(propList);

    strcpy(toReturn, "[");

    ListIterator propWrite = createIterator(propList);

    Property *tmpTmpProp = (Property*)(nextElement(&propWrite));
    char *tmpPropJSON = propToJSON(tmpTmpProp);
    strcat(toReturn, tmpPropJSON);
    free(tmpPropJSON);

    if (numProp == 1) {
        strcat(toReturn, "]");
	    return toReturn;
    }

    for (int i = 0; i < numProp - 1; i++) {
        Property *tmpProp = (Property *)(nextElement(&propWrite));
        char *propJSON = propToJSON(tmpProp);
        strcat(toReturn, ",");
        strcat(toReturn, propJSON);
        free(propJSON);
    }
    
    /* Adding the end bit */
    strcat(toReturn, "]");

    return toReturn;
}

/* Converts Alarm object into JSON string
 * without changing the original alarm.
 *
 * Format: 
 * {"action":"action goes here","trigger":"trigger goes here","numProps":100}*/
char *alarmToJSON(const Alarm *alarm)
{
    char *toReturn = NULL;

    /* NULL check */
    if (alarm == NULL)
    {   // we will just return "{}"
        toReturn = calloc(3, sizeof(char));
        if (toReturn == NULL) return NULL;
        strcpy(toReturn, "{}");
        return toReturn;
    }

    /* Size of our skeleton */
    size_t len = 5000;

    /* Bull variables */
    char writeAction[10000];
    char writeTrigger[10000];

    /* Write such things */
    strcpy(writeAction, alarm->action);
    strcpy(writeTrigger, alarm->trigger);

    /* Creating propVal and almVal */
    int propVal = 2;

    if (alarm->properties != NULL)
        propVal += getLength(alarm->properties);

    /* Finally, allocate */
    toReturn = calloc(1, sizeof(char) * len + 1);
    if (toReturn == NULL) return NULL;

    /* I shouldn't do this in one step */
    sprintf(toReturn, "{\"action\":\"%s\",\"trigger\":\"%s\",\"numProps\":%d}", writeAction, writeTrigger, propVal);

    /* Finally we send the payload out */
    return toReturn;
}

/* Converts list of Alarm into JSON string
 * without changing anything */
char *alarmListToJSON(const List *alarmList)
{
    char *toReturn = NULL;
    if (alarmList == NULL || getLength((List*)alarmList) == 0)
    {
        toReturn = calloc(3, sizeof(char));
        if (toReturn == NULL) return NULL;
        strcpy(toReturn, "[]");
        return toReturn;
    }

    /* Length */
    int numAlarms = getLength((List*)alarmList);

    /* Temporary allocate because I don't trust realloc */
    // allocating for [, ], and nullterm
    toReturn = calloc(3, sizeof(char));
    if (toReturn == NULL) return NULL;
    size_t len = 3;
    strcpy(toReturn, "[");

    /* Iterating through the alarm list for bookkeeping */
    ListIterator alarmIter = createIterator((List*)alarmList);
    void *alarm = NULL;
    while ((alarm = nextElement(&alarmIter)) != NULL)
    {
        /* Temp cast */
        Alarm *tmpAlarm = (Alarm*)alarm;
        /* Creating our JSON string */
        char *alarmJSON = alarmToJSON(tmpAlarm);
        
        /* Adding to our length */
        len += strlen(alarmJSON) + 1;
        /* realloc'ing */
        toReturn = realloc(toReturn, sizeof(char) * len + 1);

        /* freeing our temp string */
        free(alarmJSON);
    }

    /* Creating the shared iterator */
    ListIterator alarmWrite = createIterator((List*)alarmList);

    /* Taking the first event */
    Alarm *tmpTmpAlarm = (Alarm*)(nextElement(&alarmWrite));
    /* Taking the JSON string */
    char *tmpAlarmJSON = alarmToJSON(tmpTmpAlarm);
    /* Concatenating the JSON string */
    strcat(toReturn, tmpAlarmJSON);
    /* Freeing the resultant string */
    free(tmpAlarmJSON);

    /* We don't need to deal with commas if there's
     * only one event in the entire list . . . */
    if (numAlarms == 1)
    {
        strcat(toReturn, "]");
        return toReturn;
    }

    /* Iterating through the event list for more events */
    for (int i = 0; i < numAlarms - 1; i++)
    {
        Alarm *tmpAlarm = (Alarm*)(nextElement(&alarmWrite));
        char *alarmJSON = alarmToJSON(tmpAlarm);
        strcat(toReturn, ",");
        strcat(toReturn, alarmJSON);
        free(alarmJSON);
    } 
    
    /* Adding the end bit */
    strcat(toReturn, "]");

    return toReturn;
}

/* Converts Event object into JSON string 
 * without changing the original object. */
char *eventToJSON(const Event *event)
{
    char *toReturn = NULL;
    /* NULL check */
    if (event == NULL)
    {   // we will just return "{}"
        toReturn = calloc(3, sizeof(char));
        if (toReturn == NULL) return NULL;
        strcpy(toReturn, "{}");
        return toReturn;
    }

    /* Creating our JSON DTstart string */
    char *DTval = dtToJSON(event->startDateTime);

    /* Size of our skeleton */
    size_t len = strlen("{\"startDT\":\"\",\"numProps\":,\"numAlarms\":,\"summary\":\"\",\"location\":\"\",\"organizer\":\"\"}");
    /* + DT size */
    len += strlen(DTval);
    /* + propVal size */
    len += 5; // I swear if there are 100000 properties lol
    /* + almVal size */
    len += 5; // . . .

    /* + Summary */
    size_t sumValSize = 0;
    char sumVal[2500];
    strcpy(sumVal, "");
    ListIterator propIter = createIterator(event->properties);
    void *prop = NULL;
    while ((prop = nextElement(&propIter)) != NULL)
    { // iterate until we find it. otherwise it stays at 0
        Property *tmpProp = (Property*)prop;
        if (strcmp(tmpProp->propName, "SUMMARY") == 0) {
            strcpy(sumVal, tmpProp->propDescr);
            sumValSize = strlen(tmpProp->propDescr) + 1;
        }
    }
    len += sumValSize; // this will either be a +value or +0

    // organizer
    size_t organizerSize = 0;
    char organizerVal[2500];
    strcpy(organizerVal, "");
    ListIterator organizerIter = createIterator(event->properties);
    prop = NULL;
    while ((prop = nextElement(&organizerIter)) != NULL)
    { // iterate until we find it, or it stays at 0
	Property *tmpProp = (Property*)prop;
	if (strcmp(tmpProp->propName, "ORGANIZER") == 0) {
	    strcpy(organizerVal, tmpProp->propDescr);
	    organizerSize = strlen(tmpProp->propDescr) + 1;
	}
    }
    len += organizerSize;

    // location
    size_t locationSize = 0;
    char locationVal[2500];
    strcpy(locationVal, "");
    ListIterator locationIter = createIterator(event->properties);
    prop = NULL;
    while ((prop = nextElement(&locationIter)) != NULL)
    {
        Property *tmpProp = (Property*)prop;
        if (strcmp(tmpProp->propName, "LOCATION") == 0) {
            strcpy(locationVal, tmpProp->propDescr);
            locationSize = strlen(tmpProp->propDescr) + 1;
        }
    }
    len += locationSize;

    /* Creating propVal and almVal */
    int propVal = 3, almVal = 0;

    if (event->properties != NULL)
        propVal = getLength(event->properties) + 3;

    if (event->alarms != NULL)
        almVal = getLength(event->alarms);

    /* Finally, allocate */
    toReturn = calloc(1, sizeof(char) * (len + 200));
    if (toReturn == NULL) return NULL;

    /* I shouldn't do this in one step */
    sprintf(toReturn, "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"%s\",\"location\":\"%s\",\"organizer\":\"%s\"}", DTval, propVal, almVal, sumVal, locationVal, organizerVal);

    /* Free anything temporary we have used */
    free(DTval);
    /* Finally we send the payload out */
    return toReturn;
}

/* Converts Event list into JSON string
 * without changing the original object */
char *eventListToJSON(const List *eventList)
{
    char *toReturn = NULL;
    /* NULL or blank event lists */
    if (eventList == NULL || getLength((List*)eventList) == 0)
    {
        toReturn = calloc(3, sizeof(char));
        if (toReturn == NULL) return NULL;
        strcpy(toReturn, "[]");
        return toReturn;
    }

    /* Length */
    int numEvents = getLength((List*)eventList);

    /* Temporary allocate because I don't trust realloc */
    // allocating for [, ], and nullterm
    toReturn = calloc(3, sizeof(char));
    if (toReturn == NULL) return NULL;
    size_t len = 3;
    strcpy(toReturn, "[");

    /* Iterating through the event list for bookkeeping */
    ListIterator eventIter = createIterator((List*)eventList);
    void *event = NULL;
    while ((event = nextElement(&eventIter)) != NULL)
    {
        /* Temp cast */
        Event *tmpEvent = (Event*)event;
        /* Creating our JSON string */
        char *eventJSON = eventToJSON(tmpEvent);
        
        /* Adding to our length */
        len += strlen(eventJSON) + 1;
        /* realloc'ing */
        toReturn = realloc(toReturn, sizeof(char) * len + 1);

        /* freeing our temp string */
        free(eventJSON);
    }

    /* Creating the shared iterator */
    ListIterator eventWrite = createIterator((List*)eventList);

    /* Taking the first event */
    Event *tmpTmpEvent = (Event*)(nextElement(&eventWrite));
    /* Taking the JSON string */
    char *tmpEventJSON = eventToJSON(tmpTmpEvent);
    /* Concatenating the JSON string */
    strcat(toReturn, tmpEventJSON);
    /* Freeing the resultant string */
    free(tmpEventJSON);

    /* We don't need to deal with commas if there's
     * only one event in the entire list . . . */
    if (numEvents == 1)
    {
        strcat(toReturn, "]");
        return toReturn;
    }

    /* Iterating through the event list for more events */
    for (int i = 0; i < numEvents - 1; i++)
    {
        Event *tmpEvent = (Event*)(nextElement(&eventWrite));
        char *eventJSON = eventToJSON(tmpEvent);
        strcat(toReturn, ",");
        strcat(toReturn, eventJSON);
        free(eventJSON);
    } 
    
    /* Adding the end bit */
    strcat(toReturn, "]");

    return toReturn;
}

/* Converts Calendar into JSON string
 * without changing the original Calendar */
char *calendarToJSON(const Calendar *cal)
{
    char *toReturn = NULL;
    if (cal == NULL)
    {
        toReturn = calloc(3, sizeof(char));
        if (toReturn == NULL) return NULL;
        strcpy(toReturn, "{}");
        return toReturn;
    }

    /* Size of our skeleton */
    size_t len = strlen("{\"version\":,\"prodID\":\"\",\"numProps\":,\"numEvents\":}");
    
    /* Version */
    float verVal = cal->version;
    len += 10; // can't trust anyone

    /* Product ID */
    char newProdid[1000];
    strcpy(newProdid, cal->prodID);
    len += strlen(newProdid);

    /* Number of Properties */
    int numProp = 2;
    numProp += getLength(cal->properties);
    len += 10; // lmao

    /* Number of Events */
    int numEvents = getLength(cal->events);
    if (numEvents < 1) numEvents = 1;
    len += 10; // I hate myself
    
    /* Let's allocate */
    toReturn = calloc(1, sizeof(char) * len + 1);

    /* Yikes */
    sprintf(toReturn, "{\"version\":%d,\"prodID\":\"%s\",\"numProps\":%d,\"numEvents\":%d}", (int)verVal, newProdid, numProp, numEvents);

    return toReturn;
}

/* Converts a JSON string into a Calendar struct */
Calendar *JSONtoCalendar(const char *str)
{
    /* Always NULL check */
    if (str == NULL)
        return NULL;
    
    /* Extract the version */
    char *version = findValue(str, "version");
    if (version == NULL) return NULL;
    float finalVer = atof(version);

    /* Extract the PRODID */
    char *prodid = findValue(str, "prodID");
    if (prodid == NULL) return NULL;
    char *finalProdid = prodid;

    /* Create our calendar */
    Calendar *calendar = calloc(1, sizeof(Calendar));
    if (calendar == NULL) return NULL;

    /* Write version */
    calendar->version = finalVer;
    
    /* Write prodID */
    strcpy(calendar->prodID, finalProdid);

    /* Initialize events list */
    calendar->events = initializeList(printEvent, deleteEvent, compareEvents);

    /* Initialize properties list */
    calendar->properties = initializeList(printProperty, deleteProperty, compareProperties);

    /* Free */
    free(version);
    free(prodid);
    return calendar;
}

/* Wedlock function: converts JSON to dateTime struct */
DateTime *JSONtoDT(const char *str)
{
    DateTime *payload = calloc(1, sizeof(DateTime));

    char *date = findValue(str, "date");
    char *time = findValue(str, "time");
    char *isUTC = findValue(str, "isUTC");

    strcpy(payload->date, date);
    strcpy(payload->time, time);
    
    if (strcmp(isUTC, "true") == 0)
        payload->UTC = true;
    else if (strcmp(isUTC, "false") == 0)
	payload->UTC = false;
    else
	printf("Could not parse isUTC value.\n");

    free(date);
    free(time);
    free(isUTC);
    return payload;
}

/* Converts a JSON string into an Event struct */
Event *JSONtoEvent(const char *str)
{
    printf("JSONtoEvent string: \"%s\"", str);
    /* NULL check */
    if (str == NULL)
        return NULL;
    
    /* Extract the UID */
    char *UID = findValue(str, "UID");
    if (UID == NULL) return NULL;

    /* Extract the DTSTART */
    char *DTSTART = findValue(str, "DTSTART");

    /* Extract the DTSTAMP */
    char *DTSTAMP = findValue(str, "DTSTAMP");

    /* Extract the summary */
    char *summary = findValue(str, "summary");

    /* Create our event */
    Event *event = calloc(1, sizeof(Event));

    /* Write UID */
    strcpy(event->UID, UID);
    
    /* Write DTSTART */
    DateTime *DTSTARTstruct = JSONtoDT(DTSTART);
    strcpy(event->startDateTime.date, DTSTARTstruct->date);
    strcpy(event->startDateTime.time, DTSTARTstruct->time);
    event->startDateTime.UTC = DTSTARTstruct->UTC;
    
    /* Write DTSTAMP */
    DateTime *DTSTAMPstruct = JSONtoDT(DTSTAMP);
    strcpy(event->creationDateTime.date, DTSTAMPstruct->date);
    strcpy(event->creationDateTime.time, DTSTAMPstruct->time);
    event->creationDateTime.UTC = DTSTAMPstruct->UTC;

    /* Initialize properties list */
    event->properties = initializeList(printProperty, deleteProperty, compareProperties);

    /* Initialize alarms list */
    event->alarms = initializeList(printAlarm, deleteAlarm, compareAlarms);

    /* Write summary */
    if (strcmp(summary, "") != 0) {
    	Property * sumProp = calloc(1, sizeof(Property) + sizeof(char) * 1000 + 1);
    	strcpy(sumProp->propName, "SUMMARY");
    	strcpy(sumProp->propDescr, summary);
    	insertBack(event->properties, sumProp);
    }

    free(UID);
    free(DTSTART);
    free(DTSTAMP);
    free(summary);
    free(DTSTARTstruct);
    free(DTSTAMPstruct);
    return event;
}

/* Adds an Event struct to an existing Calendar struct */
void addEvent (Calendar *cal, Event *toBeAdded)
{
    if (cal == NULL || toBeAdded == NULL)
        return;
   
    /* insane mechanics */ 
    if (cal->events != NULL)
        insertBack(cal->events, toBeAdded);

    return;
}

char *filenameToList(char *fileName, int eventNum, int discrim)
{
    /* Seriously, this piece of code is pathetic. Why am I
     * doing this. .  . anyways...
     *
     * discrim = 1 = ALARM LIST
     * discrim = 2 = PROP  LIST
     *
     * damn this is pathetic */

    printf("filenameToPropList called\n");
    printf("Attempting to open %s\n", fileName);

    Calendar *newCalendar = NULL;
    
    ICalErrorCode ccValidate = createCalendar(fileName, &newCalendar);

    if (ccValidate != OK) {
        printf("createCalendar failed on %s\n", fileName);
	return NULL;
    }

    printf("Attempting to validate %s\n", fileName);
    ICalErrorCode vcValidate = validateCalendar(newCalendar);

    if (vcValidate != OK) {
        printf("validateCalendar failed on %s\n", fileName);
	if (newCalendar) deleteCalendar(newCalendar);
	return NULL;
    }

    char *toReturn = NULL;

    printf("Iterating through events of %s\n", fileName);
    ListIterator eventIter = createIterator(newCalendar->events);
    void *event = NULL;
    int i = 0;
    while ((event = nextElement(&eventIter)) != NULL) {
	if (i == eventNum) {
	    Event *tmpEvent = (Event*)event;
	    if (discrim == 2)
	    {
                char *tmpStr = propListToJSON(tmpEvent->properties);
	        toReturn = calloc(strlen(tmpStr) + 1, sizeof(char));
	        strcpy(toReturn, tmpStr);
	        free(tmpStr);
	    }
	    else if (discrim == 1)
	    {
                char *tmpStr = alarmListToJSON(tmpEvent->alarms);
		toReturn = calloc(strlen(tmpStr) + 1, sizeof(char));
		strcpy(toReturn, tmpStr);
		free(tmpStr);
	    }
	    else
	    {
                printf("You have specified neither alarms nor properties...\n");
	    }
	    if (newCalendar) deleteCalendar(newCalendar);
	    return toReturn;
	}
	i++;
    }

    if (newCalendar) deleteCalendar(newCalendar);
    printf("Returning final JSON string.\n");
    printf("Your string (from C) is %s\n", toReturn);
    return toReturn;
}

int serverCreateCalendar(char *calJSON, char *evtJSON, char *calPath, int flag)
{
    /* flag = 1 => create calendar
     * flag = 2 => add event to existing */

    /* the versatility of this is that now the
     * third parameter will either be polymorphism'd
     * into either a write path or a path to an
     * existing calendar that needs to be edited.*/

    printf("serverCreateCalendar called.\n");

    Calendar *newCalendar = NULL;

    if (flag == 1) {
        printf("Calling JSONtoCalendar.\n");
        newCalendar = JSONtoCalendar(calJSON);
    }

    else if (flag == 2) {
        printf("Calling createCalendar.\n");
	newCalendar = NULL;
	createCalendar(calPath, &newCalendar);
    }

    if (newCalendar == NULL)
	return 0;

    printf("Calling JSONtoEvent.\n");
    Event *newEvent = JSONtoEvent(evtJSON);
    
    if (newEvent == NULL)
	return 0;

    printf("Calling addEvent.\n");
    addEvent(newCalendar, newEvent);

    printf("Calling validateCalendar.\n");
    ICalErrorCode validateCheck = validateCalendar(newCalendar);

    if (validateCheck != OK) {
	char *errorTime = printError(validateCheck);
	printf("validateCalendar failed: %s\n", errorTime);
	free(errorTime);
    }
    
    printf("Calling writeCalendar.\n");
    ICalErrorCode writeCheck = writeCalendar(calPath, newCalendar);

    if (writeCheck != OK)
	return 0;

    printf("writePath is %s\n", calPath);
    return 1;
}

/* Validate Calendar */
ICalErrorCode validateCalendar(const Calendar* obj)
{
    /* First check. If obj is NULL */
    if (obj == NULL)
        return INV_CAL;

/* ------- *
 * INV_CAL 
 * ------- */

    /* Checking prodID for empty string */
    if ((obj->prodID == NULL) || (obj->prodID)[0] == '\0')
        return INV_CAL;

    /* Checking prodID if it exceeds array length */
    int ended = 0; // flag to see if we have reached nullterm yet
    for (int i = 0; i < 1000; i++)
        if ((obj->prodID)[i] == '\0')
        {
            ended = 1;
            break;
        }
    if (ended == 0)
        return INV_CAL;

    /* Checking if List *events is NULL */
    if ((obj->events) == NULL)
        return INV_CAL;

    /* Checking if List *events is empty */
    if (getLength(obj->events) < 1)
        return INV_CAL;

    /* Checking if List *properties is NULL */
    if ((obj->properties) == NULL)
        return INV_CAL;

    /* Checking for illegal/black cal properties/components (whitelist) */
    char *allowedProperties[] = {"CALSCALE", "METHOD", "PRODID", "VERSION", NULL};
    ListIterator calProps = createIterator(obj->properties); // iterator
    void *property = NULL; // temp prop
    while ((property = nextElement(&calProps)) != NULL) // for each element
    {
        Property *tmpProperty = (Property*)property;
        /* Checking propName for exceed */
        int hasNullTerm = 0;
        for (int k = 0; k < 200; k++)
            if ((tmpProperty->propName)[k] == '\0')
            {
                hasNullTerm = 1;
                break;
            }
        if (hasNullTerm == 0)
            return INV_CAL;

        /* Checking for illegal cal props/components (whitelist) */
        int i = 0;
        for (; allowedProperties[i] != NULL; i++)
            if (strcmp(tmpProperty->propName, allowedProperties[i]) == 0)
                break;
        if (allowedProperties[i] == NULL)
            return INV_CAL;

    }

    /* Checking for independent occurrences 
     * 0 - CALSCALE, 1 - METHOD, 2 - PRODID, 3 - VERSION*/
    int record[4] = {0, 0, 0, 0};
    ListIterator occurrences = createIterator(obj->properties);
    void *occurrence = NULL; // temp prop
    while ((occurrence = nextElement(&occurrences)) != NULL)
    {
        Property *tmpProperty = (Property*)occurrence;
        for (int i = 0; i < 4; i++)
        {
            if (strcmp(tmpProperty->propName, allowedProperties[i]) == 0)
            {
                (record[i])++;
                if (record[i] > 1)
                    return INV_CAL;
            }
        }
    }

    /* Checking for internal occurrences (i.e. VERSION and
     * PRODID -- they are not in the prop list. They are
     * internal and as such it is easier to sneak a duplicate
     * of either of the two. Not today, though)*/
    ListIterator internals = createIterator(obj->properties);
    void *internal = NULL; // temp prop
    while ((internal = nextElement(&internals)) != NULL)
    {
        Property *tmpProperty = (Property*)internal;
        if (strcmp(tmpProperty->propName, "VERSION") == 0 || 
            strcmp(tmpProperty->propName, "PRODID") == 0)
            return INV_CAL;
    } 

    /* Checking for properties with blank descriptions */
    ListIterator descriptions = createIterator(obj->properties);
    void *description = NULL;
    while ((description = nextElement(&descriptions)) != NULL)
    {
        Property *tmpProperty = (Property*)description;
        if (strcmp(tmpProperty->propDescr, "") == 0)
            return INV_CAL;
    }


/* --------- *
 * INV_EVENT 
 * --------- */

    char *optionalRestrictedProps[] = 
    {
        /* Optional, exactly one allowed */
        "CLASS", "CREATED", "DESCRIPTION", "GEO", "LAST-MODIFIED", "LOCATION", 
        "ORGANIZER", "PRIORITY", "SEQUENCE", "STATUS", "SUMMARY", "TRANSP", 
        "URL", "RECURRENCE-ID", NULL
    };

    /* 27 properties */
    char *whitelistedProperties[] = 
        {"CLASS", "CREATED", "DESCRIPTION", "GEO", "LAST-MODIFIED", "LOCATION", 
        "ORGANIZER", "PRIORITY", "SEQUENCE", "STATUS", "SUMMARY", "TRANSP", 
        "URL", "RECURRENCE-ID", "RRULE", "DTEND", "DURATION", "ATTACH", "ATTENDEE", 
        "CATEGORIES", "COMMENT", "CONTACT", "EXDATE", "REQUEST-STATUS", "RELATED-TO", 
        "RESOURCES", "RDATE", NULL};

    /* Creating our iterator */
    ListIterator events = createIterator(obj->events);
    void *event = NULL;
    /* For each event... */
    while ((event = nextElement(&events)) != NULL)
    {
        /* Cast */
        Event *tmpEvent = (Event*)event;

        /* Event user ID (UID) must not be empty */
        if (strcmp(tmpEvent->UID, "") == 0)
            return INV_EVENT;

        /* UID must not exceed bounds */
        int ended = 0; // flag to see if we have reached nullterm yet
        for (int i = 0; i < 1000; i++)
            if ((tmpEvent->UID)[i] == '\0')
            {
                ended = 1;
                break;
            }
        if (ended == 0)
            return INV_EVENT;

        /* creationDateTime (DTSTART) date/time must not be empty nor overflowed */
        if (strcmp((tmpEvent->creationDateTime).date, "") == 0)
            return INV_EVENT;
        if (strcmp((tmpEvent->creationDateTime).time, "") == 0)
            return INV_EVENT;
        /* Overflow check for date/time */
        ended = 0;
        for (int i = 0; i < 9; i++)
            if (((tmpEvent->creationDateTime).date)[i] == '\0')
            {
                ended = 1;
                break;
            }
        if (ended == 0)
            return INV_EVENT;
        ended = 0;
        for (int i = 0; i < 7; i++)
            if (((tmpEvent->creationDateTime).time)[i] == '\0')
            {
                ended = 1;
                break;
            }
        if (ended == 0)
            return INV_EVENT;
        /* Strlen check */
        if (strlen(((tmpEvent->creationDateTime).date)) != 8)
            return INV_EVENT;
        if (strlen(((tmpEvent->creationDateTime).time)) != 6)
            return INV_EVENT;

        /* startDateTime (DTSTAMP) must neither be empty nor overflowed */
        if (strcmp((tmpEvent->startDateTime).date, "") == 0)
            return INV_EVENT;
        if (strcmp((tmpEvent->startDateTime).time, "") == 0)
            return INV_EVENT;
        /* Overflow check for date/time */
        ended = 0;
        for (int i = 0; i < 9; i++)
            if (((tmpEvent->startDateTime).date)[i] == '\0')
            {
                ended = 1;
                break;
            }
        if (ended == 0)
            return INV_EVENT;
        ended = 0;
        for (int i = 0; i < 7; i++)
            if (((tmpEvent->startDateTime).time)[i] == '\0')
            {
                ended = 1;
                break;
            }
        if (ended == 0)
            return INV_EVENT;
        /* Strlen check */
        if (strlen(((tmpEvent->startDateTime).date)) != 8)
            return INV_EVENT;
        if (strlen(((tmpEvent->startDateTime).time)) != 6)
            return INV_EVENT;

        /* List *properties must not be NULL */
        if (tmpEvent->properties == NULL)
            return INV_EVENT;

        /* List *alarms must not be NULL */
        if (tmpEvent->alarms == NULL)
            return INV_EVENT;

        /* UID, DTSTART, DTSTAMP must occur exactly once. This means
         * if they exist in the properties list, then it's bad.
         * Also looking for foreign properties that shouldn't exist.
         * Also checking occurrence rule as shown in the array below.
         */
        int propRec[27] = {0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0, 0, 0, 0, 0};
        ListIterator eventProperties = createIterator(tmpEvent->properties);
        void *eventProperty = NULL;
        int dtendExists = 0, durationExists = 0; // exclusivity
        while ((eventProperty = nextElement(&eventProperties)) != NULL)
        {
            /* Temp cast */
            Property *tmpProp = (Property*)eventProperty;

            /* Checking for null termination */
            int eventPropNameHasNullTerm = 0;
            for (int k = 0; k < 200; k++)
                if ((tmpProp->propName)[k] == '\0')
                {
                    eventPropNameHasNullTerm = 1;
                    break;
                }
            if (eventPropNameHasNullTerm == 0)
                return INV_EVENT;

            /* UID, DTSTART, and DTSTAMP are internal and can't exist
             * in the properties list... */
            if (strcmp(tmpProp->propName, "UID") == 0)
                return INV_EVENT;
            if (strcmp(tmpProp->propName, "DTSTART") == 0)
                return INV_EVENT;
            if (strcmp(tmpProp->propName, "DTSTAMP") == 0)
                return INV_EVENT;
            /* Checking DTEND and DURATION exclusivity */
            if (strcmp(tmpProp->propName, "DTEND") == 0)
                dtendExists = 1;
            if (strcmp(tmpProp->propName, "DURATION") == 0)
                durationExists = 1;
            if (dtendExists == 1 && durationExists == 1)
                return INV_EVENT;

            /* for loop that iterates through the list of allowed
             * properties. It must match at least one of the strings
             * in the array as those are the whitelisted properties.
             * If we reach the end of the list (i.e. the i pointer is
             * touching the NULL terminator of the list), then we return
             * an INV_EVENT error. */
            int i = 0;
            for (; whitelistedProperties[i] != NULL; i++)
                if (strcmp(tmpProp->propName, whitelistedProperties[i]) == 0)
                    break;
            if (whitelistedProperties[i] == NULL)
                return INV_EVENT;

            /* Iterating over the occurrence rules... certain event props
             * cannot happen more than once. */
            int j = 0;
            for (; optionalRestrictedProps[j] != NULL; j++)
                if (strcmp(tmpProp->propName, optionalRestrictedProps[j]) == 0)
                    if (++propRec[j] > 1)
                        return INV_EVENT;

            /* Checking for blank propDescr */
            if (tmpProp->propDescr[0] == '\0' || strlen(tmpProp->propDescr) == 0)
                return INV_EVENT;

        }
    }

/* --------- *
 * INV_ALARM 
 * --------- */

    /* Iterating over our events again with a different iterator */
    ListIterator eventAlmIter = createIterator(obj->events);
    event = NULL;
    /* List of alarm properties */

        // Alarm props that only occur once 
        char *almPropWhitelist[] =
        {
            "DURATION", "REPEAT", "ATTACH", NULL
        };

    /* For each event... */
    while ((event = nextElement(&eventAlmIter)) != NULL)
    {
        /* Temporary cast */
        Event *tmpEvent = (Event*)event;
        
        /* Instead of iterating over the properties, we are now
         * going to iterate over the alarms */
        ListIterator alarms = createIterator(tmpEvent->alarms);
        void *alarm = NULL; // temporary counter
        while ((alarm = nextElement(&alarms)) != NULL)
        {
            /* Temporary cast */
            Alarm *tmpAlarm = (Alarm*)alarm;
            
            /* Alarm action must not be empty */
            if (strcmp(tmpAlarm->action, "") == 0)
                return INV_ALARM;

            /* Alarm action must not exceed the array bound */
            int actionHasNullTerm = 0;
            for (int i = 0; i < 200; i++)
                if (tmpAlarm->action[i] == '\0')
                {
                    actionHasNullTerm = 1;
                    break;
                }
                // checking our flag
            if (actionHasNullTerm == 0)
                return INV_ALARM;

            /* Alarm trigger must not be NULL */
            if (tmpAlarm->trigger == NULL)
                return INV_ALARM;

            /* Alarm trigger must not be empty */
            if (tmpAlarm->trigger[0] == '\0' || strcmp(tmpAlarm->trigger, "") == 0)
                return INV_ALARM;

            /* List *properties must not be NULL but it may be empty */
            if (tmpAlarm->properties == NULL)
                return INV_ALARM;

            /* Iterate all properties for
             * - what's allowed and not allowed
             * - if anything is required
             * - duplicate rules
             * - propName  empty
             * - propName  blank
             * - propDescr empty
             * - propDescr blank 
             * - special occurrence rules*/
            
            /* If tmpAlarm->properties was NULL it would've been caught */
            ListIterator almProps = createIterator(tmpAlarm->properties);
            void *almProp = NULL;
            /* Flags for DURATION and REPEAT properties */
            int hasDuration = 0, hasRepeat = 0;
            /* Resetting our occurrences */
            int almPropRec[3] = {0, 0, 0};
            /* Third iterator */
            while ((almProp = nextElement(&almProps)) != NULL)
            {
                /* Temp cast */
                Property *tmpProp = (Property*)almProp;

                /* Checking propName for null termination */
                int alarmPropNameTerm = 0;
                for (int l = 0; l < 200; l++)
                    if ((tmpProp->propName)[l] == '\0')
                    {
                        alarmPropNameTerm = 1;
                        break;
                    }
                if (alarmPropNameTerm == 0)
                    return INV_ALARM;

                /* ACTION and TRIGGER are internal and can't occur */
                if (strcmp(tmpProp->propName, "ACTION") == 0)
                    return INV_ALARM;
                if (strcmp(tmpProp->propName, "TRIGGER") == 0)
                    return INV_ALARM;

                /* Marking DURATION and REPEAT inclusivity */
                if (strcmp(tmpProp->propName, "DURATION") == 0)
                    hasDuration = 1;
                if (strcmp(tmpProp->propName, "REPEAT") == 0)
                    hasRepeat = 1;

                /* Checking for allowed properties and duplication */
                int i = 0;
                for (; almPropWhitelist[i] != NULL; i++)
                    if (strcmp(tmpProp->propName, almPropWhitelist[i]) == 0)
                        break;
                if (almPropWhitelist[i] == NULL)
                    return INV_ALARM;

                /* Iterating over duplication rules */
                int j = 0;
                for (; almPropWhitelist[j] != NULL; j++)
                    if (strcmp(tmpProp->propName, almPropWhitelist[j]) == 0)
                        if (++almPropRec[j] > 1)
                            return INV_ALARM;

                /* Checking for blank propDescr*/
                if (tmpProp->propDescr[0] == '\0' || strlen(tmpProp->propDescr) == 0)
                    return INV_ALARM;
            }
            /* This is very hacky but I don't care at this point */
            if ((hasDuration == 1 && hasRepeat == 0) ||
                (hasDuration == 0 && hasRepeat == 1))
                return INV_ALARM;
        }
    }

/* ----------- *
 * OTHER_ERROR 
 * ----------- */

    // TODO

    /* If we somehow reach this point, then we're good. */
    return OK;
}

/* writeCalendar */
ICalErrorCode writeCalendar (char* fileName, const Calendar *obj)
{
    printf("writeCalendar called.\n");
    /* Variables */
    FILE *fp; // output file for writing

    /* Block identifiers */
    typedef enum c { CAL_VER, CAL_PROD, CAL_PROP, CAL_EVT } CalendarBlock;
    typedef enum e { EVT_UID, EVT_CREATE, EVT_START, EVT_PROP, EVT_ALM } EventBlock;
    typedef enum a { ALM_ACT, ALM_TRIG, ALM_PROP } AlarmBlock;

    /* Our defined sequence */
    CalendarBlock calSequence[4] = {CAL_PROD, CAL_VER, CAL_EVT, CAL_PROP};
    EventBlock evtSequence[5] = {EVT_UID, EVT_CREATE, EVT_START, EVT_PROP, EVT_ALM};
    AlarmBlock almSequence[3] = {ALM_TRIG, ALM_PROP, ALM_ACT};

    /* Validate the two parameters for NULL */
    if (fileName == NULL || obj == NULL)
        return WRITE_ERROR;

    /* Attempt to open the file for writing */
    fp = fopen(fileName, "w"); // "w" will overwrite

    if (fp == NULL)
        return WRITE_ERROR;

    /* ------------------ BEGIN:VCALENDAR\r\n ----------------
     * As per the Augmented Backus-Naur Form, all iCalendar objects
     * must start with BEGIN:CALENDAR with proper CR LF line break.
     * ------------------------------------------------------- */
        fprintf(fp, "BEGIN:VCALENDAR\r\n");
    /* ------------------------------------------------------- */

    /* Begin the Calendar Sequence */
    for (int i = 0; i < MAX_CALENDAR; i++)
    {
        switch (calSequence[i])
        {
            case CAL_VER:;
            /* -------------------- WRITE VERSION -------------------- 
             * All iCalendar objects must have a version. By the time we
             * reach writeCalendar(), the possibility of having no version
             * should ideally be gone.
             * ------------------------------------------------------- */
                // printf("Writing version\n");
                float version = obj->version;
                fprintf(fp, "VERSION:%.1f\r\n", version);
                break;
            /* ------------------------------------------------------- */

            case CAL_PROD:;
            /* -------------------- WRITE PRODID --------------------- 
             * All iCalendar objects must have a PRODID (product ID). by
             * the time we reach writeCalendar(), the possibility of having
             * no PRODID should ideally be gone.
             * ------------------------------------------------------- */
                // printf("Writing PRODID\n");
                char PRODID[1000];
                strcpy(PRODID, obj->prodID);
                fprintf(fp, "PRODID:%s\r\n", PRODID);
                break;
            /* ------------------------------------------------------- */

            case CAL_PROP:;
            /* ----------------- WRITE PROPERTY(IES) -----------------
             * In addition to the two mandatory properties above, an iCal
             * object may have any number (>=0) of optional properties.
             * Out of stylistic choice, I have decided to write the
             * properties of the calendar object before the events.
             *
             * List *properties  = a reference copy of the iCal object's
             *                     calendar property list
             *
             * ListIterator 
             *      propIter     = an iterator allocated onto the stack
             *                     that safely traverses the properties
             *
             * void *property    = temporary counter variable
             * ------------------------------------------------------- */
                // printf("Writing properties\n");
                List *properties = obj->properties;
                ListIterator propIter = createIterator(properties);
                void *property;
                
                /* This while loop will iterate through all of the iCal
                * object's properties until it reaches NULL. This is a
                * much safer approach then just blindly poking the list.
                *
                * Property *tmpProperty = temporary casting variable that
                *                         we use in order to cast the void
                *
                * char tmpPropName[200] = temporary variable used to store
                *                         the property name of the prop
                *
                * size_t propDescrLen   = strlen() size for the flexible
                *                         array member of the description
                *
                * char tmpPropDescr[propDescrLen + 1]
                *                       = the actual array to hold descr
                */

                while ((property = nextElement(&propIter)) != NULL)
                {
                    Property *tmpProperty = (Property*)property;

                    char tmpPropName[200];

                    size_t propDescrLen = strlen(tmpProperty->propDescr);

                    char tmpPropDescr[propDescrLen + 1];

                    /* Copying over by value */
                    strcpy(tmpPropName, tmpProperty->propName);
                    strcpy(tmpPropDescr, tmpProperty->propDescr);

                    /* The actual writing */

                    fprintf(fp, "%s%s%s\r\n", tmpPropName, 
                            containsColon(tmpPropDescr) == 1
                            ? ";" : ":", tmpPropDescr);
                }
                break;
            /* ------------------------------------------------------- */

            case CAL_EVT:;
            /* -------------------- WRITE EVENT(S) -------------------
             * All iCalendar objects must have at least one event component
             * dictated by an iana-comp or x-comp tag. By the time we reach
             * writeCalendar(), the possibility of having not a single event
             * should ideally be gone.
             *
             * List *events      = a reference copy of the iCal object's
             *                     calendar event list
             *
             * ListIterator iter = an iterator allocated onto the stack
             *                     that safely traverses the events
             *
             * void *event       = temporary counter for event
             * ------------------------------------------------------ */
                // printf("Writing events\n");
                List *events = obj->events;
                ListIterator eventIter = createIterator(events);
                void *event;

                /* An iterator that walks through all events */
                while ((event = nextElement(&eventIter)) != NULL)
                {
                    Event *tmpEvent = (Event*)event; // temp cast

                    fprintf(fp, "BEGIN:VEVENT\r\n"); // Event start

                    /* We start our event block sequence */
                    for (int i = 0; i < MAX_EVENT; i++)
                    {
                        switch (evtSequence[i])
                        {
                            case EVT_UID:;
                                // printf("\tWriting event UID\n");
                                /* Temporary buffer */
                                char tmpUID[1000];
                                /* Copying to this buffer */
                                strcpy(tmpUID, tmpEvent->UID);
                                /* Writing it to the file */
                                fprintf(fp, "UID:%s\r\n", tmpUID);
                                break;

                            case EVT_CREATE:; // DTSTAMP = creationDateTime
                                // printf("\tWriting event create\n");
                                /* Temporary buffer */
                                DateTime tmpCreationDateTime;
                                /* Copying it over by value */
                                strcpy(tmpCreationDateTime.date,
                                        tmpEvent->creationDateTime.date);
                                strcpy(tmpCreationDateTime.time,
                                        tmpEvent->creationDateTime.time);
                                tmpCreationDateTime.UTC =
                                        tmpEvent->creationDateTime.UTC;
                                /* Writing it to the file */
                                fprintf(fp, "DTSTAMP:%sT%s", 
                                        tmpCreationDateTime.date,
                                        tmpCreationDateTime.time);
                                /* If it is UTC, we append a Z */
                                if (tmpCreationDateTime.UTC)
                                    fprintf(fp, "Z");
                                /* Close off the line as usual */
                                fprintf(fp, "\r\n");
                                break;

                            case EVT_START:; // DTSTART = startDateTime
                                // printf("\tWriting event start\n");
                                /* Temporary buffer */
                                DateTime tmpStartDateTime;
                                /* Copying it over by value */
                                strcpy(tmpStartDateTime.date,
                                        tmpEvent->startDateTime.date);
                                strcpy(tmpStartDateTime.time,
                                        tmpEvent->startDateTime.time);
                                tmpStartDateTime.UTC =
                                        tmpEvent->startDateTime.UTC;
                                /* Writing it to the file */
                                fprintf(fp, "DTSTART:%sT%s",
                                        tmpStartDateTime.date,
                                        tmpStartDateTime.time);
                                /* If it is UTC, we append a Z */
                                if (tmpStartDateTime.UTC)
                                    fprintf(fp, "Z");
                                /* Close off the line as usual */
                                fprintf(fp, "\r\n");
                                break;

                            case EVT_PROP:;
                                // printf("Writing properties\n");
                                /* TODO: boilerplate code */
                                List *eventProperties = tmpEvent->properties;
                                ListIterator eventPropIter = createIterator(eventProperties);
                                void *property;

                                while ((property = nextElement(&eventPropIter)) != NULL)
                                {
                                    Property *tmpProperty = (Property*)property;

                                    char tmpPropName[200];

                                    size_t propDescrLen = strlen(tmpProperty->propDescr);

                                    char tmpPropDescr[propDescrLen + 1];

                                    /* Copying over by value */
                                    strcpy(tmpPropName, tmpProperty->propName);
                                    strcpy(tmpPropDescr, tmpProperty->propDescr);

                                    /* The actual writing */
                                    fprintf(fp, "%s%s%s\r\n", tmpPropName, 
                                    containsColon(tmpPropDescr) == 1
                                    ? ";" : ":", tmpPropDescr);

                                }
                                break;

                            case EVT_ALM:;
                                // printf("\tWriting event alarm\n");
                                /*----------WRITE ALARMS IF ANY----------
                                 * Events don't need alarms though.
                                 * List *alarms = ref copy of alarm list
                                 * ListIterator alarmIter = alm iterator
                                 * void *alarm = temporary alarm counter
                                 *---------------------------------------*/
                                List *alarms = tmpEvent->alarms;
                                ListIterator alarmIter = createIterator(alarms);
                                void *alarm;

                                while ((alarm = nextElement(&alarmIter)) != NULL)
                                {
                                    Alarm *tmpAlarm = (Alarm*)alarm; // temp cast

                                    fprintf(fp, "BEGIN:VALARM\r\n"); // alarm start

                                    for (int i = 0; i < MAX_ALARM; i++)
                                    {
                                        switch(almSequence[i])
                                        {
                                            case ALM_ACT:;
                                                // printf("\t\tWriting alarm action\n");
                                                /* Temporary action for copying */
                                                char tmpAction[200];
                                                /* Copy it over */
                                                strcpy(tmpAction, tmpAlarm->action);
                                                /* Write it */
                                                fprintf(fp, "ACTION%s%s\r\n", containsColon(tmpAction) == 1 ? ";" : ":", tmpAction);
                                                break;

                                            case ALM_TRIG:;
                                                {// printf("\t\tWriting alarm trigger\n");
                                                /* Determine how big */
                                                size_t len = strlen(tmpAlarm->trigger);
                                                /* Create the temporary trigger */
                                                char tmpTrigger[len + 1];
                                                /* Copy it over */
                                                strcpy(tmpTrigger, tmpAlarm->trigger);
                                                /* Write it */
                                                fprintf(fp, "TRIGGER%s%s\r\n", containsColon(tmpTrigger) == 1 ? ";" : ":", tmpTrigger);
                                                break;}

                                            case ALM_PROP:;
                                                // printf("\t\tWriting alarm property\n");
                                                /* TODO: boilerplate code */
                                                List *alarmProperties = tmpAlarm->properties;
                                                ListIterator alarmPropIter = createIterator(alarmProperties);
                                                void *property;

                                                while ((property = nextElement(&alarmPropIter)) != NULL)
                                                {
                                                    Property *tmpProperty = (Property*)property;

                                                    char tmpPropName[200];

                                                    size_t propDescrLen = strlen(tmpProperty->propDescr);

                                                    char tmpPropDescr[propDescrLen + 1];

                                                    /* Copying over by value */
                                                    strcpy(tmpPropName, tmpProperty->propName);
                                                    strcpy(tmpPropDescr, tmpProperty->propDescr);

                                                    /* The actual writing */
                                                    fprintf(fp, "%s%s%s\r\n", tmpPropName, 
                                                    containsColon(tmpPropDescr) == 1
                                                    ? ";" : ":", tmpPropDescr);

                                                }
                                                break;
                                        }
                                    }

                                    fprintf(fp, "END:VALARM\r\n"); // alarm end
                                }
                                break;

                        }
                    }

                    fprintf(fp, "END:VEVENT\r\n"); // Event end
                }
                break;
            /* ------------------------------------------------------- */
        }
    }

    /* -------------------- END:VCALENDAR\r\n ---------------
     * As per the Augmented Backus-Naur Form, all iCalendar objects
     * must end with END:VCALENDAR with proper CR LF line break.
     * ------------------------------------------------------ */
    fprintf(fp, "END:VCALENDAR\r\n");
    /* ------------------------------------------------------- */

    /* Attempt to close the file from writing */
    if (fclose(fp) != 0) // rare error... I hope it doesn't happen
        return WRITE_ERROR;

    /* If we have reached this point, it is an OK from me! */

    printf("Successfully written Calendar to %s\n", fileName);

    return OK;
}

char *printError(ICalErrorCode err)
{
    char *toReturn = calloc(1, sizeof(char) * 50);
    switch (err)
    {
        case INV_FILE:
            strcpy(toReturn, "Invalid file");
            break;

        case INV_CAL:
            strcpy(toReturn, "Invalid calendar");
            break;

        case INV_VER:
            strcpy(toReturn, "Invalid version");
            break;

        case DUP_VER:
            strcpy(toReturn, "Duplicate version");
            break;

        case INV_PRODID:
            strcpy(toReturn, "Invalid prodid");
            break;

        case DUP_PRODID:
            strcpy(toReturn, "Duplicate prodid");
            break;

        case INV_EVENT:
            strcpy(toReturn, "Invalid event");
            break;

        case INV_DT:
            strcpy(toReturn, "Invalid DateTime");
            break;

        case INV_ALARM:
            strcpy(toReturn, "Invalid alarm");
            break;

        case OTHER_ERROR:
            strcpy(toReturn, "Other error");
            break;

        case WRITE_ERROR:
            strcpy(toReturn, "Write error");
            break;

        case OK:
            strcpy(toReturn, "OK");
            break;

        default:
            strcpy(toReturn, "Invalid Error Code");
            break;

    }

    return toReturn;
}

int validLineEndings(char *toParse)
{
    if (toParse == NULL) return 0;
    int i = 0;
    while (toParse[i] != '\0')
    {
        if (toParse[i] == '\r' && toParse[i + 1] != '\n')
        {
            return 0;
        }
        else if (toParse[i] == '\n')
        {
            return 0;
        }
        else if (toParse[i] == '\r' && toParse[i + 1] == '\n')
        {
            i += 2;
        }
        else
        {
            i++;
        }
    }

    if (toParse[i] == '\0')
    {
        if (toParse[i - 1] != '\n')
            return 0;
        if (toParse[i - 2] != '\r')
            return 0;
    }


    return 1; // good
}


int validateDateTime(char *toParse)
{
    if (toParse == NULL) return 0;
    //YYYYMMDD char date[9]       = 8 chars, always
    //T                           = 1 char, always
    //HHMMSS char time[7]         = 6 chars, always
    //Z or no Z depending on UTC  = 1 or 0 chars

    /* The string has to exist */
    if (toParse == NULL)
        return 0;

    /* The string must not be empty */
    if (strcmp(toParse, "") == 0)
        return 0;

    /* The strlen must return 15 or 16. */
    if (strlen(toParse) != 15 && strlen(toParse) != 16)
        return 0;

    /* The first 8 char. must be numeric. */
    for (int i = 0; i < 8; i++)
        if (isdigit(toParse[i]) == 0)
            return 0;

    /* The next char. must be T. */
    if (toParse[8] != 'T')
        return 0;

    /* The next 6 char. must be numeric. */
    for (int i = 9; i < 15; i++)
        if (isdigit(toParse[i]) == 0)
            return 0;

    /* If a last character exists, it must be Z */
    if (strlen(toParse) == 16)
        if (toParse[15] != 'Z')
            return 0;
    return 1; // good
}

DateTime *getDateTime(char *toParse)
{
    if (toParse == NULL) return NULL;
    DateTime *toReturn = NULL;
    toReturn = calloc(1, sizeof(DateTime));
    if (toReturn != NULL)
    {
        // 16 means probably UTC
        // 15 probably means non-UTC
        for (int i = 0; i < 8; i++)
        {
            (toReturn->date)[i] = toParse[i];
        }
        for (int i = 9; i < 15; i++)
        {
            (toReturn->time)[i - 9] = toParse[i];
        }

        if (strlen(toParse) == 15)
        {
            toReturn->UTC = false;
        }
        else if (strlen(toParse) == 16)
        {
            toReturn->UTC = true;
        }
        else
        {
            return NULL;
        }
    }
    return toReturn;
}

char *printDate(void *toBePrinted)
{
    char *tmpStr;
    DateTime* tmpDate;

    if (toBePrinted == NULL)
        return NULL;

    tmpDate = (DateTime*)toBePrinted;

    tmpStr = calloc(1, 52);

    if (tmpStr == NULL) return NULL;

    sprintf(tmpStr, "Date: %s\n\tTime: %s\n\tUTC: %s", tmpDate->date, tmpDate->time, tmpDate->UTC ? "true" : "false");

    return tmpStr;
}

char *printProperty(void *toBePrinted)
{
    char *tmpStr;
    Property *tmpProp;
    int len;

    if (toBePrinted == NULL)
        return NULL;

    tmpProp = (Property*)toBePrinted;

    len = (6 + 200 + 1 + 6 + strlen(tmpProp->propDescr) + 1 + 21);
    tmpStr = calloc(1, sizeof(char) * len);
    if (tmpStr == NULL) return NULL;
    sprintf(tmpStr, "\tName: %s\n\tDesc: %s\n", tmpProp->propName, tmpProp->propDescr);
    return tmpStr;
}

char *printAlarm(void *toBePrinted)
{
    char *tmpStr;
    Alarm *tmpAlarm;
    int len;

    if (toBePrinted == NULL)
        return NULL;

    tmpAlarm = (Alarm*)toBePrinted;

    char *listToString = toString(tmpAlarm->properties);

    len = (8 + 200 + 1 + 9 + strlen(tmpAlarm->trigger) + 1 + 1 + 12 + strlen(listToString) + 1 + 35);

    tmpStr = calloc(1, sizeof(char) * len);
    if (tmpStr == NULL) return NULL;

    sprintf(tmpStr, "\tALARM\n\t==================\n\tAction: %s\n\tTrigger: %s\n\n\tProperties: %s\n\t==================", tmpAlarm->action, tmpAlarm->trigger, listToString);

    free(listToString);
    return tmpStr;
}

char *printEvent(void *toBePrinted)
{
    char *tmpStr;
    Event *tmpEvent;
    int len;

    if (toBePrinted == NULL)
    {
        return NULL;
    }

    tmpEvent = (Event*)toBePrinted;

    /**
     * 20191026235959U
     */

    char *tmpEventDTcreated = printDate((void*)&(tmpEvent->creationDateTime));
    char *tmpEventDTstarted = printDate((void*)&(tmpEvent->startDateTime));
    char *propertiesToString = toString(tmpEvent->properties);
    char *alarmsToString = toString(tmpEvent->alarms);

    /*
     * Length of the string is:
     * - 'UID: '
     * - 1000 byte string for the UID
     * - newline
     * - 'Event created: '
     * - DateTime variable for creationDateTime
     * - newline
     * - 'Event starts: '
     * - DateTime variable for startDateTime
     * - null terminator
     */
    len = (240 + 1000 + strlen(tmpEventDTcreated) + strlen(tmpEventDTstarted) + strlen(propertiesToString) + strlen(alarmsToString));
    tmpStr = calloc(1, (sizeof(char) * len) + 6);
    if (tmpStr == NULL) return NULL;

    sprintf(tmpStr, "|||||||||||EVENT||||||||||||\n\tUID: %s\n\n\tEvent created: \n\t%s\n\n\tEvent starts: \n\t%s\n\n\tProperties: \n\t%s\n\n\tAlarms: \n\t%s\n||||||||||||||||||||||||||||", tmpEvent->UID, tmpEventDTcreated, tmpEventDTstarted, propertiesToString, alarmsToString);

    free(tmpEventDTcreated);
    free(tmpEventDTstarted);
    free(propertiesToString);
    free(alarmsToString);
    return tmpStr;
}

void deleteEvent(void *toBeDeleted)
{
    Event *tmpEvent;

    if (toBeDeleted == NULL)
        return;

    tmpEvent = (Event*)toBeDeleted;

    // free the properties
    freeList(tmpEvent->properties);
    // free the alarms
    freeList(tmpEvent->alarms);
    // free the Event, finally
    free(tmpEvent);
    return;
}
void deleteAlarm(void *toBeDeleted)
{
    Alarm *tmpAlarm;

    if (toBeDeleted == NULL)
        return;

    tmpAlarm = (Alarm*)toBeDeleted;

    // free everything in the list
    freeList(tmpAlarm->properties);
    // free the trigger
    free(tmpAlarm->trigger);
    // free the Alarm
    free(tmpAlarm);
    return;
}
void deleteProperty(void *toBeDeleted)
{
    Property *tmpProperty;

    if (toBeDeleted == NULL)
        return;

    tmpProperty = (Property*)toBeDeleted;
    free(tmpProperty);
    return;
}
void deleteDate(void *toBeDeleted)
{
    /*DateTime *tmpDate;

      if (toBeDeleted == NULL)
      return;

      tmpDate = (DateTime*)toBeDeleted;
      free(tmpDate);*/
    // TODO is this logical
    return;
}

int compareEvents(const void *first, const void *second)
{
    Event *tmpEvent1;
    Event *tmpEvent2;

    if (first == NULL || second == NULL)
        return 0;

    tmpEvent1 = (Event*)first;
    tmpEvent2 = (Event*)second;
    return strcmp((char*)tmpEvent1->UID, (char*)tmpEvent2->UID);
    return 0;
}

int compareProperties(const void *first, const void *second)
{
    Property *tmpProp1;
    Property *tmpProp2;
    if (first == NULL || second == NULL)
        return 0;
    tmpProp1 = (Property*)first;
    tmpProp2 = (Property*)second;
    return strcmp((char*)tmpProp1->propName, (char*)tmpProp2->propName);
}

int compareDates(const void *first, const void *second)
{
    // nub will always return 0 =) . . . for now
    return 0;
}

int compareAlarms(const void *first, const void *second)
{
    Alarm *tmpAlarm1;
    Alarm *tmpAlarm2;
    if (first == NULL || second == NULL)
        return 0;
    tmpAlarm1 = (Alarm*)first;
    tmpAlarm2 = (Alarm*)second;
    return strcmp((char*)tmpAlarm1->action, (char*)tmpAlarm2->action);
}

int extensionCheck(char *fileName)
{
    if (fileName == NULL)
    {
        return 0;
    }
    // iterating over the entire fileName
    for (int i = 0; i < strlen(fileName); i++)
    {
        if (fileName[i] == '.') // if .
            if ((i + 4) == strlen(fileName)) // if we are at the end
                if(fileName[i + 1] == 'i' || fileName[i + 1] == 'I') // i
                    if(fileName[i + 2] == 'c' || fileName[i + 2] == 'C') // c 
                        if(fileName[i + 3] == 's' || fileName[i + 3] == 'S') // s
                            if(fileName[i + 4] == '\0') // if we really are at the end
                                return 1; // then we are good
    }
    return 0; // otherwise we return 0, which in this case is bad
}

ICalErrorCode createCalendar(char *fileName, Calendar** obj)
{
    // why is this even a thing
    // you honestly could have used a stack of enums here
    if (fileName == NULL)
        return INV_FILE;

    if (obj == NULL)
        return OTHER_ERROR;

    // checking if the file ends in .ics
    if (extensionCheck(fileName) != 1)
    {
        (*obj) = NULL; //TODO INV_FILE
        return INV_FILE; // invalid_file error
    }

    FILE *fp = fopen(fileName, "r"); // try to open the file for reading
    if (fp == NULL) // checking if the file has truly been opened
    {
        (*obj) = NULL; // we set the file to NULL to signify something bad happened
        return INV_FILE; // then we return the invalid_file error //TODO INV_FILE
    }

    (*obj) = calloc(1, sizeof(Calendar));
    if ((*obj) == NULL)
    {
        return INV_FILE; //TODO this isn't the right error
    }

    fseek(fp, 0, SEEK_END); 
    long fsize = (long)ftell(fp); // this and the above step are for filesize alloc
    rewind(fp); // this is for when I read from the file again

    char *buffer = calloc(fsize + 1, sizeof(char)); // allocating based on size

    if (buffer != NULL)
    {
        char **lines = calloc(1, sizeof(char*) * MAX_LINE_COUNT);
        if (!lines)
        {
            free(buffer);
            free(*obj);
            (*obj) = NULL;
            return OTHER_ERROR;
        }
        fread(buffer, fsize, 1, fp); // reading based on previous size constraint
        if (buffer == NULL)
        {
            free(*obj);
            free(buffer); // the string is done being used I guess
            fclose(fp);
            free(lines);
            (*obj) = NULL;
            return INV_FILE;
        }
        else if (strcmp(buffer, "") == 0)
        {
            free(*obj);
            free(buffer); // the string is done being used I guess
            fclose(fp);
            free(lines);
            (*obj) = NULL;
            return INV_FILE;
        }
        char *unfoldedBuffer = unfold(buffer);
        if (validLineEndings(unfoldedBuffer) == 0)
        {
            free(*obj);
            free(buffer); // the string is done being used I guess
            free(unfoldedBuffer);
            fclose(fp);
            free(lines);
            (*obj) = NULL;
            return INV_FILE;
        }
        int lastLine = -1;
        char *token = strtok(unfoldedBuffer, "\r\n");
        if (token == NULL)
        {
            free(*obj);
            free(buffer);
            free(unfoldedBuffer);
            fclose(fp);
            free(lines);
            (*obj) = NULL;
            return INV_FILE;
        }
        if (token[0] != ';'){
            lines[++lastLine] = calloc(1, sizeof(char) * strlen(token) + 1);
            strcpy(lines[lastLine], token);}
        while (token != NULL)
        {
            token = strtok(NULL, "\r\n\0");
            if (token != NULL)
            {
                lines[++lastLine] = calloc(1, sizeof(char) * strlen(token) + 1);
                strcpy(lines[lastLine], token);
            }
        }

        //for (int i = 0; i <= lastLine; i++)
        //  printf("%s\n", lines[i]);


        (*obj)->events = initializeList(printEvent, deleteEvent, compareEvents);
        (*obj)->properties = initializeList(printProperty, deleteProperty, compareProperties);

        // for (int i = 0; i <= lastLine; i++)
        int i = 0;

        if ((strcmp(lines[lastLine], "END:VCALENDAR") != 0) || (strcmp(lines[0], "BEGIN:VCALENDAR") != 0)) // first and last line checking
        {
            free((*obj)->events);
            free((*obj)->properties);
            free(*obj);
            (*obj) = NULL;
            free(buffer);
            free(unfoldedBuffer);
            fclose(fp);
            for (int i = 0; i <= lastLine; i++)
                free(lines[i]);
            free(lines);
            return INV_CAL;
        }

        int prodidFound = 0, versionFound = 0, hasOneEvent = 0;
        while (i <= lastLine)
        { //probably could have used a stack of enums here
            char temp[MAX_LINE];
            char *newToken;
            strcpy(temp, lines[i]);

            if (temp != NULL)
            {
                if (strcmp(temp, "BEGIN:VALARM") == 0 || (strcmp(temp, "BEGIN:VCALENDAR") == 0 && (i != 0)) || strcmp(temp, "END:VEVENT") == 0 || strcmp(temp, "END:VALARM") == 0)
                {
                    deleteCalendar(*obj);
                    (*obj) = NULL;
                    free(buffer);
                    free(unfoldedBuffer);
                    fclose(fp);
                    for (int i = 0; i <= lastLine; i++)
                        free(lines[i]);
                    free(lines);
                    return INV_CAL;
                    // TODO return invalid calendar
                }

                /* Event is encountered */
                else if (strcmp(temp, "BEGIN:VEVENT") == 0)
                {
                    // printf("\tEVENT HAS STARTED\n");
                    int eventContainsUID = 0;
                    int eventContainsDTStamp = 0;
                    int eventContainsDTStart = 0;
                    Event *newEvent = calloc(1, sizeof(Event));
                    newEvent->properties = initializeList(printProperty, deleteProperty, compareProperties);
                    newEvent->alarms = initializeList(printAlarm, deleteAlarm, compareAlarms);
                    i++;
                    while (i <= lastLine)
                    {
                        strcpy(temp, lines[i]);
                        /* Event cannot encounter another Calendar, a Calendar end, another event, or an alarm end */
                        if ((strcmp(temp, "BEGIN:VCALENDAR") == 0) || (strcmp(temp, "BEGIN:VEVENT") == 0) || (strcmp(temp, "END:VCALENDAR") == 0) || (strcmp(temp, "END:VALARM") == 0))
                        {

                            deleteCalendar(*obj);
                            (*obj) = NULL;

                            free(buffer);
                            free(unfoldedBuffer);
                            fclose(fp);
                            for (int i = 0; i <= lastLine; i++)
                                free(lines[i]);
                            free(lines);
                            deleteEvent(newEvent);
                            return INV_EVENT;

                        }

                        /* Event end */
                        if (strcmp(temp, "END:VEVENT") == 0)
                        {
                            /* check if it has the required properties:
                             * an event requires the following:
                             * - UID
                             * - creationDateTime
                             * - startDateTime */

                            if (eventContainsUID == 0 || eventContainsDTStamp == 0 || eventContainsDTStart == 0)
                            {
                                deleteCalendar(*obj);
                                (*obj) = NULL;

                                free(buffer);
                                free(unfoldedBuffer);
                                fclose(fp);
                                for (int i = 0; i <= lastLine; i++)
                                    free(lines[i]);
                                free(lines);
                                deleteEvent(newEvent);
                                return INV_EVENT;

                            }

                            insertBack((*obj)->events, newEvent);
                            // TODO add check
                            hasOneEvent = 1;
                            // printf("\tENDING VEVENT\n");
                            break;
                        }


                        else if (strcmp(temp, "BEGIN:VALARM") == 0)
                        {
                            // printf("\t\t\tALARM INSIDE EVENT STARTED\n");
                            Alarm *newAlarm = calloc(1, sizeof(Alarm));
                            newAlarm->properties = initializeList(printProperty, deleteProperty, compareProperties);
                            i++;
                            int alarmContainsTrigger = 0;
                            int alarmContainsAction = 0;
                            while (i <= lastLine)
                            {
                                strcpy(temp, lines[i]);
                                if (strcmp(temp, "BEGIN:VALARM") == 0 || strcmp(temp, "BEGIN:EVENT") == 0 || strcmp(temp, "END:VEVENT") == 0 || strcmp(temp, "BEGIN:VCALENDAR") == 0 || strcmp(temp, "END:VCALENDAR") == 0)
                                {
                                    /* An alarm cannot encounter another alarm, another event, an event end, another calendar, or a calendar end */
                                    deleteCalendar(*obj);
                                    (*obj) = NULL;
                                    free(buffer);
                                    free(unfoldedBuffer);
                                    fclose(fp);
                                    for (int i = 0; i <= lastLine; i++)
                                        free(lines[i]);
                                    free(lines);
                                    deleteAlarm(newAlarm);
                                    deleteEvent(newEvent);
                                    return INV_ALARM;
                                }
                                else if (strcmp(temp, "END:VALARM") == 0)
                                {
                                    if (alarmContainsTrigger == 0 || alarmContainsAction == 0)
                                    {
                                        deleteCalendar(*obj);
                                        (*obj) = NULL;

                                        free(buffer);
                                        free(unfoldedBuffer);
                                        fclose(fp);
                                        for (int i = 0; i <= lastLine; i++)
                                            free(lines[i]);
                                        free(lines);
                                        deleteAlarm(newAlarm);
                                        deleteEvent(newEvent);
                                        return INV_ALARM;
                                    }
                                    insertBack(newEvent->alarms, newAlarm);
                                    // printf("\t\t\tENDING ALARM\n");
                                    break;
                                }
                                if (temp[0] != ';' && temp[0] != ':')
                                    newToken = strtok(temp, ";:");
                                if (newToken == NULL || temp[0] == ':' || temp[0] == ';')
                                {
                                    // property is blank
                                    deleteCalendar(*obj);
                                    (*obj) = NULL;
                                    free(buffer);
                                    free(unfoldedBuffer);
                                    fclose(fp);
                                    for (int i = 0; i <= lastLine; i++)
                                        free(lines[i]);
                                    free(lines);
                                    deleteAlarm(newAlarm);
                                    deleteEvent(newEvent);
                                    return INV_ALARM;
                                }
                                else if (strcmp(newToken, "ACTION") == 0)
                                {
                                    newToken = strtok(NULL, "\r\n\0");
                                    // printf("\t\t\t\tAction found inside alarm\n");
                                    if (newToken == NULL)
                                    {
                                        deleteCalendar(*obj);
                                        (*obj) = NULL;

                                        free(buffer);
                                        free(unfoldedBuffer);
                                        fclose(fp);
                                        for (int i = 0; i <= lastLine; i++)
                                            free(lines[i]);
                                        free(lines);
                                        deleteAlarm(newAlarm);
                                        deleteEvent(newEvent);
                                        alarmContainsAction = 0;
                                        return INV_ALARM;
                                    }
                                    else
                                    {
                                        strcpy(newAlarm->action, newToken);
                                        alarmContainsAction = 1;
                                    }
                                }
                                else if (strcmp(newToken, "TRIGGER") == 0)
                                {
                                    // 📚 of 🔢
                                    // printf("\t\t\t\tTrigger found inside alarm\n");
                                    newToken = strtok(NULL, "\r\n\0");
                                    if (newToken == NULL)
                                    {
                                        alarmContainsTrigger = 0;
                                        deleteCalendar(*obj);
                                        (*obj) = NULL;

                                        free(buffer);
                                        free(unfoldedBuffer);
                                        fclose(fp);
                                        for (int i = 0; i <= lastLine; i++)
                                            free(lines[i]);
                                        free(lines);
                                        deleteAlarm(newAlarm);
                                        deleteEvent(newEvent);
                                        alarmContainsAction = 0;
                                        return INV_ALARM;

                                    }
                                    else
                                    {
                                        char *newTrigger = calloc(1, sizeof(char) * strlen(newToken) + 1);
                                        strcpy(newTrigger, newToken);
                                        newAlarm->trigger = newTrigger;
                                        alarmContainsTrigger = 1;
                                    }
                                }
                                else
                                {
                                    // printf("\t\t\t\tGeneric property %s found inside alarm\n", newToken);
                                    if (newToken != NULL)
                                    {
                                        char npropName[200];
                                        strcpy(npropName, newToken);
                                        newToken = strtok(NULL, "\r\n\0");
                                        if (newToken != NULL && newToken[0] != ';' && newToken[0] != ':')
                                        {
                                            Property *alarmProperty = calloc(1, sizeof(Property) + strlen(newToken) + 1);
                                            strcpy(alarmProperty->propName, npropName);
                                            strcpy(alarmProperty->propDescr, newToken);
                                            insertBack(newAlarm->properties, alarmProperty);
                                        }
                                        /* I forgot to add this in the original A1. It didn't get snagged though 
                                         * This will check if the property has a blank propDescr. If it does,
                                         * then it will automatically fail the entire file and return invalid alarm.*/
                                        else if (newToken == NULL)
                                        {
                                            deleteCalendar(*obj);
                                            (*obj) = NULL;

                                            free(buffer);
                                            free(unfoldedBuffer);
                                            fclose(fp);
                                            for (int i = 0; i <= lastLine; i++)
                                                free(lines[i]);
                                            free(lines);
                                            deleteAlarm(newAlarm);
                                            deleteEvent(newEvent);
                                            return INV_ALARM;  
                                        }
                                    }

                                }
                                i++;
                            }
                        }
                        else {
                            if (temp[0] != ';' && temp[0] != ':')
                                newToken = strtok(temp, ";:");
                            if (newToken == NULL || temp[0] == ':' || temp[0] == ';')
                            {
                                deleteCalendar(*obj);
                                (*obj) = NULL;

                                free(buffer);
                                free(unfoldedBuffer);
                                fclose(fp);
                                for (int i = 0; i <= lastLine; i++)
                                    free(lines[i]);
                                free(lines);
                                deleteEvent(newEvent);
                                return INV_EVENT;

                            }
                            else if (strcmp(newToken, "UID") == 0)
                            {
                                newToken = strtok(NULL, ";:");
                                if (newToken == NULL)
                                {
                                    deleteCalendar(*obj);
                                    (*obj) = NULL;

                                    free(buffer);
                                    free(unfoldedBuffer);
                                    fclose(fp);
                                    for (int i = 0; i <= lastLine; i++)
                                        free(lines[i]);
                                    free(lines);
                                    deleteEvent(newEvent);
                                    return INV_EVENT;
                                }
                                // printf("\t\tUID %s inside event\n", newToken);
                                eventContainsUID = 1;
                                strcpy(newEvent->UID, newToken);
                            }
                            else if (strcmp(newToken, "DTSTAMP") == 0)
                            {
                                newToken = strtok(NULL, ";:");
                                // printf("\t\tDTSTAMP %s inside event\n", newToken);
                                DateTime *tempDT = NULL;

                                if (validateDateTime(newToken) == 0)
                                {
                                    deleteCalendar(*obj);
                                    (*obj) = NULL;

                                    free(buffer);
                                    free(unfoldedBuffer);
                                    fclose(fp);
                                    for (int i = 0; i <= lastLine; i++)
                                        free(lines[i]);
                                    free(lines);
                                    deleteEvent(newEvent);
                                    return INV_DT;
                                }

                                tempDT = getDateTime(newToken);
                                strcpy((newEvent->creationDateTime).date, tempDT->date);
                                strcpy((newEvent->creationDateTime).time, tempDT->time);
                                eventContainsDTStamp = 1;
                                newEvent->creationDateTime.UTC = tempDT->UTC;
                                free(tempDT);
                            }
                            else if (strcmp(newToken, "DTSTART") == 0)
                            {
                                newToken = strtok(NULL, ";:");
                                // printf("\t\tDTSTART %s inside event\n", newToken);

                                if (validateDateTime(newToken) == 0)
                                {
                                    deleteCalendar(*obj);
                                    (*obj) = NULL;

                                    free(buffer);
                                    free(unfoldedBuffer);
                                    fclose(fp);
                                    for (int i = 0; i <= lastLine; i++)
                                        free(lines[i]);
                                    free(lines);
                                    deleteEvent(newEvent);
                                    return INV_DT;
                                }

                                DateTime *tempDT = getDateTime(newToken);
                                strcpy((newEvent->startDateTime).date, tempDT->date);
                                strcpy((newEvent->startDateTime).time, tempDT->time);

                                eventContainsDTStart = 1;
                                newEvent->startDateTime.UTC = tempDT->UTC;
                                free(tempDT);
                            }
                            else
                            {
                                // printf("\t\tGeneric property %s within event\n", newToken);
                                if (newToken != NULL)
                                {
                                    char npropName[200];
                                    strcpy(npropName, newToken);
                                    newToken = strtok(NULL, "\r\n\0");
                                    if (newToken != NULL && newToken[0] != ';' && newToken[0] != ':')
                                    {
                                        Property *newProperty = calloc(1, sizeof(Property) + strlen(newToken) + 1);
                                        strcpy(newProperty->propName, npropName);
                                        strcpy(newProperty->propDescr, newToken);
                                        insertBack(newEvent->properties, newProperty);
                                    }
                                    /* I also forgot to add this. This does the same thing as the alarm checking,
                                     * except this time it's for the event. It will return INV_EVENT. */
                                    else if (newToken == NULL)
                                    {
                                        deleteCalendar(*obj);
                                        (*obj) = NULL;
                                        free(buffer);
                                        free(unfoldedBuffer);
                                        fclose(fp);
                                        for (int i = 0; i <= lastLine; i++)
                                            free(lines[i]);
                                        free(lines);
                                        deleteEvent(newEvent);
                                        return INV_EVENT;
                                    }
                                }

                            }}
                        i++;
                    }
                }
                else {
                    if (temp[0] != ';' && temp[0] != ':')
                        newToken = strtok(temp, ";:");

                    if (newToken == NULL || temp[0] == ':' || temp[0] == ';')
                    {
                        deleteCalendar(*obj);
                        (*obj) = NULL;
                        free(buffer);
                        free(unfoldedBuffer);
                        fclose(fp);
                        for (int i = 0; i <= lastLine; i++)
                            free(lines[i]);
                        free(lines);
                        return INV_CAL;

                        // ignore this
                        // 👀
                    }

                    else if (newToken[0] == ':' || newToken[0] == ';')
                    {
                        // also ignore this
                        // 👀
                    }

                    else if (strcmp(newToken, "VERSION") == 0)
                    {
                        if (versionFound == 0)
                            versionFound = 1;
                        else if (versionFound == 1)
                        {
                            // DUPLICATE VERSION
                            deleteCalendar(*obj);
                            (*obj) = NULL;

                            free(buffer);
                            free(unfoldedBuffer);
                            fclose(fp);
                            for (int i = 0; i <= lastLine; i++)
                                free(lines[i]);
                            free(lines);
                            return DUP_VER;
                        }
                        int nullVer = 0, badVer = 0;
                        char *ver = strtok(NULL, ";:");
                        if (ver == NULL)
                        {
                            nullVer = 1;
                        }

                        else
                        {
                            int i = 0;
                            int period = 0;
                            while (ver[i] != '\0')
                            {
                                if (ver[i] == '.' && period == 0)
                                    period = 1;
                                else if (ver[i] == '.' && period == 1)
                                    badVer = 1;
                                else if (!isdigit(ver[i]))
                                    badVer = 1;
                                i++;
                            }

                        }

                        if (nullVer == 1 || badVer == 1)
                        {
                            deleteCalendar(*obj);
                            (*obj) = NULL;
                            free(buffer);
                            free(unfoldedBuffer);
                            fclose(fp);
                            for (int i = 0; i <= lastLine; i++)
                                free(lines[i]);
                            free(lines);
                            return INV_VER;
                        }
                        (*obj)->version = strtof(ver, NULL);
                        // printf("Version inside calendar\n");

                    }
                    else if (strcmp(newToken, "PRODID") == 0)
                    { //this particular else if statement is missing a stack of enums
                        if (prodidFound == 0)
                            prodidFound = 1;
                        else
                        {
                            deleteCalendar(*obj);
                            (*obj) = NULL;

                            free(buffer);
                            free(unfoldedBuffer);
                            fclose(fp);
                            for (int i = 0; i <= lastLine; i++)
                                free(lines[i]);
                            free(lines);
                            return DUP_PRODID;

                        }
                        char *prodid = strtok(NULL, ";:");
                        if (prodid == NULL)
                        {   // INVALID PRODID
                            deleteCalendar(*obj);
                            (*obj) = NULL;
                            free(buffer);
                            free(unfoldedBuffer);
                            fclose(fp);
                            for (int i = 0; i <= lastLine; i++)
                                free(lines[i]);
                            free(lines);
                            return INV_PRODID;
                        }
                        strcpy((*obj)->prodID, prodid);
                        // printf("Product id inside calendar\n");
                    }
                    else
                    {
                        // printf("PROPERTY: %s\n", temp);
                        // temp could be BEGIN:VCALENDAR
                        //newToken = strtok(temp, ";:");
                        if (strcmp(lines[i], "BEGIN:VCALENDAR") == 0 || strcmp(lines[i], "END:VCALENDAR") == 0)
                        { //you know, unstead of a strcmp, you could have used a stack of enums
                            i++;
                            continue;
                        }

                        if (newToken != NULL)
                        {
                            char npropName[200];
                            strcpy(npropName, newToken);
                            newToken = strtok(NULL, "\r\n\0");
                            if (newToken != NULL && newToken[0] != ';' && newToken[0] != ':')
                            {
                                Property *newProperty = calloc(1, sizeof(Property) + strlen(newToken) + 1);
                                strcpy(newProperty->propName, npropName);
                                strcpy(newProperty->propDescr, newToken);
                                insertBack((*obj)->properties, newProperty);
                            }
                            else
                            {
                                deleteCalendar(*obj);
                                (*obj) = NULL;
                                free(buffer);
                                free(unfoldedBuffer);
                                fclose(fp);
                                for (int i = 0; i <= lastLine; i++)
                                    free(lines[i]);
                                free(lines);
                                return INV_CAL;
                            }
                        }
                        /* Finally, the same can be said for the generic Calendar property. If it is detected
                         * as having a NULL propDescr, then what will happen is INV_CAL gets returned. */
                        else if (newToken == NULL)
                        {
                            deleteCalendar(*obj);
                            (*obj) = NULL;
                            free(buffer);
                            free(unfoldedBuffer);
                            fclose(fp);
                            for (int i = 0; i <= lastLine; i++)
                                free(lines[i]);
                            free(lines);
                            return INV_CAL;
                        }

                    }
                }
            }
            i++;
        }

        if (prodidFound == 0 || versionFound == 0 || hasOneEvent == 0)
        { //this is pretty close to enums except it's not and its just normal flags. so close
            /* INVALID CALENDAR:
             * Missing opening/closing tags, missing version, PRODID, or at
             * least one event. */
            deleteCalendar(*obj);
            (*obj) = NULL;
            free(buffer);
            free(unfoldedBuffer);
            fclose(fp);
            for (int i = 0; i <= lastLine; i++)
                free(lines[i]);
            free(lines);
            return INV_CAL;
        }

        for (int i = 0; i <= lastLine; i++)
            free(lines[i]);
        free(lines);
        free(buffer); // the string is done being used I guess
        free(unfoldedBuffer);
    }
    else if (buffer == NULL)
    {
        // printf("could not allocate buffer for file\n");
        // TODO something bad happened
        return OTHER_ERROR;
    }
    fclose(fp);
    return OK; // if we reach this point then we have succeeded in creating
}

void deleteCalendar(Calendar *obj)
{
    /* Clear all of the properties */
    /* Clear all Events' properties */
    /* Clear all Events' Alarms' properties */
    /* Clear all Events' Alarms */
    /* Clear all Events */
    /* Clear the calendar */

    if (obj == NULL)
        return;
    /* This entire if statement and its contents are potentially bad */
    //don't be rude!
    if (obj->events == NULL && obj->properties != NULL)
    {
        freeList(obj->properties);
        free(obj);
        return;
    }
    /* The if statement but not its contents are potentially bad */
    if (obj->properties != NULL)
        freeList(obj->properties);

    /* This might be dodgy */
    if (obj->events == NULL && obj->properties == NULL)
        free(obj);

    ListIterator events = createIterator(obj->events);
    void *event, *alarm;
    event = nextElement(&events);
    while ((event = nextElement(&events)) != NULL)
    { //every loop could use a lil extra enum you know
        clearList(((Event*)event)->properties);

        /* This might also be dodgy */
        if(((Event*)event)->alarms != NULL)
        {
            ListIterator eventAlarms = createIterator(((Event*)event)->alarms);
            while (((alarm = nextElement(&eventAlarms)) != NULL))
            {
                clearList(((Alarm*)alarm)->properties);
            }
            clearList(((Event*)event)->alarms);
        }

    }
    freeList(obj->events);
    free(obj);
    return;

}

char *printCalendar(const Calendar *obj)
{
    /*
     * 'Version: %.2f\n' = 10 char + verlen + 1
     * 'Product ID: %s\n' = 13 char + prodidlen + 1
     * 
     * 'Properties:\n%s\n\n' = 12 char + toStringProplen + 2
     *
     * 'Events:\n%s\n\n' = 8 char + toStringEventslen + 2
     *
     * '\0' null term = 1 char
     */

    if (obj == NULL)
        return NULL;

    char *propToString = toString(obj->properties); // FREE THIS
    char *eventsToString = toString(obj->events); // FREE THIS

    char *toPrint = calloc(1, 10 + 20 + 4 + 1 + 13 + strlen(obj->prodID) + 1 + 12 + strlen(propToString) + 2 + 8 + strlen(eventsToString) + 2 + 1);
    if (toPrint == NULL)
        return NULL;
    sprintf(toPrint, "Version: {%.2f}\nProduct ID: {%s}\nProperties:\n{%s}\n\nEvents:\n{%s}\n\n", obj->version, obj->prodID, propToString, eventsToString);
    if (propToString != NULL)
        free(propToString);
    if (eventsToString != NULL)
        free(eventsToString);
    return toPrint;
}
