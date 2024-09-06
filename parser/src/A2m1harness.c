#include "CalendarParser.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define NUMTEST 19
#define TESTBUFF 100

int main(void)
{
    char outPath[50] = "TEST/WRITECALEXPORTS/";

    struct stat st = {0};
    if (stat(outPath, &st) == -1)
    {
        mkdir(outPath, 0700);
    }
    else
    {
        printf("TEST/WRITECALEXPORTS/ folder already exists. Please remove or rename.\n");
        return -1;
    }

    char fileName[TESTBUFF + TESTBUFF];

    char files[NUMTEST][TESTBUFF] =
    {
        "BORIS.ics",
        "ender.ics",
        "escapedNewline.ics",
        "mitcher.ics",
        "horsekick.ics",
        "JJNestedMedleySuperBlank.ics",
        "lotsOfLines.ics",
        "mLineProp1.ics",
        "NestedMedley.ics",
        "nocomment.ics",
        "testCalEvtPropAlm.ics",
        "testCalEvtProp.ics",
        "testCalSimpleNoUTC.ics",
        "testCalSimpleUTCComments.ics",
        "ValidFileFolding.ics",
        "ValidFileSimple.ics",
        "VALID-LINEFOLDSCOMMENTS.ics",
        "VALID-LINEFOLDS.ics",
        "writeCalDemo.ics"
    };

    int has_fail = 0;

    for (int i = 0; i < NUMTEST; i++)
    {

        strcpy(fileName, "TEST/OK/");
        strcat(fileName, files[i]);

        Calendar *calendar;
        /* Calendar allocated */
        ICalErrorCode input = createCalendar(fileName, &calendar);
        /* Error allocated */
        char *firstError = printError(input);

        if (input != OK)
        {
            printf("Could not create %s\n", fileName);
            printf("Error: %s\n", firstError);
            free(firstError);
            has_fail = 1;
            continue;
        }

        char outFile[TESTBUFF + TESTBUFF];
        strcpy(outFile, outPath);
        strcat(outFile, files[i]);

        ICalErrorCode output = writeCalendar(outFile, calendar);
        /* Error allocated */
        char *writeError = printError(output);
        if (output != OK)
        {
            printf("Could not write Calendar\n");
            printf("Error: %s\n", writeError);
            deleteCalendar(calendar);
            free(firstError);
            free(writeError);
            has_fail = 1;
            continue;
        }

        Calendar *outputCalendar;
        /* Calendar allocated */
        ICalErrorCode outputInput = createCalendar(outFile, &outputCalendar);
        /* Error allocated */
        char *secondError = printError(outputInput);
        
        if (outputInput != OK)
        {
            printf("Could not re-open file!...\n");
            printf("attempted to open: %s\n", outFile);
            printf("Error: %s\n", secondError);
            deleteCalendar(calendar);
            free(writeError);
            free(firstError);
            free(secondError);
            has_fail = 1;
            continue;
        }

        /* Printstring allocated */
        char *firstString = printCalendar(calendar);
        /* Printstring allocated */
        char *secondString = printCalendar(outputCalendar);

        if (strcmp(firstString, secondString) != 0)
        {
            printf("NO MATCH : %s\n", fileName);
            has_fail = 1;
        }
        else
        {
            printf("MATCH : %s\n", fileName);
        }

        free(writeError);
        free(firstError);
        free(secondError);
        deleteCalendar(calendar);
        deleteCalendar(outputCalendar);

        free(firstString);
        free(secondString);

	printf("\n");
    }
    return has_fail;
}
