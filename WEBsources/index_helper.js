

function HSVtoRGB(h, s, v) {
    /* accepts parameters
     *  h  Object = {h:x, s:y, v:z}
     * OR 
     * h, s, v
     */
    var r, g, b, i, f, p, q, t;
    if (arguments.length === 1) {
        s = h.s, v = h.v, h = h.h;
    }
    i = Math.floor(h * 6);
    f = h * 6 - i;
    p = v * (1 - s);
    q = v * (1 - f * s);
    t = v * (1 - (1 - f) * s);
    switch (i % 6) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }
    return {
        r: Math.round(r * 255),
        g: Math.round(g * 255),
        b: Math.round(b * 255)
    };
}

var temperatureGauge = new RadialGauge({
    renderTo: document.createElement('canvas'),
    width: 300,
    height: 300,
    units: "\u00B0C ",
    title: "Temperature",
    minValue: 80,
    maxValue: 120,
    majorTicks: [
        80, 85, 90, 95, 100, 105, 110, 115, 120
    ],
    minorTicks: 2,
    strokeTicks: true,
    highlights: [
        {
            "from": 80,
            "to": 100,
            "color": "rgba(0,0, 255, .3)"
        },
        {
            "from": 100,
            "to": 120,
            "color": "rgba(255, 0, 0, .3)"
        }
    ],
    ticksAngle: 225,
    startAngle: 67.5,
    colorMajorTicks: "#ddd",
    colorMinorTicks: "#ddd",
    colorTitle: "#eee",
    colorUnits: "#ccc",
    colorNumbers: "#eee",
    colorPlate: "#222",
    borderShadowWidth: 0,
    borders: true,
    needleType: "arrow",
    needleWidth: 2,
    needleCircleSize: 7,
    needleCircleOuter: true,
    needleCircleInner: false,
    animationDuration: 100,
    animationRule: "linear",
    colorBorderOuter: "#333",
    colorBorderOuterEnd: "#111",
    colorBorderMiddle: "#222",
    colorBorderMiddleEnd: "#111",
    colorBorderInner: "#111",
    colorBorderInnerEnd: "#333",
    colorNeedleShadowDown: "#333",
    colorNeedleCircleOuter: "#333",
    colorNeedleCircleOuterEnd: "#111",
    colorNeedleCircleInner: "#111",
    colorNeedleCircleInnerEnd: "#222",
    valueBoxBorderRadius: 0,
    colorValueBoxRect: "#222",
    colorValueBoxRectEnd: "#333"
});


var powerGauge = new LinearGauge({
    renderTo: document.createElement('canvas'),
    title: "Heater Power",
    width: 400,
    height: 80,
    minValue: 0,
    maxValue: 3,
    majorTicks: [
        "0",
        "10",
        "100",
        "1000"
    ],
    minorTicks: 0.1,
    colorPlate: "#333e",

    borderShadowWidth: 1,
    borders: false,
    barBeginCircle: false,
    ticksWidth: 20,
    ticksWidthMinor: 15,
    colorNeedle: "cornsilk",
    colorNumbers: "cornsilk",
    colorTitle: "cornsilk",
    animationDuration: 150,
    animationRule: "linear",
    animationTarget: "plate",
    tickSide: "left",
    numberSide: "left",
    needleSide: "left",
    needleWidth: 7,
    colorBarProgress: "#327ac0",
    colorBar: "#bd976d"

});

fetch('api/v1/firmware')
    .then(function (response) {
        return response.json()
    })
    .then(function (data) {
        parseFirmware(data);
    })
    .catch(function (err) {
        console.log('error' + err);
    });


if (!!window.EventSource) {
    var source = new EventSource('/events');
    source.addEventListener('open', function (e) {
        console.log("Events Connected");
    }, false);
    source.addEventListener('error', function (e) {
        if (e.target.readyState != EventSource.OPEN) {
            console.log("Events Disconnected");
        }
    }, false);

    source.addEventListener('status', function (e) {
        //  console.log("Status:", e.data);
        parseState(JSON.parse(e.data));
    }, false);
}

function buildStatusLine(measure, value, verbatim) {
    divStatusLine = document.createElement("div");
    var divStatusMeasure = document.createElement("div");
    var divStatusValue = document.createElement("div");
    divStatusLine.setAttribute("class", "statusLine");

    divStatusMeasure.setAttribute("class", "measure");
    divStatusValue.setAttribute("class", "value");

    divStatusLine.setAttribute("id", measure)
    divStatusMeasure.setAttribute("id", measure);
    divStatusValue.setAttribute("id", measure)

    divStatusMeasure.innerHTML = verbatim;
    divStatusLine.appendChild(divStatusMeasure);
    divStatusValue.innerHTML = value;
    divStatusLine.appendChild(divStatusValue);
    return divStatusLine;

}
function parseState(data) {

    temperatureGauge.value = data.measuredTemperature;

    var jsonHighlights = {};
    var highlightRange = {};
    jsonHighlights.highlights = [];
    highlightRange.from = 80;
    highlightRange.to = data.targetTemperature - 5.0;
    highlightRange.color = "rgba(0,0,255,1)";
    jsonHighlights.highlights.push(highlightRange);

    highlightRange = {};
    highlightRange.from = data.targetTemperature - 5.0;
    highlightRange.to = data.targetTemperature - 2.0;
    highlightRange.color = "rgba(0, 0 , 255, .5)";
    jsonHighlights.highlights.push(highlightRange);

    highlightRange = {};
    highlightRange.from = data.targetTemperature - 2.0;
    highlightRange.to = data.targetTemperature - 1.0;
    highlightRange.color = "rgba(0, 0 , 255, .25)";
    jsonHighlights.highlights.push(highlightRange);

    highlightRange = {};
    highlightRange.from = data.targetTemperature - 1.0;
    highlightRange.to = data.targetTemperature + 1.0;
    highlightRange.color = "rgba(0, 255 , 0, 1)";
    jsonHighlights.highlights.push(highlightRange);

    highlightRange = {};
    highlightRange.from = data.targetTemperature + 1.0;
    highlightRange.to = data.targetTemperature + 2.0;
    highlightRange.color = "rgba(255, 0 , 0, .25)";
    jsonHighlights.highlights.push(highlightRange);

    highlightRange = {};
    highlightRange.from = data.targetTemperature + 2.0;
    highlightRange.to = data.targetTemperature + 5.0;
    highlightRange.color = "rgba(255, 0, 0, 0.5)";
    jsonHighlights.highlights.push(highlightRange);

    highlightRange = {};
    highlightRange.from = data.targetTemperature + 5.0;
    highlightRange.to = 120;
    highlightRange.color = "rgba(255, 0, 0, 1)";
    jsonHighlights.highlights.push(highlightRange);


    temperatureGauge.update(jsonHighlights);
    temperatureGauge.draw();

    powerGauge.value = Math.log10(data.heaterPower);

    var rgbcolor = HSVtoRGB(.75 - .75 / 3 * Math.log10(Math.max(data.heaterPower, 1)), 1, 1);
    var jsoncolor = {};
    jsoncolor.colorBarProgress = "rgba(" + rgbcolor.r + "," + rgbcolor.g + "," + rgbcolor.b + ",1)";
    powerGauge.update(jsoncolor);
    powerGauge.draw();


    var pidModeSwitch = document.getElementById("toggle--pwr");
    var extCtrlSwitch = document.getElementById("toggle--pidext")
    if (data.powerOffMode) {
        pidModeSwitch.checked = false;
    } else {
        pidModeSwitch.checked = true;
    }

    if (data.externalControlMode) {

        extCtrlSwitch.checked = false;
    } else {

        extCtrlSwitch.checked = true;
    }
}
function parseFirmware(data) {
    var divFirmware = document.getElementById("firmware")
    divFirmware.innerHTML = "Firmware:" + data.version;
}


function pwrSwitch() {
    if (document.getElementById("toggle--pwr").checked) {
        // (Heater is OFF)
        fetch('/api/v1/pwr?powerOffMode=false');
    } else {
        // .(HEATER is ON)
        fetch('/api/v1/pwr?powerOffMode=true');
    }
}

function pidextSwitch() {
    if (document.getElementById("toggle--pidext").checked) {
        // (Heater is OFF)
        fetch('/api/v1/set?externalControlMode=false');
    } else {
        // .(External is ON)
        fetch('/api/v1/set?externalControlMode=true');

    }
}


