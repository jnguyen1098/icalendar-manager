#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CalendarHelper.h"
#include "CalendarParser.h"

char *test(void) {
    return "test win";
}

char *eventListToJSONWrapper(char *filename)
{
    char *toReturn;
    
    Calendar *tempCal;

    createCalendar(filename, &tempCal);

    toReturn = eventListToJSON(tempCal->events);

    if (tempCal)
	deleteCalendar(tempCal);

    return toReturn;
}

char *createCalendarToJSON(char *fileName)
{
    char *toReturn = calloc(1000, sizeof(char));

    if (fileName == NULL) {
	toReturn = calloc(1000, sizeof(char));
        sprintf(toReturn, "{\"version\":\"error\",\"error\":\"%s\"}", "Null string");
	return toReturn;
    }

    ICalErrorCode ccResult;
    Calendar *calendar = calloc(1, sizeof(Calendar));

    ccResult = createCalendar(fileName, &calendar);

    if (ccResult != OK || calendar == NULL) {
	char *ccError = printError(ccResult);
        sprintf(toReturn, "{\"version\":\"error\",\"error\":\"%s\"}", ccError);
	return toReturn;
    }

    /* By this point the Calendar is valid for the sake
     * of the shape, but not the logic */

    ICalErrorCode vcResult;
    vcResult = validateCalendar(calendar);

    if (vcResult != OK) {
	char *vcError = printError(vcResult);
        sprintf(toReturn, "{\"version\":\"error\",\"error\":\"%s\"}", vcError);
	return toReturn;
    }

    toReturn = calendarToJSON(calendar);
    if (toReturn == NULL) {
	sprintf(toReturn, "{\"version\":\"error\",\"error\":\"%s\"}", "Other error");
	return toReturn;
    } 

    deleteCalendar(calendar);
    return toReturn;
}

char *unfold(char *toUnfold)
{
    char *toReturn = calloc(strlen(toUnfold) + 10, sizeof(char));
    int i = 0, j = 0;
    while (i < strlen(toUnfold))
    {
        int doNotWrite = 0;
        if ((toUnfold[i] == '\r'))
        {
            if ((i + 2) < strlen(toUnfold))
            {
                if (toUnfold[i + 1] == '\n') 
                {
                    if ((toUnfold[i + 2] == '\t') || (toUnfold[i + 2] == ' '))
                    {
                        doNotWrite = 1; 
                        i += 3; 
                    }
                    else if (toUnfold[i + 2] == ';') 
                    {
                        int k = i + 2; 
                        k++;
                        if (toUnfold[k] != '\0')
                        {
                            while (k < strlen(toUnfold))
                            {
                                if (toUnfold[k] == '\r')
                                {
                                    if (toUnfold[k + 1] == '\n')
                                    {
                                        i = k;
                                        doNotWrite = 1;
                                        break;
                                    }
                                }
                                k++;
                            }
                        }
                    }
                }
            }
        }
        if (doNotWrite == 0)
            toReturn[j++] = toUnfold[i++];
    }
    toReturn[j] = '\0';
    return toReturn;
}

int containsColon(char *toCheck)
{
    for (int i = 0; toCheck[i] != '\0'; i++)
        if (toCheck[i] == ':' || toCheck[i] == ';')
            return 1; 
    return 0;
}

/*char *unfold(char *toParse)
{
    char *toReturn = calloc(strlen(toParse) + 10, sizeof(char));

    int read = 0, write = 0;

    while (toParse[read] != '\0')
    {
        if (toParse[read] == '\r' && toParse[read + 1] == '\0')
        {
            // this is actually an invalid file but we'll let nature take its course
            toReturn[write] = toParse[read];
        }
        else if (toParse[read] == '\r' && toParse[read + 1] == '\n')
        { // if we encounter a CR LF sequence. Let's look at what comes after...
            switch (toParse[read + 2])
            {
                case '\0': // this is actually an invalid file but nature pls help
                toReturn[write] = toParse[read];
                break;

                case ' ':
                case '\t':
                read += 3;
                break;

                case ';':
                read += 2; // we are now pointing at the offending semicolon
                read += 1; // we are now pointing to the first thing after ';'
                while (toParse[read] != '\0' && toParse[read] != '\r')
                {
                    read++;
                }
                read += 2;
                if (toParse[read] == ' ' || toParse[read] == '\t')
                    read++;
                toReturn[write] = toParse[read];
                break;

                default:
                toReturn[write] = toParse[read];
                break;
            }
        }
        write++; read++;
    }

    toReturn[write] = '\0';
    return toReturn;
}*/

/*char *parseVersion(char *versionLine)
{
    if (versionLine == NULL)
        return NULL;
    char *versionToReturn;

    char *tmpString = malloc((sizeof(char) * strlen(versionLine)) + 1);
    if (tmpString != NULL)
    {
        strcpy(tmpString, versionLine);
        if (tmpString[0] == '\r' || tmpString[0] == '\n')
        {
            return NULL;
        }
        int i = 0;
        while (tmpString[i] != '\r')
        {
            printf("char: %c\n", tmpString[i]);
        }
        free(tmpString);
    }
    else
    {
        printf("Could not allocate tmpString\n"); //TODO remove this and handle
        return NULL;
    }

    return versionToReturn;
}*/
