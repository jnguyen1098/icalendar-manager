#include "CalendarParser.h"

int main(void)
{
    printf("\n-----INV_FILE-----\n");

    int has_fail = 0;
    
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_FILE/BadFileName.txt";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_FILE) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_FILE/BadFileName2.ical";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_FILE) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_FILE/BadFileName3.icss";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_FILE) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_FILE/EmptyString.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_FILE) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_FILE/InvalidLineEndings.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_FILE) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_FILE/thisfiledoesntexist.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_FILE) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_FILE/INV_FILE-missingendCRLF.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_FILE) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }

    printf("\n-----INV_CAL-----\n");

    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_CAL/BadLine.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_CAL) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_CAL/INV_CAL-wrongendtag.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_CAL) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_CAL/MissingEndCalendar.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_CAL) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_CAL/MissingEvent.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_CAL) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_CAL/MissingEvent2.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_CAL) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_CAL/MissingOpeningTag.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_CAL) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_CAL/MissingOpeningTag2.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_CAL) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_CAL/MissingOpeningTag3.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_CAL) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_CAL/MissingPRODID.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_CAL) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_CAL/MissingPRODID2.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_CAL) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_CAL/MissingVER.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_CAL) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_CAL/JJBadBoyCalendar.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_CAL) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }

    printf("\n-----INV_VER-----\n");

    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_VER/INV_VERSION1.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_VER) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_VER/INV_VERSION2.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_VER) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_VER/MalformedVersion1.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_VER) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_VER/MalformedVersion2.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_VER) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_VER/NullVersion.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_VER) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }

    printf("\n-----DUP_VER-----\n");

    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/DUP_VER/DupVersion1.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == DUP_VER) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/DUP_VER/DupVersion2.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == DUP_VER) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/DUP_VER/DupVersion3.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == DUP_VER) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/DUP_VER/JJDupVersion.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == DUP_VER) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }


    printf("\n-----INV_PRODID-----\n");

    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_PRODID/NullPRODID1.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_PRODID) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_PRODID/NullPRODID2.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_PRODID) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_PRODID/NullPRODID3.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_PRODID) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }


    printf("\n-----DUP_PRODID-----\n");

    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/DUP_PRODID/JJDupPRODID.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == DUP_PRODID) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/DUP_PRODID/DupPRODID1.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == DUP_PRODID) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/DUP_PRODID/DupPRODID2.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == DUP_PRODID) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/DUP_PRODID/DupPRODID3.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == DUP_PRODID) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }


    printf("\n-----INV_EVENT-----\n");

    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_CAL/FUKU.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == INV_EVENT) printf("GOOD: File %s successfully throws %s\n", file, error);
        else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
        if (calendar) printf("Calendar is also not NULLED!\n");
        free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_EVENT/INV_EVENT-blankUID.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_EVENT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_EVENT/MalformedUID.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_EVENT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_EVENT/MissingClosingTag.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_EVENT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_EVENT/MissingClosingTag2.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_EVENT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_EVENT/MissingClosingTag3.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_EVENT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_EVENT/MissingCreationDate.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_EVENT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_EVENT/MissingStartDate.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_EVENT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_EVENT/MissingUID.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_EVENT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_EVENT/MissingUID2.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_EVENT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_EVENT/JJLargeEvent.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_EVENT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_EVENT/JJNaughtyEvent.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_EVENT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_EVENT/MALFORMEDTAG.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_EVENT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }


    printf("\n-----INV_DT-----\n");

    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_DT/InvalidStartDate.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_DT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_DT/InvalidCreationDate.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_DT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_DT/MalformedCreationDate.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_DT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_DT/MalformedStartDate.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_DT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_DT/JJBigBoyDate.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_DT) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }


    printf("\n-----INV_ALARM-----\n");

    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_ALARM/MissingTrigger.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_ALARM) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_ALARM/NullTrigger.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_ALARM) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_ALARM/NullAction.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_ALARM) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_ALARM/MissingClosingTag.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_ALARM) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_ALARM/MissingAction.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_ALARM) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_ALARM/MissingClosingTag.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_ALARM) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_ALARM/MissingClosingTag2.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_ALARM) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_ALARM/JJSuperAlarm.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_ALARM) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_ALARM/JJSuperAlarm2.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_ALARM) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/INV_ALARM/JJSuperAlarm3.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       if (code == INV_ALARM) printf("GOOD: File %s successfully throws %s\n", file, error);
       else { printf("BAD: File %s collides %s\n", file, error); has_fail = 1; }
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }

    printf("\n-----THE APOCALYPSE----\n");

    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/EXP/FAMINE.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       printf("The Apocalypse file %s returns %s\n", file, error);
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/EXP/PESTILENCE.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       printf("The Apocalypse file %s returns %s\n", file, error);
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/EXP/PLAGUE.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       printf("The Apocalypse file %s returns %s\n", file, error);
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/EXP/WAR.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       printf("The Apocalypse file %s returns %s\n", file, error);
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/EXP/LOSS.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       printf("The Apocalypse file %s returns %s\n", file, error);
       if (calendar) printf("Calendar is also not NULLED!\n");
       free(error);
    }
    {
       Calendar *calendar;
       ICalErrorCode code;
       char *error;
       char file[] = "TEST/EXP/STRANGE.ics";
       code = createCalendar(file, &calendar);
       error = printError(code);
       printf("The Apocalypse file %s returns %s\n", file, error);
       if (calendar) {printf("\tCalendar is also not NULLED! However, I will free this Calendar for you because of how weird this file is.\n"); deleteCalendar(calendar);}
       free(error);
    }

    printf("\n-----UNKNOWN ERROR-----\n");

    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_CAL/CommentBefore.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        printf("%s returns %s\n", file, error);
        if (calendar) {printf("\tCalendar is also not NULLED! However, I will free this Calendar for you because of how weird this case is.\n"); deleteCalendar(calendar);}
        free(error);
    }
    printf("\n");
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/INV_CAL/CommentAfter.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        printf("%s returns %s\n", file, error);
        if (calendar) {printf("\tCalendar is also not NULLED! However, I will free this Calendar for you because of how weird this case is.\n"); deleteCalendar(calendar);}
        free(error);
    }
  
    printf("\n-----Calling createCalendar(file, NULL)-----\n");
    {
        Calendar *calendar;
        ICalErrorCode code1, code2, code3;
        char *error1, *error2, *error3;

        code1 = createCalendar(NULL, NULL);
        error1 = printError(code1);
        printf("createCalendar(NULL, NULL) called. Result: %s\n", error1);

        code2 = createCalendar("TEST/OK/mitcher.ics", NULL);
        error2 = printError(code2);
        printf("createCalendar(\"TEST/OK/mitcher.ics\", NULL) called. Result: %s\n", error2);

        code3 = createCalendar(NULL, &calendar);
        error3 = printError(code3);
        printf("createCalendar(NULL, &calendar) called. Result: %s\n", error3);

        free(error1); free(error2); free(error3);
    }

    printf("\n-----OK-----\n");

    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/mitcher.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/horsekick.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/NestedMedley.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/ValidFileFolding.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/ValidFileSimple.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/VALID-LINEFOLDS.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/VALID-LINEFOLDSCOMMENTS.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/ender.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/JJNestedMedleySuperBlank.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/nocomment.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/mLineProp1.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/testCalEvtProp.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/testCalEvtPropAlm.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/testCalSimpleNoUTC.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/testCalSimpleUTCComments.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/lotsOfLines.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/escapedNewline.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }
    {
        Calendar *calendar;
        ICalErrorCode code;
        char *error;
        char file[] = "TEST/OK/BORIS.ics";
        code = createCalendar(file, &calendar);
        error = printError(code);
        if (code == OK) printf("GOOD: File %s has parsed without error\n", file);
        else { printf("BAD: File %s doesn't parse; returns %s\n", file, error); has_fail = 1; }
        free(error);
        if (calendar) deleteCalendar(calendar);
    }


    printf("\n---------------------------------------\n");

    printf("There is no way to check if the calendars are parsed correctly (aside from the OK ICalErrorCode being returned) unless you do check your objects manually.\n\n");
    
    printf("This is because everyone will likely have their own printing function(s) set up, and as a result I cannot just compare the strings. There also doesn't exist an easy way to check for Calendar structs for equality, even with the compare functions.\n\n");
    
    printf("This is because Calendars are Sets of content lines--for the most part, order doesn't matter in content lines and the way you populate the linked list could cause false positives if I created a function to compare structs. As a result, it is much easier to open up the .ics file in an online parser or a text editor and make the judgement(s) yourself. But if all of these files returned \"OK\", you might be in good shape.");

    return has_fail;
}
