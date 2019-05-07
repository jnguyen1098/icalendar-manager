'use strict'

// Settings tweaks
const globalChrono = true; // sort files from oldest to newest; will alpha if false

// C library API
const ffi = require('ffi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

// MySql time
const mysql = require('mysql');
var connection;

app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
    res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
    //Feel free to change the contents of style.css to prettify your Web app
    res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
    fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
	const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
	res.contentType('application/javascript');
	res.send(minimizedContents._obfuscatedCode);
    });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
    if(!req.files) {
	return res.status(400).send('No files were uploaded.');
	console.log('/upload: no files were uploaded.');
    }

    let uploadFile = req.files.uploadCalendarPicker;

    // Use the mv() method to place the file somewhere on your server
    uploadFile.mv('uploads/' + uploadFile.name, function(err) {
	if(err) {
	    return res.status(500).send(err);
	}
	console.log('/upload: ' + uploadFile.name + ' was uploaded by user');
	res.redirect('/');
    });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
    fs.stat('uploads/' + req.params.name, function(err, stat) {
	console.log(err);
	if(err == null) {
	    res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
	} else {
	    res.send('else statement');
	}
    });
});

//******************** Your code goes here ********************

let parser = ffi.Library('./libfinal', {
    'createCalendarToJSON': ['string', ['string']],
    'serverCreateCalendar': ['int', ['string', 'string', 'string', 'int']],
    'eventListToJSONWrapper':['string', ['string']],
    'filenameToList':['string', ['string','int','int']],
});


//*************************************************************
/* Append to Console */
app.get('/appendconsole', function(req) {
    console.log(req.query['message']);
});

//CreateCalendar
app.get('/servercreatecal', function(req, res){
    var filename = './uploads/' + req.query['filename'];
    var calJSON = JSON.stringify(req.query['caljson']);
    var evtJSON = JSON.stringify(req.query['evtjson']);
    var resultCode = parser.serverCreateCalendar(calJSON, evtJSON, filename, 1);
    var result = {};
    result['resultCode'] = resultCode;
    res.send(result);
    console.log(req.query['evtjson']['DTSTART']);
});

//AddEvent
app.get('/serveraddevent', function(req, res){
    var filename = './uploads/' + req.query['filename'];
    var calJSON = 'null';
    var evtJSON = JSON.stringify(req.query['evtjson']);
    var resultCode = parser.serverCreateCalendar(calJSON, evtJSON, filename, 2);
    var result = {};
    result['resultCode'] = resultCode;
    res.send(result);
});

//Respond to GET requests for alarm list
app.get('/getalarmjson', function(req, res){
    let fileName = req.query['fileName'];
    let eventNum = req.query['eventNum'];
    let fileMode = req.query['mode'];

    let result = parser.filenameToList(fileName, eventNum, fileMode);
    res.send(result);
});

//Respond to GET requests for prop list
app.get('/getpropsjson', function(req, res){
    let fileName = req.query['fileName'];
    let eventNum = req.query['eventNum'];
    let fileMode = req.query['mode'];

    let result = parser.filenameToList(fileName, eventNum, fileMode);
    console.log("your proplist is " + result);
    res.send(result);
});

//Respond to GET requests for a calendar json string
app.get('/caltojson', function(req , res){
    /* Reference to the JSON request */
    let query = req.query;

    /* Our filename */
    let fileName = query['fileName'];

    /* Create our resultant JSON string */
    let calJSON = parser.createCalendarToJSON('uploads/' + fileName);

    /* Parse the resultant string */
    let finalJSON = JSON.parse(calJSON);

    /* Success checking */
    if (finalJSON['version'] == 'error') {
	console.log('/caltojson: [' + finalJSON['error'] + '] - ' + fileName);
    } else {
	console.log('/caltojson: [OK] - ' + fileName);
    }
    res.send(finalJSON);
});

//Respond to GET requests for a Calendar's event list
app.get('/getevents', function(req, res) {
    var fileName = req.query['filename'];
    var finalpath = __dirname + '/uploads/' + fileName;
    var stringBoy = parser.eventListToJSONWrapper(finalpath);
    var arrOfEvt = JSON.parse(stringBoy);
    res.send(arrOfEvt);
});

//Respond to GET requests for a list of filenames in upload/
app.get('/listallfiles', function(req , res){
    fs.readdir('uploads/', function(err, items) {
	//c|||||onsole.log(items);

	for (var i = 0; i < items.length; i++) {
	    //c|||||onsole.log(items[i]);
	}

	if (globalChrono == true) {
	    items.sort(function(a, b) {
		return fs.statSync('uploads/' + a).mtime.getTime() -
		    fs.statSync('uploads/' + b).mtime.getTime();
	    });
	}

	res.send(items);
    });
});

//Login to mysql database
app.get('/databaseLogin', function(req, res) {
    let usernameVal = req.query['usernameVal'];
    let passwordVal = req.query['passwordVal'];
    let databaseVal = req.query['databaseVal'];

    connection = mysql.createConnection({
	host	: 'dursley.socs.uoguelph.ca',
	user	: usernameVal,
	password: passwordVal,
	database: databaseVal
    });

    connection.connect(function(err) {
	if (err) {
	    console.log(err.message);
	    res.send({'result':'failure'});
	}
	else {
	    console.log("Could log in.");

	    let createFileTable = 'create table FILE(cal_id INT PRIMARY KEY AUTO_INCREMENT, file_Name VARCHAR(60) NOT NULL, version INT NOT NULL, prod_id VARCHAR(256) NOT NULL)';
	    connection.query(createFileTable, function(err, results, fields) {
		if (err) {
		    console.log(err.message);
		} else {
		    console.log("Could successfully create FILE table");
		}
	    });

	    let createEventTable = 'create table EVENT(event_id INT PRIMARY KEY AUTO_INCREMENT, summary VARCHAR(1024), start_time DATETIME NOT NULL, location VARCHAR(60), organizer VARCHAR(256), cal_file INT NOT NULL, FOREIGN KEY(cal_file) REFERENCES FILE(cal_id) ON DELETE CASCADE)';
	    connection.query(createEventTable, function(err, result, fields) {
		if (err) {
		    console.log(err.message);
		} else {
		    console.log("Could successfully create EVENT table");
		}
	    });

	    let createAlarmTable = 'create table ALARM(alarm_id INT AUTO_INCREMENT PRIMARY KEY, action VARCHAR(256) NOT NULL, `trigger` VARCHAR(256) NOT NULL, event INT NOT NULL, FOREIGN KEY(event) REFERENCES EVENT(event_id) ON DELETE CASCADE)';
	    connection.query(createAlarmTable, function(err, result, fields) {
		if (err) {
		    console.log(err.message);
		} else {
		    console.log("Could successfully create ALARM table");
		}
	    });

	    res.send({'result':'success'});
	}

    });
});

//Store all files
app.get('/filestotable', function(req, res) {
    fs.readdir('uploads/', function(err, items) {
	let numEvents = 0;
	for (var i = 0; i < items.length; i++) {

	    let cal_id_primary = i + 1;
	    let duplicate = false; 
	    let item = items[i];
	    let fName = items[i];

	    connection.query("SELECT file_Name from FILE", function(err, result, fields) {
		if (err) {
		    console.log("Could not retrieve filenames from FILE");
		} else {
		    for (let j = 0; j < result.length; j++) {
			if (result[j]['file_Name'] == fName) {
			    duplicate = true;
			}
		    }
		    if (duplicate == false) {
			let calJSON = parser.createCalendarToJSON('uploads/' + fName);

			let finalJSON = JSON.parse(calJSON);

			let ffName = "'" + fName + "'";
			let versio = finalJSON['version'];
			let prodID = "'" + finalJSON['prodID'] + "'";

			let insert = 'INSERT INTO FILE (cal_id, file_Name, version, prod_id)'
			insert += ' VALUES (';
			insert += 'null' + ', ';
			insert += ffName + ', ';
			insert += versio + ', '; 
			insert += prodID + ');';

			connection.query(insert, function(err, result, fields) {
			    if (err) {
				console.log(err.message);
			    } else {	
				/* Push events next */
				let evtWrp = parser.eventListToJSONWrapper('uploads/' + fName);
				let arrOfEvt = JSON.parse(evtWrp);
				for (let j = 0; j < arrOfEvt.length; j++) {

				    let evtSummary = arrOfEvt[j]['summary'] == '' ? 'null' : "'" + arrOfEvt[j]['summary'] + "'";
				    let evtStartDT = "'" + iCalToSQLDT(arrOfEvt[j]['startDT']) + "'";
				    let evtLocation = arrOfEvt[j]['location'] == '' ? 'null' : "'" + arrOfEvt[j]['location'] + "'";
				    let evtOrganizer = arrOfEvt[j]['organizer'] == '' ? 'null' : "'" + arrOfEvt[j]['organizer'] + "'";
				    let evtForeignKey = cal_id_primary;

				    let addEvent = 'INSERT INTO EVENT VALUES (';
				    addEvent += 'null,';
				    addEvent += evtSummary + ',';
				    addEvent += evtStartDT + ',';
				    addEvent += evtLocation + ',';
				    addEvent += evtOrganizer + ',';
				    addEvent += evtForeignKey + ')';
				    connection.query(addEvent, function(err, result, fields) {
					if (err) {
					    console.log(err.message);
					} else {
					    numEvents++; // TODO this is very bad
					    let alarmString = parser.filenameToList('uploads/' + fName, j, 1);
					    let arrOfAlm = JSON.parse(alarmString);
					    for (let a = 0; a < arrOfAlm.length; a++) {
						let almAction = arrOfAlm[a]['action'];
						let almTrigger= arrOfAlm[a]['trigger'];
						let almForeign= numEvents;

						let addAlarm = 'INSERT INTO ALARM VALUES (';
						addAlarm += 'null,';
						addAlarm += "'" + almAction + "',";
						addAlarm += "'" + almTrigger + "',";
						addAlarm += almForeign + ')';
						connection.query(addAlarm, function(err, result, fields) {
						    if (err) {
							console.log(err.message);
						    } else {
							console.log("Pushed alarm " + (a + 1) + " of event " + almForeign);
						    }
						});
					    }
					    console.log("Pushed event " + (j + 1) + " of calendar " + fName);

					}
				    });
				}
				console.log("Pushed " + fName + " to FILE table");
			    }
			});
		    } else {
			console.log(fName + " is a duplicate and was not added");
		    }
		}
	    });

	}
    });
    res.send({'result':'success'});
});

//GET database statuses
app.get('/getdbstatus', function(req, res) {
    let fileCount = -1;
    let eventCount = -1;
    let alarmCount = -1;

    let surveyFiles = 'SELECT * FROM FILE';
    connection.query(surveyFiles, function(err, result, fields) {
	fileCount = result.length;

	let surveyEvents = 'SELECT * FROM EVENT';
	connection.query(surveyEvents, function(err, result, fields) {
	    eventCount = result.length;

	    let surveyAlarms = 'SELECT * FROM ALARM';
	    connection.query(surveyAlarms, function(err, result, fields) {
		alarmCount = result.length;

		let resultJ = {};
		resultJ['files'] = fileCount;
		resultJ['events'] = eventCount;
		resultJ['alarms'] = alarmCount;
		res.send(resultJ);

	    });
	});
    });
});

//clear all tables
app.get('/clearalltables', function(req, res) {
    let clearAlarms = 'DELETE FROM ALARM';
    connection.query(clearAlarms, function(err) {
	if (err) {
	    console.log(err.message);
	}
    });

    let clearEvents = 'DELETE FROM EVENT';
    connection.query(clearEvents, function(err) {
	if (err) {
	    console.log(err.message);
	}
    });

    let clearFiles  = 'DELETE FROM FILE';
    connection.query(clearFiles, function(err) {
	if (err) {
	    console.log(err.message);
	}
    });

    let resetFiles = 'ALTER TABLE FILE AUTO_INCREMENT = 1';
    connection.query(resetFiles, function(err) {
	if (err) {
	    console.log(err.message);
	}
    });

    let resetEvents = 'ALTER TABLE EVENT AUTO_INCREMENT = 1';
    connection.query(resetEvents, function(err) {
	if (err) {
	    console.log(err.message);
	}
    });

    let resetAlarms = 'ALTER TABLE ALARM AUTO_INCREMENT = 1';
    connection.query(resetAlarms, function(err) {
	if (err) {
	    console.log(err.message);
	}
    });

    res.send({'result':'success'});
});

// returns the events from a DB table's FILE children
app.get('/geteventsfromdb', function(req, res) {
    let fileName = req.query['fileName'];
    let cal_id = -1;
    let payload = [];
    let query = "SELECT cal_id from FILE where file_Name ='" + fileName + "'";
    connection.query(query, function(err, result) {
	if (err) {
	    console.log(err.message);
	} else {
	    cal_id = JSON.parse(JSON.stringify(result))[0]['cal_id'];
	    let eventQuery = "SELECT * from EVENT where cal_file = " + cal_id;
	    connection.query(eventQuery, function(err, result) {
		if (err) {
		    console.log(err.message);
		} else {
		    for (let i = 0; i < result.length; i++) {
			let temp = {};
			temp['summary'] = result[i].summary;
			temp['start_time'] = (JSON.stringify(result[i].start_time)).substring(12,20);
			temp['start_date'] = (JSON.stringify(result[i].start_time)).substring(1,11);
			temp['organizer'] = result[i].organizer;
			payload[i] = temp;
		    }
		    res.send({'payload':payload});
		}
	    });
	}
    });
});

// returns the events from a DB table's FILE children
app.get('/geteventsfromdbsorted', function(req, res) {
    let payload = [];
    let eventQuery = "SELECT * from EVENT ORDER BY start_time";
    connection.query(eventQuery, function(err, result) {
	if (err) {
	    console.log(err.message);
	} else {
	    for (let i = 0; i < result.length; i++) {
		let temp = {};
		temp['summary'] = result[i].summary;
		temp['start_time'] = (JSON.stringify(result[i].start_time)).substring(12,20);
		temp['start_date'] = (JSON.stringify(result[i].start_time)).substring(1,11);
		temp['organizer'] = result[i].organizer;
		temp['location'] = result[i].location;
		payload[i] = temp;
	    }
	    res.send({'payload':payload});
	}
    });
});

// returns the events matching a certain location
app.get('/geteventsfromdbsortedlocation', function(req, res) {
    let payload = [];
    let eventQuery = "SELECT * FROM EVENT WHERE location=\"" + req.query['loc'] + "\"";
    console.log(eventQuery);
    connection.query(eventQuery, function(err, result) {
	if (err) {
	    console.log(err.message);
	} else {
	    for (let i = 0; i < result.length; i++) {
		let temp = {};
		temp['summary'] = result[i].summary;
		temp['start_time'] = (JSON.stringify(result[i].start_time)).substring(12,20);
		temp['start_date'] = (JSON.stringify(result[i].start_time)).substring(1,11);
		temp['organizer'] = result[i].organizer;
		temp['location'] = result[i].location;
		payload[i] = temp;
	    }
	    res.send({'payload':payload});
	}
    });
});

// returns the alarms in the database
app.get('/getalarms', function(req, res) {
    let payload = [];
    let alarmQuery = "SELECT * FROM ALARM";
    let currYear = new Date().getFullYear();
    connection.query(alarmQuery, function(err, result) {
	if (err) {
	    console.log(err.message);
	} else {
	    for (let i = 0; i < result.length; i++) {
		let trigger = result[i].trigger;
		if (trigger.substring(0,16) == 'VALUE=DATE-TIME:') {
		    let yearVal = trigger.substring(16,20);
		    if (yearVal > currYear) {
			let temp = {};
			temp['action'] = result[i].action;
			temp['trigger'] = result[i].trigger;
			payload.push(temp);
			console.log(result[i].trigger);
		    }
		}
	    }
	    res.send({'payload':payload});
	}
    });
});

// Returns the filenames present on the server
app.get('/getdbfilenames', function(req, res) {
    let payload = [];
    let query = "SELECT file_Name FROM FILE";
    connection.query(query, function(err, result) {
	if (err) {
	    console.log(err.message);
	} else {
	    for (let i = 0; i < result.length; i++) {
		payload.push(result[i].file_Name);
	    }
	    res.send(payload);
	}
    });
});

// 

/* Converts a raw DT into a SQL date 
 * {"date":"19981212","time":"121212",isUTC:false}*/
function iCalToSQLDT(iCalDate) {
    let sqlDT = "Placeholder";

    let finalYear = iCalDate['date'].substring(0,4);
    let finalMonth = iCalDate['date'].substring(4,6);
    let finalDay = iCalDate['date'].substring(6,8);

    let finalHour = iCalDate['time'].substring(0,2);
    let finalMinute = iCalDate['time'].substring(2,4);
    let finalSecond = iCalDate['time'].substring(4,6);

    sqlDT = finalYear + "-" + finalMonth + "-" + finalDay + " " + finalHour + ":" + finalMinute + ":" + finalSecond;
    return sqlDT;
}

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
