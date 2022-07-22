const { google } = require('googleapis')
const { oauth2 } = require('googleapis/build/src/apis/oauth2')

const { OAuth2 } = google.auth

const clientId = require('./login.json').clientId;
const clientSecret = require('./login.json').clientSecret;
const refreshToken = require('./login.json').refreshToken;
const calIds = [require('./login.json').Calendar_1, require('./login.json').Calendar_2, require('./login.json').Calendar_3, require('./login.json').Calendar_4];
const calColors = [[3, 0, 0], [0, 3, 0], [0, 0, 3], [0, 3, 3]] // colors in function of the calendar id
const hlp = require("./helper");

/* for adding a new calendar, make sure:
- you update the login file in the google cloud environment
- you add exceptions to the overlayhours function

*/


const oAuth2Client = new OAuth2(clientId, clientSecret)
oAuth2Client.setCredentials({ refresh_token: refreshToken })
const calendar = google.calendar({ version: 'v3', auth: oAuth2Client })


// Variables
let nrLeds = 144;
let mode = 1;
let LedSequence = [];
let indColor0 = [0, 0, 0];// the basecolor
let indColor1 = [4, 2, 0]; // the first special color
let indColor2 = [4, 4, 0];
let indColor3 = [4, 4, 4];
let ColorBad = [1, 0, 0];
let ColorGood = [0, 2, 0];

// date
let sleepColor = [0, 0, 0]; // color to set during sleep hours
let nowColor = [0, 8, 0]; // indicates the current time
let appColor0 = [5, 0, 5];
let appColor1 = [0, 0, 0];
let appColor2 = [0, 0, 0];
let appColor3 = [0, 0, 0];
let appColor4 = [0, 0, 0];
let pastDiv = 4;

let runAction = false;
let count = 1;
let maxCount = 144;
let delay = 1300;
let parts = 0;
let datatimes = []
let hourmod = 0;
let showPrint = true;


function picRunAct(prc) {
    if (prc == "start") {
        runAction = true;
    }
    if (prc == "stop") {
        runAction = false;
    }
}

function setLedSequence(scnc) {
    hlp.Print("setting seq");
    LedSequence = scnc;
}

async function getLedSequence() {
    return LedSequence;
}

function setCount(cnt) {
    count = cnt;
}

function pushSpecial(count, pos1, pos2, pos3) {
    if (count % pos3 == 0) {
        LedSequence.push(indColor3);
    }
    else if (count % pos2 == 0) {
        LedSequence.push(indColor2);
    }
    else if (count % pos1 == 0) {
        LedSequence.push(indColor1);
    }
    else {
        LedSequence.push(indColor0);
    }
}

function createBlank() {
    for (let index = 0; index < nrLeds; index++) {
        LedSequence[index] = [0, 0, 0];
    }
}

function createFullSpecial(pos1, pos2, pos3) {
    hlp.Print("setting full special");
    console.log(nrLeds, indColor1, indColor2, indColor3)
    let end = nrLeds;
    console.log("end:", end);
    for (let x = 1; x <= end; x++) {
        pushSpecial(x, pos1, pos2, pos3);
        // console.log("index:", x);
    }
}

function createFull(rgb) {
    for (let index = 0; index < nrLeds; index++) {
        LedSequence[index] = rgb;
    }
}

function fillSet(start, stop, rgb) {
    for (let index = start; index < stop; index++) {
        LedSequence[index] = rgb;
    }
}

async function CountUp(prc) {
    updateIndColors();
    hlp.Print("runaction: " + runAction);
    if (count < maxCount && runAction) {           //  if the counter < 10, call the loop function
        hlp.Print(count);

        // code
        if (count > 0) {
            pushSpecial(count, 5, 10, 30)
        }

        hlp.Print("in countup" + LedSequence);
        count++;                    //  increment the counter
    }                       //  ..  setTimeout()
}

async function testCountUp() {
    setTimeout(function () {   //  call a 3s setTimeout when the loop is called
        hlp.Print(count);

        // code
        if (count > 0) {
            pushSpecial(count, 5, 10, 30)
        }

        // hlp.Print(LedSequence);
        count++;                    //  increment the counter

        if (count < maxCount && runAction) {           //  if the counter < 10, call the loop function
            testCountUp();             //  ..  again which will trigger another 
        }                       //  ..  setTimeout()
    }, delay)
}

async function CountDown(nrMinutes) {
    if (nrMinutes == 0 ) {
        return 0;
    }
    if (count <= nrMinutes && runAction) {     //  call a 3s setTimeout when the loop is called
        hlp.Print("count" + count);
        hlp.Print("minutes" + nrMinutes);
        // code
        if (count > 0) {
            let start = (count - 1) * parts;
            let stop = (count) * parts;
            hlp.Print("startstop: " + start + stop);
            fillSet(start, stop, ColorGood)
        } else {
            hlp.Print("creating full");
            createFull(ColorBad);
            parts = Math.ceil(nrLeds / nrMinutes);
            hlp.Print("runaction parts: " + runAction + parts);
        }
        hlp.Print(LedSequence);
        if (count == nrMinutes){
            return 0;
        }
        count++;                    //  increment the counter
    } 
    
}

async function testCountDown(nrMinutes) {
    if (nrMinutes == 0) { return; }


    createFull(ColorBad);
    // hlp.Print(LedSequence);

    parts = Math.round(nrLeds / nrMinutes);
    hlp.Print("runaction parts: " + runAction + parts);
    testCountDownLoop(nrMinutes);


}

async function testCountDownLoop(nrMinutes) {

    setTimeout(async function () {   //  call a 3s setTimeout when the loop is called
        hlp.Print(count);

        // code
        if (count > 0) {
            let start = (count - 1) * parts;
            let stop = (count) * parts;
            hlp.Print("startstop: " + start + stop);
            fillSet(start, stop, ColorGood)
        }

        // hlp.Print(LedSequence);
        count++;                    //  increment the counter

        if (count <= nrMinutes && runAction) {           //  if the counter < 10, call the loop function
            await testCountDownLoop(nrMinutes);             //  ..  again which will trigger another 
        }
        else {
            return 0;
        }                    //  ..  setTimeout()
    }, delay)
}

// calendar functions
async function createCalendar(hourshift) {
    //console.log("in cal");
    hourmod = hourshift;
    if (hourshift != 0) {
        // if this is running in the cloud then take the cloud variables
        updateIndColors();
        updateCalVars();
    }
    // first create a blank array
    //await createBlank();
    setLedSequence([]);
    // console.log(LedSequence);
    createFullSpecial(6, 18, 72);
    // console.log(LedSequence);
    //console.log("starting sequ");
    //console.log(LedSequence);
    //console.log("");

    // then get the data
    // the order in which the calendars get called dictates which one gets overwritten
    // await getCalendarData(calId);
    // await putAppointment(datatimes, appColor0);

    for (let index = 0; index < calIds.length; index++) {
        await getCalendarData(calIds[index]);
        await putAppointment(datatimes, calColors[index]);
    }

    // dim all the leds in the past + highlight the current led
    await dimPast();
    console.log(LedSequence);

    console.log("cal END ========");

}

function updateIndColors() { // update all variables that have something to do with the calendar
    let show = 0;
    console.log("updating vars")
    if (show) {
        console.log("before : nrLeds:", nrLeds, "baseColor:", indColor0, typeof indColor0, "hourColor:", indColor1, "hour3Color:", indColor2, "hour12Color:", indColor3, "sleepColor:", sleepColor, "nowColor:", nowColor, "appointmentColor:", appColor0, "amColor:", appColor1, "pastDiv:", pastDiv)
    }
    nrLeds = process.env.nrLeds;
    console.log("updated lednrs")

    indColor0 = JSON.parse(process.env.indColor0);
    indColor1 = JSON.parse(process.env.indColor1);
    indColor2 = JSON.parse(process.env.indColor2);
    indColor3 = JSON.parse(process.env.indColor3);
    console.log("updated colornrs")
    
    if (show) {
        console.log("aftr : nrLeds:", nrLeds, "baseColor:", indColor0, typeof indColor0, "hourColor:", indColor1, "hour3Color:", indColor2, "hour12Color:", indColor3, "sleepColor:", sleepColor, "nowColor:", nowColor, "appointmentColor:", appColor0, "amColor:", appColor1, "pastDiv:", pastDiv)
    }
    console.log("done updating vars")
}

function updateCalVars() { // update all variables that have something to do with the calendar
    let show = 1;
    console.log("updating vars")
    if (show) {
        console.log("before : nrLeds:", nrLeds, "baseColor:", indColor0, typeof indColor0, "hourColor:", indColor1, "hour3Color:", indColor2, "hour12Color:", indColor3, "sleepColor:", sleepColor, "nowColor:", nowColor, "appointmentColor:", appColor0, "amColor:", appColor1, "pastDiv:", pastDiv)
    }
    nrLeds = process.env.nrLeds;
    console.log("updated lednrs")

    sleepColor = JSON.parse(process.env.sleepColor);
    nowColor = JSON.parse(process.env.nowColor);
    console.log("updated nowcolor")
    appColor0 = JSON.parse(process.env.appColor0);
    appColor1 = JSON.parse(process.env.appColor1);
    appColor2 = JSON.parse(process.env.appColor2);
    appColor3 = JSON.parse(process.env.appColor3);
    appColor4 = JSON.parse(process.env.appColor4);

    calColors[0] = appColor0;
    calColors[1] = appColor1;
    calColors[2] = appColor2;
    calColors[3] = appColor3;
    calColors[4] = appColor4;
    console.log("updated appcolornrs")
    pastDiv = parseFloat(process.env.pastDiv);
    if (show) {
        console.log("aftr : nrLeds:", nrLeds, "baseColor:", indColor0, typeof indColor0, "hourColor:", indColor1, "hour3Color:", indColor2, "hour12Color:", indColor3, "sleepColor:", sleepColor, "nowColor:", nowColor, "appointmentColor:", appColor0, "amColor:", appColor1, "pastDiv:", pastDiv)
    }
    console.log("done updating vars")
}

async function getCalendarData(calendarId) {
    hlp.Print("start caldata in caldata")
    // set start of today
    const start = new Date();
    start.setHours(0, 0, 0, 0);
    start.addHours(hourmod);

    // set end of today
    const end = new Date();
    end.setHours(0, 1, 0, 0);
    end.addHours(24 + hourmod);

    const times = [];
    hlp.Print("hours set")
    // get events of today
    return calendar.events.list({
        "calendarId": calendarId,
        "alwaysIncludeEmail": false,
        "maxResults": 25,
        "orderBy": "startTime",
        "showDeleted": false,
        "singleEvents": true,
        "timeMax": end,
        "timeMin": start,
        "timeZone": "Europe/Zurich"
    })
        .then(function (response) {
            // Handle the results here (response.result has the parsed body).
            let evitems = response.data.items;
            console.log("evitems");
            // console.log(evitems);
            evitems.forEach(element => {
                if (element.start.dateTime) {
                    // check if starttime and endtime are in the right order
                    let startpos = 0;
                    let endpos = nrLeds;

                    if (dateCheck(element.start.dateTime) != -1) {
                        startpos = timeToLedpos(calTime(element.start.dateTime), true);
                    }
                    if (dateCheck(element.end.dateTime) != 1) {
                        endpos = timeToLedpos(calTime(element.end.dateTime));
                    }
                    if (startpos == -1){
                        startpos = 0;
                    }
                    if (endpos == -1){
                        endpos = 144;
                    }
                    console.log("start end ", startpos, endpos, nrLeds)
                    times.push([parseInt(startpos), parseInt(endpos)]);


                    console.log("item name: ", element.summary)
                    console.log("start: ", element.start.dateTime);
                    // formatTime(element.start.dateTime);
                }
            });
            datatimes = times;
            console.log(datatimes);
        },
            function (err) {
                console.error("Execute error", err);
            });
}

Date.prototype.addHours = function (h) {
    this.setTime(this.getTime() + (h * 60 * 60 * 1000));
    return this;
}

function calTime(timeIn) {
    //console.log("timein: ",timeIn)
    let hour = timeIn.substring(11, 13);
    //console.log("hour: ",hour)
    let min = timeIn.substring(14, 16);
    return [hour, min];
}

function timeToLedpos([hour, min], down = false) {
    // change the time into a position on the ledstrip
    //console.log("hour: ",hour,":", min)
    let deciMin = 0;
    if (down) {
        deciMin = Math.floor(min / 10);
    }
    else {
        deciMin = Math.ceil(min / 10);
    }

    let ledPos = hour * 6 + deciMin-1;
    // console.log("-----------------------")
    // console.log("info:")
    // console.log(hour, min, deciMin, ledPos);
    // console.log("-----------------------")
    return ledPos;
}

function dateCheck(timeIn) {
    let date = Date.parse(timeIn);
    let start = new Date();
    start.setHours(0, 0, 0, 0);
    start.addHours(hourmod);

    // set end of today
    let end = new Date();
    end.setHours(0, 1, 0, 0);
    end.addHours(24 + hourmod);
    if (date < start) {
        return -1
    }
    else if (date > end) {
        return 1
    }
    else {
        return 0
    }
}

async function putAppointment(datetimesIn, color) {
    // pushing the appointements to the led array
    console.log("pushing appointments:")
    console.log("dates in:", datetimesIn, "color:", color)
    datetimesIn.forEach(element => {
        // console.log(element);
        for (let index = element[0]; index < element[1]; index++) {
            LedSequence[index] = addRGBS(LedSequence[index], color);
            // console.log(element);
        }
    });
    console.log(LedSequence);
}

function addRGBS(rgb1, rgb2) {
    let rgb = [0, 0, 0];
    rgb[0] = Math.max(rgb1[0], rgb2[0])
    rgb[1] = Math.max(rgb1[1], rgb2[1])
    rgb[2] = Math.max(rgb1[2], rgb2[2])
    return rgb
}
async function dimPast() {
    console.log("dim past");
    // console.log(LedSequence);
    let now = new Date();
    now.addHours(1);
    let h = addZero(now.getHours());
    let m = addZero(now.getMinutes());
    console.log(now.getHours(), now.getMinutes());
    console.log(now);
    const curPos = timeToLedpos([h, m], true);

    for (let pos = 0; pos <= curPos; pos++) {
        if (pos == curPos) {
            LedSequence[pos] = nowColor;
        }
        else {
            // console.log("ledpos", [pos]);
            // console.log("before:",LedSequence[pos]);
            //let newlet = [0,0,0]
            //console.log("newvar:",newvar);
            // console.log("ledpos:", LedSequence[pos][0], "div:", pastDiv);
            let newvar = [Math.round((LedSequence[pos][0]) / pastDiv), Math.round((LedSequence[pos][1]) / pastDiv), Math.round((LedSequence[pos][2]) / pastDiv)];
            // console.log("newvar:",newvar);
            LedSequence[pos] = newvar;
            // console.log("after: ",LedSequence[pos]);
            // console.log("");
        }
    }
    // console.log(LedSequence);
}

function addZero(i) {
    if (i < 10) {
        i = "0" + i;
    }
    return i;
}

// functies
module.exports.SetRunAction = picRunAct;
module.exports.SetLedSequence = setLedSequence;
module.exports.GetLedSequence = getLedSequence;
module.exports.SetCount = setCount;
module.exports.CountUp = CountUp;
module.exports.TestCountUp = testCountUp;
module.exports.CountDown = CountDown;
module.exports.TestCountDown = testCountDown;
module.exports.CreateCalendar = createCalendar;

// variablen
module.exports.LedSequence = LedSequence;
module.exports.Count = count;
// let refreshRate = 10;
// let website = "testWebsite";

module.exports.indColor0 = indColor0;
module.exports.indColor1 = indColor1;
module.exports.indColor2 = indColor2;
module.exports.indColor3 = indColor3;
module.exports.Mode = mode;

module.exports.RunAction = runAction;

module.exports.MaxCount = maxCount;
module.exports.Delay = delay;
