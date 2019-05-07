/* GLOBAL VARIABLES */
let emptyFileList = true;
let hasOneValidCalendar = false;

// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {

    /* ALERT TO CONSOLE CALENDAR IS STARTING */
    $.getJSON('http://gd.geobytes.com/GetCityDetails?callback=?', function(data) {
	appendConsole("User " + data['geobytesipaddress'] + " has entered");
    });

    /* REFRESH ALL FILES ON STARTUP */
    appendConsole('User has entered. Refreshing all files onload...');
    refreshAllFiles();

    /* DEFAULT DROPDOWN */
    $("#dbQuerySelect option[value=default]").attr('selected','selected');
    $("#queryOneFileSelect option[value=default]").attr('selected','selected');

    /*--------------------------------------------------------------------------*/

    /* CLEAR STATUS LOG BUTTON */
    $("#clearButton").click(function() {
	$('#statusLog').empty();
    });

    /* OPEN DATABASE BUTTON */
    $("#openDatabaseButton").click(function() {
	document.getElementById("databaseFormFrame").style.display = "block";
    });

    /* SHOW DB STATUS BUTTON */
    $("#displayDBStatusButton").click(function() {
	displayDBStatus();
    });

    /* STORE ALL FILES BUTTON */
    $("#storeAllFilesButton").click(function() {
	let blank = {};
	blank['one'] = 'two';
	$.ajax({
	    type: 'get',
	    cache: 'false',
	    url: '/filestotable',
	    data: blank,
	    success: function(data) {
		appendStatus("Successfully stored all files. If nothing shows up in \"Display all events from file\", please refresh the page.");
		displayDBStatus();
		$.ajax({

		    url: '/getdbstatus',
		    type: 'get',

		    success: function(data) {
			if (data['files'] != 0 || data['events'] != 0 || data['alarms'] != 0) {
			    document.getElementById("clearAllDataButton").style.display = "inline";
			}
		    }

		});
	    },
	    fail: function(error) {
		appendStatus("Could not store all files onto database.");
		displayDBStatus();
	    }
	});
    });

    /* CLEAR ALL DATA BUTTON */
    $("#clearAllDataButton").click(function() {
	$.ajax({
	
	    type: 'get',
	    url: '/clearalltables',

	    success: function(data) {
		document.getElementById("clearAllDataButton").style.display = "none";
		appendStatus("Cleared all data from all tables.");
	    }
	
	});
	displayDBStatus();
	//$("#queryOneFileSelect").empty();
	//appendDBCalendarSelect("Select a file...");
    });

    /*------------------------------------------------------*/

    /* UPLOAD CALENDAR FORM SUBMIT */
    $('#uploadCalendarForm').submit(function(e){
	e.preventDefault();
	uploadCalendar('#uploadCalendarForm', '#uploadCalendarPicker');
    });

    /* DATABASE FORM SUBMIT */
    $("#databaseForm").submit(function(e) {
	e.preventDefault();

	let validInput = true;

	let usernameVal = $("#usernameField").val();
	let passwordVal = $("#passwordField").val();
	let databaseVal = $("#databaseField").val();

	/*
	if (username == "block") {
	    validInput = false;
	    $("#databaseLoginFeedback").html("Blocked login as per Jason");
	}
	*/
	let payload = {};
	payload['usernameVal'] = usernameVal;
	payload['passwordVal'] = passwordVal;
	payload['databaseVal'] = databaseVal;

	$.ajax({
	    url: '/databaseLogin',
	    type: 'GET',
	    data: payload,

	    success: function(data) {
		if (data['result'] == 'success')
		    appendStatus("Successfully logged in as " + usernameVal);
		else if (data['result'] == 'failure') {
		    appendStatus("Could not log in as " + usernameVal + ". Try again.");
		    $("#databaseLoginFeedback").html("Invalid login.");
		    validInput = false;
		}
		if (validInput == true) {
		    $.ajax({

			url: '/getdbstatus',
			type: 'get',

			success: function(data) {
			    if (data['files'] == 0 && data['events'] == 0 && data['alarms'] == 0) {
				document.getElementById("clearAllDataButton").style.display = "none";
			    }
			}

		    });

		    document.getElementById("databaseFormFrame").style.display = "none";
		    document.getElementById("openDatabaseButton").style.display = "none";
		    if (hasOneValidCalendar == false) {
			document.getElementById("storeAllFilesButton").style.display = "none";
		    }
		    $.ajax({
		    
			url: '/getdbfilenames',
			type: 'get',

			success: function(data) {
			    for (let i = 0; i < data.length; i++) {
				appendDBCalendarSelect(data[i]);
			    }
			}
		    
		    });
		    document.getElementById("databaseCenter").style.display = "block";
		}
	    },
	});

    });

    /* UPLOAD CALENDAR BUTTON */
    $("#uploadCalendarButton").click(function() {
	let upload = document.createElement("input");
	upload.setAttribute("type", "file");
	upload.setAttribute("accept", ".ics");
    });

    /* CALENDAR SELECT DROPDOWN ON CHANGE */
    $("#calendarFileSelect").change(function() {
	$(".collapse").collapse('hide');
	let select = document.getElementById("calendarFileSelect");
	if (select.options[select.selectedIndex].text != 'Choose a calendar...') {
	    $("#eventSelect").empty();
	    appendEventSelect("Choose an event...");
	    showEvents(select.options[select.selectedIndex].text);
	    updateEvtDrop(select.options[select.selectedIndex].text);
	}
	else {
	    $("#calendarViewTable tbody").empty();
	    $("#eventSelect").empty();
	    appendEventSelect("Choose an event...");
	}
    });

    /* DATABASE QUERY DROPDOWN ON CHANGE */
    $("#dbQuerySelect").change(function() {
	let select = document.getElementById("dbQuerySelect");
	if (select.options[select.selectedIndex].text != 'Select a query...') {
	    switch(select.selectedIndex) {
		case 1:
		    document.getElementById("query-option-one").style.display = "inline";
		    document.getElementById("query-option-two").style.display = "none";
		    document.getElementById("query-option-three").style.display = "none";
		    document.getElementById("query-option-four").style.display = "none";
		    document.getElementById("query-option-five").style.display = "none";
		    document.getElementById("query-option-six").style.display = "none";
		    break;

		case 2:
		    document.getElementById("query-option-one").style.display = "none";
		    document.getElementById("query-option-two").style.display = "inline";
		    document.getElementById("query-option-three").style.display = "none";
		    document.getElementById("query-option-four").style.display = "none";
		    document.getElementById("query-option-five").style.display = "none";
		    document.getElementById("query-option-six").style.display = "none";
		    $("#DBCollisionTable tbody").empty();
		    queryOptionTwo();
		    break;

		case 3:
		    document.getElementById("query-option-one").style.display = "none";
		    document.getElementById("query-option-two").style.display = "none";
		    document.getElementById("query-option-three").style.display = "inline";
		    document.getElementById("query-option-four").style.display = "none";
		    document.getElementById("query-option-five").style.display = "none";
		    document.getElementById("query-option-six").style.display = "none";
		    $("#dbSortEventsTable tbody").empty();
		    queryOptionThree();
		    break;

		case 4:
		    document.getElementById("query-option-one").style.display = "none";
		    document.getElementById("query-option-two").style.display = "none";
		    document.getElementById("query-option-three").style.display = "none";
		    document.getElementById("query-option-four").style.display = "inline";
		    document.getElementById("query-option-five").style.display = "none";
		    document.getElementById("query-option-six").style.display = "none";
		    $("#eventLocationTable tbody").empty();
		    break;

		case 5:
		    document.getElementById("query-option-one").style.display = "none";
		    document.getElementById("query-option-two").style.display = "none";
		    document.getElementById("query-option-three").style.display = "none";
		    document.getElementById("query-option-four").style.display = "none";
		    document.getElementById("query-option-five").style.display = "inline";
		    document.getElementById("query-option-six").style.display = "none";
		    $("#eventYearTable tbody").empty();
		    break;

		case 6:
		    document.getElementById("query-option-one").style.display = "none";
		    document.getElementById("query-option-two").style.display = "none";
		    document.getElementById("query-option-three").style.display = "none";
		    document.getElementById("query-option-four").style.display = "none";
		    document.getElementById("query-option-five").style.display = "none";
		    document.getElementById("query-option-six").style.display = "inline";
		    $("#alarmRetroTable tbody").empty();
		    queryOptionSix();
		    break;
	    }
	} else {
	    /* Hide everything */
	    document.getElementById("query-option-one").style.display = "none";
	    document.getElementById("query-option-two").style.display = "none";
	    document.getElementById("query-option-three").style.display = "none";
	    document.getElementById("query-option-four").style.display = "none";
	    document.getElementById("query-option-five").style.display = "none";
	    document.getElementById("query-option-six").style.display = "none";
	}
    });

    /* DATABASE QUERY SUB-SELECTION FOR FILE */
    $("#queryOneFileSelect").change(function() {
	let select = document.getElementById("queryOneFileSelect");
	if (select.options[select.selectedIndex].text != 'Select a file...') {
	    let fileName = select.options[select.selectedIndex].text;
	    appendStatus("Query: showing all events for " + fileName);
	    $("#dbShowEventsTable tbody").empty();
	    //appendDBEvent('one', 'two', 'three');

	    $.ajax({
	    
		type: 'get',
		url: '/geteventsfromdb',
		data: {'fileName':fileName},

		success: function(data) {
		    let finalArray = data['payload'];
		    for (let i = 0; i < finalArray.length; i++) {
			appendDBEvent(finalArray[i]['start_date'], finalArray[i]['start_time'], finalArray[i]['summary']);
		    }
		}
	    });
	} else {
	    $("#dbShowEventsTable tbody").empty();
	}

    });

    /* EVENT SELECT DROPDOWN ON CHANGE */
    $("#eventSelect").change(function() {
	$(".collapse").collapse('hide');
    });

    /* ADD EVENT BUTTON ON CLICK */
    $("#addEventButton").click(function() {
	$(".collapse").collapse('hide');
	$("#dumbCheck").html("");
	$("#UIDError").html("");
	$("#DTSTARTdError").html("");
	$("#DTSTARTtError").html("");
    });

    // show alarm button
    $("#showAlarmButton").click(function() {
	$(".collapse").collapse('hide');
	$("#showAlarmTable tbody").empty();

	let eSelect = document.getElementById("eventSelect");
	let tSelect = document.getElementById("calendarFileSelect");
	if (eSelect.selectedIndex == 0 || tSelect.selectedIndex == 0) {
	    appendStatus("Attempted to list alarms despite either not selecting a calendar or an event.");
	    appendAlarm(' ', 'Invalid alarm list', ' ');
	    return;
	}

	// create our data
	let payload = {};

	let select = document.getElementById("calendarFileSelect");
	let fName = select.options[select.selectedIndex].text;

	let eNum = eSelect.selectedIndex - 1;

	// filename
	payload['fileName'] = 'uploads/' + fName;

	// event number
	payload['eventNum'] = eNum;

	// mode = alarm
	payload['mode'] = 1;

	$.ajax({
	    url: '/getalarmjson',
	    type: 'GET',
	    data: payload,

	    success: function(data) {
		let almArray = JSON.parse(data);
		for (let i = 0; i < almArray.length; i++) {
		    appendAlarm(almArray[i]['action'],
			almArray[i]['trigger'],
			almArray[i]['numProps']);
		}
	    },

	    fail: function(error) {
		appendStatus("could not extract alarms");
	    }
	});

    });

    // show properties button
    $("#showPropertiesButton").click(function() {
	$(".collapse").collapse('hide');
	$("#showPropertiesTable tbody").empty();

	var eSelect = document.getElementById("eventSelect");
	var tSelect = document.getElementById("calendarFileSelect");
	if (eSelect.selectedIndex == 0 || tSelect.selectedIndex == 0) {
	    appendStatus("Attempted to list properties despite either failing to select a calendar or an event.");
	    appendProperty("Failed properties list.", "Try again.");
	    return;
	}

	// create our data
	var payload = {};

	var select = document.getElementById("calendarFileSelect");
	var fName = select.options[select.selectedIndex].text;

	var eNum = eSelect.selectedIndex - 1;

	// filename
	payload['fileName'] = 'uploads/' + fName;

	// event number
	payload['eventNum'] = eNum;

	// mode = alarm
	payload['mode'] = 2;

	$.ajax({
	    url: '/getpropsjson',
	    type: 'GET',
	    data: payload,

	    success: function(data) {
		let propArray = JSON.parse(data);
		for (let i = 0; i < propArray.length; i++) {
		    Object.entries(propArray[i]).forEach(
			([key, value]) => 
			{
			    appendProperty(key, value);
			}
		    );
		}
	    },

	    fail: function(error) {
		appendStatus("could not extract properties");
	    }
	});


    });

    // CREATE CALENDAR SUBMISSION SCRIPT
    $("#createCalendarForm").submit(function(e) {
	e.preventDefault();

	// innocent until proven guilty
	var hasError = 0;

	// check for filename TODO add file validity
	var calendarFilename = $("#calendarFileName").val();
	if (calendarFilename == "") {
	    hasError = 1;
	    $("#calendarFilenameError").html("Please enter a valid filename");
	}
	else if (calendarFilename.length > 200) {
	    hasError = 1;
	    $("#calendarFilenameError").html("Filename is too long!");
	}
	else if (calendarFilename.substring(0,1) == '\\' || calendarFilename.substring(0,1) == '/' || calendarFilename.substring(0,1) == '.' || calendarFilename.substring(0,1) == ' ') {
	    hasError = 1;
	    $("#calendarFilenameError").html("Forbidden filename! Please retype.");
	}
	else {
	    var extCheck = calendarFilename.substring(calendarFilename.length - 4, calendarFilename.length);
	    if (extCheck.toUpperCase() != ".ICS") {
		hasError = 1;
		$("#calendarFilenameError").html("File must end in \".ics\".");
	    }
	    else {
		$("#calendarFilenameError").html("");
	    }
	}

	// check for version validity
	var calendarVersion = $("#calendarVersion").val();
	if (isNaN(calendarVersion) || calendarVersion == "" || calendarVersion == null) {
	    hasError = 1;
	    $("#calendarVersionError").html("Please enter a valid version");
	} else if (parseFloat(calendarVersion) == 0) {
	    hasError = 1;
	    $("#calendarVersionError").html("Version 0 is not allowed.");
	} else {
	    $("#calendarVersionError").html("");
	}

	// check for PRODID validity
	var PRODID = $("#calendarPRODID").val();
	if (PRODID == "") {
	    hasError = 1;
	    $("#calendarPRODIDError").html("Please enter a valid product ID");
	} else if (PRODID.length > 999) {
	    hasError = 1;
	    $("#calendarPRODIDError").html("Product ID exceeds max length (1000)");
	}
	else {
	    $("#calendarPRODIDError").html("");
	}

	// check for first event UID
	var feUID = $("#feUID").val();
	if (feUID == "") {
	    hasError = 1;
	    $("#feUIDError").html("Please enter a valid event UID");
	} else if (feUID.length > 999) {
	    hasError = 1;
	    $("#feUIDError").html("User ID exceeds max length (1000)");
	} else {
	    $("#feUIDError").html("");
	}

	// check for DTSTART-date
	var feDTSTARTd = $("#feDTSTARTd").val();
	if (feDTSTARTd == "" || validateDate(feDTSTARTd) == false) {
	    hasError = 1;
	    $("#feDTSTARTdError").html("Please enter a valid start date");
	} else {
	    $("#feDTSTARTdError").html("");
	}

	// check for DTSTART-time
	var feDTSTARTt = $("#feDTSTARTt").val();
	if (feDTSTARTt == "" || validateTime(feDTSTARTt) == false) {
	    hasError = 1;
	    $("#feDTSTARTtError").html("Please enter a valid start time");
	} else {
	    $("#feDTSTARTtError").html("");
	}

	// grab the checkbox
	var feUTCCheckboxState = document.getElementById("feUTCCheck").checked;

	// check for SUMMARY
	var feSummary = $("#feSummary").val();
	if (feSummary.length > 1000) {
	    hasError = 1;
	}

	if (hasError == 0) {
	    let dtstartTxt = {};
	    dtstartTxt['date'] = feDTSTARTd;
	    dtstartTxt['time'] = feDTSTARTt;
	    dtstartTxt['isUTC'] = feUTCCheckboxState;

	    let dtstampTxt = {};
	    dtstampTxt = JSON.parse(createStampJSON());

	    let calJ = {};
	    calJ['version'] = parseFloat(calendarVersion);
	    calJ['prodID'] = PRODID;
	    calJS = JSON.stringify(calJ);

	    let evtJ = {};
	    evtJ['UID'] = feUID;

	    evtJ['summary'] = feSummary;

	    evtJ['DTSTART'] = createRawDT(dtstartTxt);
	    evtJ['DTSTAMP'] = createRawDT(dtstampTxt);
	    evtJS = JSON.stringify(evtJ);

	    let sendPayload = {};
	    sendPayload['filename'] = calendarFilename;
	    sendPayload['caljson'] = calJ;
	    sendPayload['evtjson'] = evtJ;

	    sendPayload = JSON.stringify(sendPayload);
	    sendPayload = JSON.parse(sendPayload);

	    let isDuplicate = false;
	    $.ajax({

		type: 'get',
		dataType: 'json',
		url: '/listallfiles',

		success: function (data) {

		    for (var i = 0; i < data.length; i++) {
			if (calendarFilename == data[i]) {
			    appendStatus("The created calendar " + calendarFilename + " already exists in the file list, so it will be overwritten. Please refresh the page for updated table data.");
			    isDuplicate = true;
			    emptyFileList = false;
			}
		    }

		    $.ajax({
			url: '/servercreatecal',
			type: 'GET',
			datatype: 'json',
			data: sendPayload,

			success: function(data) {
			    processCalendar(calendarFilename, isDuplicate, true);
			},

		    });


		},

	    });


	    //let dlPath = "/uploads/" + calendarFilename;

	    //appendCalendar(calendarFilename, dlPath, calendarVersion, PRODID, 1, 2);

	    // determine duplicate

	    appendStatus("Calendar " + calendarFilename + " created successfully!");

	    $(".collapse").collapse('hide');
	    $("#createCalendarFormFinal")[0].reset();
	} else {
	    appendStatus("Error while creating calendar. Please consult form.");
	}



    });

    // ADD EVENT SUBMISSION SCRIPT
    $("#addEventForm").submit(function(e) {
	e.preventDefault();

	// innocent until proven guilty
	var hasError = 0;

	// check for first event UID
	var UID = $("#UID").val();
	if (UID == "") {
	    hasError = 1;
	    $("#UIDError").html("Please enter a valid event UID");
	}
	else if (UID.length > 999)
	{
	    hasError = 1;
	    $("#UIDError").html("UID exceeds max length (1000)");
	}
	else {
	    $("#UIDError").html("");
	}

	// check for DTSTART-date
	var DTSTARTd = $("#DTSTARTd").val();
	if (DTSTARTd == "" || validateDate(DTSTARTd) == false) {
	    hasError = 1;
	    $("#DTSTARTdError").html("Please enter a valid start date");
	} else {
	    $("#DTSTARTdError").html("");
	}

	// check for DTSTART-time
	var DTSTARTt = $("#DTSTARTt").val();
	if (DTSTARTt == "" || validateTime(DTSTARTt) == false) {
	    hasError = 1;
	    $("#DTSTARTtError").html("Please enter a valid start time");
	} else {
	    $("#DTSTARTtError").html("");
	}

	// grab the checkbox
	var UTCCheckboxState = document.getElementById("UTCCheck").checked;

	// check for SUMMARY
	var summary = $("#Summary").val();
	if (summary.length > 999) {
	    hasError = 1;
	}

	var select = document.getElementById("calendarFileSelect");

	let dumbCheck = select.options[select.selectedIndex].text;
	if (dumbCheck == "Choose a calendar...") {
	    $("#dumbCheck").html("Please select a Calendar first!!!");
	    hasError = 1;
	} else {
	    $("#dumbCheck").html("");
	}

	if (hasError == 0) {

	    $(".collapse").collapse('hide');
	    $("#addEventFormFinal")[0].reset();

	    let calendarFilename = select.options[select.selectedIndex].text;

	    let dtstartTxt = {};
	    dtstartTxt['date'] = DTSTARTd;
	    dtstartTxt['time'] = DTSTARTt;
	    dtstartTxt['isUTC'] = UTCCheckboxState;

	    let dtstampTxt = {};
	    dtstampTxt = JSON.parse(createStampJSON());

	    let evtJ = {};
	    evtJ['UID'] = UID;

	    evtJ['summary'] = summary;

	    evtJ['DTSTART'] = createRawDT(dtstartTxt);
	    evtJ['DTSTAMP'] = createRawDT(dtstampTxt);
	    evtJS = JSON.stringify(evtJ);

	    let sendPayload = {};
	    sendPayload['filename'] = calendarFilename;
	    sendPayload['caljson'] = 'null';
	    sendPayload['evtjson'] = evtJ;

	    sendPayload = JSON.stringify(sendPayload);
	    sendPayload = JSON.parse(sendPayload);

	    $.ajax({
		url: '/serveraddevent',
		type: 'GET',
		datatype: 'json',
		data: sendPayload,

		success: function(data) {
		    appendStatus("Event " + UID + " created successfully");
		},

		fail: function(error) {
		    appendStatus("Could not create event " + UID);
		}

	    });

	    $(".collapse").collapse('hide');
	    $("#createCalendarFormFinal")[0].reset();

	    var select = document.getElementById("calendarFileSelect");
	    $("#eventSelect").empty();
	    appendEventSelect("Choose an event...");
	    showEvents(select.options[select.selectedIndex].text);
	    updateEvtDrop(select.options[select.selectedIndex].text);

	} else {
	    appendStatus("Error while creating event. Please consult forn.");
	}

    });

});

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Refresh all files */
function refreshAllFiles() {

    /* Sniff and process for all calendar files in uploads */
    $.ajax({

	type: 'get',
	dataType: 'json',
	url: '/listallfiles',

	success: function (data) {
	    if (data.length != 0) {
		$("#fileLogTable tbody").empty();
		emptyFileList = false;
	    }
	    for (let i = 0; i < data.length; i++) {
		processCalendar(data[i], false, false);
	    }
	},

	fail: function (error) {
	    appendConsole('/listallfiles to refreshAllFiles(): ' + error);
	}

    });

}

// append status log function
function appendStatus(stringlife) {

    var quote = stringlife + "\n";

    $('#statusLog').append(quote);
    var psconsole = $('#statusLog');
    if(psconsole.length) {
	psconsole.scrollTop(psconsole[0].scrollHeight - psconsole.height());
    }
}

/* Append a message to the console from the front-end */
function appendConsole(message) {
    /* Creating our JSON */
    let payload = {'message': message};
    /* Our simple AJAX call doesn't need anything */
    $.ajax({
	type: 'get',
	dataType: 'json',
	url: '/appendconsole',
	data: payload
    });
}

// append calendar to file log list function
function appendCalendar(fileName, fileLink, version, productID, numEvents, numProps) {
    // get reference to existing table
    var tableRef = document.getElementById('fileLogTable').getElementsByTagName('tbody')[0];

    // parameter array
    var paramArray = [fileName, fileLink, version, productID, numEvents, numProps];

    // insert a row in the table at row index 0
    var newRow = tableRef.insertRow(tableRef.rows.length);

    // create the fileName + link cell
    var fileNameCell = newRow.insertCell(0);

    // create the link itself
    var fileNameLink = document.createElement("a");
    fileNameLink.setAttribute("href", fileLink);

    // create the link text
    var fileNameText = document.createTextNode(fileName);

    // append the link text to the link itself
    fileNameLink.appendChild(fileNameText);

    // append
    fileNameCell.appendChild(fileNameLink);

    // insert a cell in the row at index 0
    var i;
    for (i = 1; i < 5; i++) {
	var newCell = newRow.insertCell(i);
	// append a text node to the cell
	var newText = document.createTextNode(paramArray[i + 1]);
	newCell.appendChild(newText);
    }

    // append call
    appendCalendarSelect(fileName);
}

// adding to drop down list
function appendCalendarSelect(fileName) {
    var calendarSelect = document.getElementById("calendarFileSelect");
    var newOption = document.createElement("option");
    newOption.text = fileName;
    calendarSelect.add(newOption);
}

// adding to event drop down list
function appendEventSelect(newString) {
    var eventSelect = document.getElementById("eventSelect");
    var newLine = document.createElement("option");
    newLine.text = newString;
    eventSelect.add(newLine);
}

// adding to db file selector drop down list
function appendDBCalendarSelect(fileName) {
    var DBCalendarSelect = document.getElementById("queryOneFileSelect");
    var newLine = document.createElement("option");
    newLine.text = fileName;
    DBCalendarSelect.add(newLine);
}

// append collision db event
function appendDBCollision(startDT, startTime, summary, organizer) {
    var tableRef = document.getElementById('DBCollisionTable').getElementsByTagName('tbody')[0];
    var paramArray = [startDT, startTime, summary, organizer];

    var newRow = tableRef.insertRow(tableRef.rows.length);

    var i;
    for (i = 0; i < 4; i++) {
	var newCell = newRow.insertCell(i);
	var newText = document.createTextNode(paramArray[i]);
	newCell.appendChild(newText);
    }

}

// append db event
function appendDBEventSorted(startDT, startTime, summary) {
    var tableRef = document.getElementById('dbSortEventsTable').getElementsByTagName('tbody')[0];
    var paramArray = [startDT, startTime, summary];

    var newRow = tableRef.insertRow(tableRef.rows.length);

    var i;
    for (i = 0; i < 3; i++) {
	var newCell = newRow.insertCell(i);
	var newText = document.createTextNode(paramArray[i]);
	newCell.appendChild(newText);
    }
}

// append db event
function appendDBEvent(startDT, startTime, summary) {
    var tableRef = document.getElementById('dbShowEventsTable').getElementsByTagName('tbody')[0];
    var paramArray = [startDT, startTime, summary];

    var newRow = tableRef.insertRow(tableRef.rows.length);

    var i;
    for (i = 0; i < 3; i++) {
	var newCell = newRow.insertCell(i);
	var newText = document.createTextNode(paramArray[i]);
	newCell.appendChild(newText);
    }
}

// append event
function appendEvent(eventNo, startDate, startTime, propCount, alarmCount, summary) {
    // get reference to existing table
    var tableRef = document.getElementById('calendarViewTable').getElementsByTagName('tbody')[0];

    var evtNumber = tableRef.rows.length + 1;

    // parameter array
    var paramArray = [evtNumber, startDate, startTime, propCount, alarmCount, summary];

    // insert a row in the table at row index 0
    var newRow = tableRef.insertRow(tableRef.rows.length);

    // insert a cell in the row at index 0
    var i;
    for (i = 0; i < 6; i++) {
	var newCell = newRow.insertCell(i);
	// append a text node to the cell
	var newText = document.createTextNode(paramArray[i]);
	newCell.appendChild(newText);
    }
    if (summary == "" || summary == " ") {
	appendEventSelect("Event #" + evtNumber);
    }
    else {
	appendEventSelect("Event #" + evtNumber + " - " + summary);
    }

}

// append alarm
function appendAlarm(action, trigger, numProps) {
    let tableRef = document.getElementById('showAlarmTable').getElementsByTagName('tbody')[0];

    let almNumber = tableRef.rows.length + 1;

    let paramArray = [almNumber, action, trigger, numProps];

    let newRow = tableRef.insertRow(tableRef.rows.length);

    let i = 0;
    for (i = 0; i < 4; i++) {
	let newCell = newRow.insertCell(i);
	let newText = document.createTextNode(paramArray[i]);
	newCell.appendChild(newText);
    }
    // appendAlarm("ACTION", "TRIGGER", 100);
}

// append property
function appendProperty(propName, propDescr) {
    let tableRef = document.getElementById('showPropertiesTable').getElementsByTagName('tbody')[0];

    let propNumber = tableRef.rows.length + 1;

    let paramArray = [propName, propDescr];

    let newRow = tableRef.insertRow(tableRef.rows.length);

    let i = 0;
    for (i = 0; i < 2; i++) {
	let newCell = newRow.insertCell(i);
	let newText = document.createTextNode(paramArray[i]);
	newCell.appendChild(newText);
    }
}

// nicify time HHMMSS
function polishTime(rawTime, UTC) {
    //HHMMSS
    //012345
    var returnVal = "";
    returnVal += rawTime.substring(0,2);
    returnVal += ':';
    returnVal += rawTime.substring(2,4);
    returnVal += ':';
    returnVal += rawTime.substring(4,6);

    if (UTC == true) {
	returnVal += ' (UTC)';
    }
    return returnVal;
}

// nicify date
function polishDate(rawDate) {
    //YYYYMMDD
    //01234567
    var returnVal = "";
    returnVal += rawDate.substring(0,4);
    returnVal += '/';
    returnVal += rawDate.substring(4,6);
    returnVal += '/';
    returnVal += rawDate.substring(6,8);

    return returnVal;
}

// populates the event dropdown list
function updateEvtDrop(fileName) {
    /*$("#eventSelect").empty();
    var payload = {};
    payload['filename'] = fileName;

    $.ajax({
	type: 'get',
	datatype: 'json',
	url: '/geteventdrop',
	data: payload,
	success: function(data) {

	},
    });*/
}

// clears the calendar view table and adds events
function showEvents(fileName) {
    appendStatus("updating events for " + fileName);
    $("#calendarViewTable tbody").empty();

    var payload = {};
    payload['filename'] = fileName;

    $.ajax({

	type: 'get',
	datatype: 'json',
	url: '/getevents',
	data: payload,
	success: function(data) {
	    for (var i = 0; i < data.length; i++) {
		var DTJSON = data[i]['startDT'];
		var date = polishDate(DTJSON['date']);
		var time = polishTime(DTJSON['time'], DTJSON['isUTC']);
		var nP = data[i]['numProps'];
		var nA = data[i]['numAlarms'];
		var summ = data[i]['summary'];
		appendEvent(1, date, time, nP, nA, summ);
	    }
	},
	fail: function(error) {
	    appendStatus("could not access that calendar's events...");
	},
    });
}

// validate date 1998/10/26
function validateDate(date) {
    if (date.length != 10)
	return false;

    var year  = date.substring(0, 4);
    var month = date.substring(5, 7);
    var day   = date.substring(8, 10);

    if (isNaN(year) || isNaN(month) || isNaN(day))
	return false;

    if (month > 12 || month < 1)
	return false;

    if (day > 31 || day < 1)
	return false;

    return true;
}

// validate time HH:MM:SS
function validateTime(time) {
    if (time.length != 8)
	return false;

    var hour   = time.substring(0, 2);
    var minute = time.substring(3, 5);
    var second = time.substring(6, 8);

    if (isNaN(hour) || isNaN(minute) || isNaN(second))
	return false;

    if (hour < 0 || hour > 23)
	return false;

    if (minute > 59 || minute < 0)
	return false;

    if (second > 59 || second < 0)
	return false;

    return true;
}

/* UPLOADS AND ICALENDAR FILE TO THE SERVER */
function uploadCalendar(formString, pickerString) {

    /* Create a reference to the FormData being taken */
    let formData = new FormData($(formString)[0]);

    /* Acquire filename by taking the value of the file chooser */
    let fName = $(pickerString).val();

    /* Create the bare name of the file by taking the last index of \ */
    let bareName = fName.substring(fName.lastIndexOf("\\") + 1, fName.length);

    /* If the picker string value is blank, alert the user and abort */
    if (fName == "") {
	appendStatus("You failed to select a file to upload.");
	return;
    }

    /* AJAX: list all files*/
    $.ajax({

	type: 'get',
	dataType: 'json',
	url: '/listallfiles',

	success: function (data) {

	    /* Check if this file already exists */
	    let isDuplicate = false;
	    for (let i = 0; i < data.length; i++) {
		if (bareName == data[i]) {
		    appendConsole('/listallfiles to uploadCalendar: ' + bareName + ' is a duplicate; will refresh all calendars immediately.');
		    appendStatus(bareName + " already exists in the file list, so it will be overwritten.");
		    isDuplicate = true;
		}
	    }

	    /* Then just upload the file through the AJAX request */
	    $.ajax({
		url: '/upload',
		type: 'POST',
		processData: false,
		contentType: false,
		cache: false,
		data: formData,

		/* Succesfully uploaded */
		success: function(data) {
		    appendStatus("Successfully uploaded " + bareName);

		    /* Refresh all files if duplicate or just process
		     * the one calendar if it isn't. */
		    if (isDuplicate == true)
			refreshAllFiles();
		    else
			processCalendar(bareName, isDuplicate, false);
		},

		/* Failed to upload */
		fail: function(error) {
		    appendConsole('/upload to uploadCalendar ' + error);
		}

	    });
	},

	/* If the file listing fails for some reason */
	fail: function(error) {
	    appendConsole('/listallfiles to uploadCalendar: ' + error);
	}
    });

    /* This will clone the file picker prompt so it looks 'cleared' */
    $(pickerString).replaceWith($(pickerString).val('').clone(true));
}

/* PROCESSES A CALENDAR GIVEN A BARE NAME */
function processCalendar(bareName, isDuplicate, createdCal) {

    /* If the calendar being processed is the first calendar, empty
     * the "No files" placeholder text */
    if (emptyFileList == true)
	$("#fileLogTable tbody").empty();

    /* AJAX call to convert the existing .ICS file into a JSON string */
    $.ajax({
	url: '/caltojson',
	type: 'GET',
	data: {"fileName" : bareName},

	/* On success, we take the JSON data and append it to the list */
	success: function(data) {
	    if (data['version'] == 'error') {
		appendStatus("Invalid calendar: " + bareName + ' (' + data['error'] + ')');
	    }
	    else if (isDuplicate == false) {
		appendCalendar(bareName, "/uploads/" + bareName, data['version'], data['prodID'], data['numEvents'], data['numProps']);

		if (hasOneValidCalendar == false) {
		    document.getElementById("storeAllFilesButton").style.display = "inline";
		}

		hasOneValidCalendar = true;
	    }


	},

	fail: function(error) {
	    appendStatus("could not process calendar");
	}
    });

}

function createRawDT(cleanDT) {
    var returnMe = {};

    var cleanDate = cleanDT['date'];
    var cleanTime = cleanDT['time'];
    var cleanUTC  = cleanDT['isUTC'];

    var finalDate = cleanDate.substring(0,4) + cleanDate.substring(5,7) + 
	cleanDate.substring(8,10);

    var finalTime = cleanTime.substring(0,2) + cleanTime.substring(3,5) +
	cleanTime.substring(6,8);

    var finalUTC = false;
    if (cleanUTC == false || cleanUTC == 'false') {
	finalUTC = false;
    }
    else {
	finalUTC = true;
    }

    returnMe['date'] = finalDate;
    returnMe['time'] = finalTime;
    returnMe['isUTC'] = finalUTC;
    return returnMe;
}

function createStampJSON() {
    let currDate = new Date().toISOString();
    let returnThis = {};

    let dateString = currDate.substring(0,4) + '/' +
	currDate.substring(5,7) + '/' +
	currDate.substring(8,10);

    let timeString = currDate.substring(11,13) + ':' +
	currDate.substring(14,16) + ':' +
	currDate.substring(17,19);

    returnThis['date'] = dateString;
    returnThis['time'] = timeString;
    returnThis['isUTC'] = true;

    //a|||||ppendStatus(JSON.stringify(returnThis));

    return JSON.stringify(returnThis);
}

/* Displays the database's status */
function displayDBStatus() {
    $.ajax({

	url: '/getdbstatus',
	type: 'get',

	success: function(data) {
	    appendStatus("Database has " + data['files'] + " files, " + data['events'] + " events, and " + data['alarms'] + " alarms");
	}

    });

}

/* Second query option because I didn't want to type */
function queryOptionTwo() {
    /* Conflicting events */
    $.ajax({

	type: 'get',
	url: '/geteventsfromdbsorted',

	success: function(data) {
	    let arrOfEvt = data['payload'];
	    let pastTime = "";
	    let pastDate = "";
	    let pastSummary = "";
	    let pastOrganizer = "";
	    for (let i = 0; i < arrOfEvt.length; i++) {
		let startDT = arrOfEvt[i]['start_date'];
		let startTime = arrOfEvt[i]['start_time'];
		let Summary = arrOfEvt[i]['summary'];
		let organizer = arrOfEvt[i]['organizer'];

		if (startDT == pastDate && startTime == pastTime) {
		    appendDBCollision(pastDate, pastTime, pastSummary, pastOrganizer);
		    pastSummary = Summary;
		    pastOrganizer = organizer;
		    if (i == arrOfEvt.length - 1) {
			appendDBCollision(startDT, startTime, Summary, organizer);
		    }
		} else {
		    pastTime = startTime;
		    pastDate = startDT;
		    pastSummary = Summary;
		    pastOrganizer = organizer;
		}
	    }
	}
    });
}

/* Third query option because I didn't want to type all of that in the dropdown area */
function queryOptionThree() {
    /* All sorted events */
    $.ajax({

	type: 'get',
	url: '/geteventsfromdbsorted',

	success: function(data) {
	    let arrOfEvt = data['payload'];
	    for (let i = 0; i < arrOfEvt.length; i++) {
		let startDT = arrOfEvt[i]['start_date'];
		let startTime = arrOfEvt[i]['start_time'];
		let Summary = arrOfEvt[i]['summary'];
		appendDBEventSorted(startDT, startTime, Summary);
	    }
	}

    });
}

/* LOCATION SEARCH SUBMIT */
$("#locationForm").submit(function(e) {
    e.preventDefault();
    let value = $("#locationPrompt").val();
    $("#eventLocationTable tbody").empty();
    queryOptionFour(value);
    $("#locationForm")[0].reset();
});

/* Fourth query option because you know */
function queryOptionFour(Location) {
    /* Show events based on year */
    $.ajax({

	type: 'get',
	url: '/geteventsfromdbsortedlocation',
	data: {'loc':Location},

	success: function(data) {
	    let arrOfEvt = data['payload'];
	    for (let i = 0; i < arrOfEvt.length; i++) {
		let startDT = arrOfEvt[i]['start_date'];
		let startTime = arrOfEvt[i]['start_time'];
		let summary = arrOfEvt[i]['summary'];
		let Location = arrOfEvt[i]['location'];
		let organizer = arrOfEvt[i]['organizer'];
		appendDBEventLocation(Location, startDT, startTime, summary);
	    }
	}

    });
}

// append db event
function appendDBEventLocation(Location, startDT, startTime, summary) {
    var tableRef = document.getElementById('eventLocationTable').getElementsByTagName('tbody')[0];
    var paramArray = [Location, startDT, startTime, summary];

    var newRow = tableRef.insertRow(tableRef.rows.length);

    var i;
    for (i = 0; i < 4; i++) {
	var newCell = newRow.insertCell(i);
	var newText = document.createTextNode(paramArray[i]);
	newCell.appendChild(newText);
    }
}

/* YEAR SEARCH SUBMIT */
$("#eventYearForm").submit(function(e) {
    e.preventDefault();
    let value = $("#eventYearPrompt").val();
    $("#eventYearTable tbody").empty();
    queryOptionFive(value);
    $("#eventYearForm")[0].reset();
});

/* Fifth query option */
function queryOptionFive(year) {
    /* Show events based on year */
    $.ajax({

	type: 'get',
	url: '/geteventsfromdbsorted',

	success: function(data) {
	    let arrOfEvt = data['payload'];
	    for (let i = 0; i < arrOfEvt.length; i++) {
		let startDT = arrOfEvt[i]['start_date'];
		let startTime = arrOfEvt[i]['start_time'];
		let summary = arrOfEvt[i]['summary'];
		let Location = arrOfEvt[i]['location'];
		let organizer = arrOfEvt[i]['organizer'];

		let parsedYear = parseInt(startDT.substring(0,4));
		if (year <= parsedYear) {
		    appendDBEventYear(startDT, startTime, summary, Location, organizer);
		}
	    }
	}

    });
}


// append db event
function appendDBEventYear(startDT, startTime, summary, Location, organizer) {
    var tableRef = document.getElementById('eventYearTable').getElementsByTagName('tbody')[0];
    var paramArray = [startDT, startTime, summary, Location, organizer];

    var newRow = tableRef.insertRow(tableRef.rows.length);

    var i;
    for (i = 0; i < 5; i++) {
	var newCell = newRow.insertCell(i);
	var newText = document.createTextNode(paramArray[i]);
	newCell.appendChild(newText);
    }
}

/* Sixth query option */
function queryOptionSix() {
    $.ajax({
    
	url: '/getalarms',
	type: 'get',

	success: function(data) {
	    let arrOfAlm = data['payload'];
	    for (let i = 0; i < arrOfAlm.length; i++) {
		let finalAction = arrOfAlm[i]['action'];
		let finalTrigger = arrOfAlm[i]['trigger'];
		appendDBEventYear(finalAction, finalTrigger);
	    }
	}

    });
}

// append db event
function appendDBEventYear(action, trigger) {
    var tableRef = document.getElementById('alarmRetroTable').getElementsByTagName('tbody')[0];
    var paramArray = [action, trigger];

    var newRow = tableRef.insertRow(tableRef.rows.length);

    var i;
    for (i = 0; i < 2; i++) {
	var newCell = newRow.insertCell(i);
	var newText = document.createTextNode(paramArray[i]);
	newCell.appendChild(newText);
    }
}

