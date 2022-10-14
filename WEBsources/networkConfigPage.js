// Javascript to populate a table with scan results
// scan returns: 
/* [{"rssi":-46,"ssid":"Watergolfjes","bssid":"F4:92:BF:6A:B4:09","channel":1,"secure":3},
    {"rssi":-79,"ssid":"Watergolfjes","bssid":"F0:9F:C2:AD:28:DF","channel":5,"secure":3},
    {"rssi":-79,"ssid":"TP-Link_1A18","bssid":"60:A4:B7:0E:72:1A","channel":9,"secure":3},
    {"rssi":-81,"ssid":"5G  hersendoder","bssid":"FA:9F:C2:AD:28:DF","channel":5,"secure":3}
   ]
*/

// Execute the whole lot after page load. 
// the configuration page after load


function getWifiSVG() {
    const tmplt = document.createElement("template");
    tmplt.innerHTML = ' <svg width="100%" height="100%" class="wifiRaw" viewBox="0 0 39 26" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" xml:space="preserve" xmlns:serif="http://www.serif.com/" style="fill-rule:evenodd;clip-rule:evenodd;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:1.5;"> \
    <g transform="matrix(1,0,0,1,-18.1604,-42.0371)"> \
        <g id="w1" transform="matrix(0.00112589,1.0194,-1.0194,0.00112589,98.065,25.8788)"> \
            <path d="M34.669,55.467C33.151,56.343 32.216,57.962 32.216,59.715C32.216,61.467 33.151,63.086 34.669,63.962" style="fill:rgb(,0,0);fill-opacity:0;stroke:rgb(0,0,0);stroke-width:3.43px;"/> \
        </g> \
        <g id="w2" transform="matrix(0.00225177,2.0388,-2.0388,0.00225177,158.899,-12.0296)"> \
            <path d="M34.669,55.467C33.151,56.343 32.216,57.962 32.216,59.715C32.216,61.467 33.151,63.086 34.669,63.962" style="fill:rgb(0,0,0);fill-opacity:0;stroke:rgb(0,0,0);stroke-width:1.72px;"/> \
        </g> \
        <g id="w3" transform="matrix(0.00337766,3.0582,-3.0582,0.00337766,219.733,-49.9379)"> \
            <path d="M34.669,55.467C33.151,56.343 32.216,57.962 32.216,59.715C32.216,61.467 33.151,63.086 34.669,63.962" style="fill:rgb(0,0,0);fill-opacity:0;stroke:rgb(0,0,0);stroke-width:1.14px;"/> \
        </g> \
        <g id="w4" transform="matrix(0.00450354,4.0776,-4.0776,0.00450354,280.567,-87.8462)"> \
            <path d="M34.669,55.467C33.151,56.343 32.216,57.962 32.216,59.715C32.216,61.467 33.151,63.086 34.669,63.962" style="fill:rgb(0,0,0);fill-opacity:0;stroke:rgb(0,0,0);stroke-width:0.86px;"/> \
        </g> \
        <g id="w0" transform="matrix(2.40204,0,0,2.40204,-58.4881,-101.337)"> \
            <circle cx="39.849" cy="69.11" r="1.01" style="fill:rgb(0,0,0);"/> \
        </g> \
    </g> \
</svg>';
    return (tmplt.content);
}




window.addEventListener("load", function () {
    console.log("scantable.js executing");
    updatePage();
    UpdateScanTable();

});


function UpdateScanTable() {
    console.log("Updating Scan Table");
    // selecting loading div

    const loader = document.querySelector("#loading");
    displayLoading(loader);

    const scantable = document.getElementById("scantable");
    scantable.setAttribute("class", "tableTable");
    while (scantable.hasChildNodes()) {
        console.log("Removing" + scantable.firstChild);
        scantable.removeChild(scantable.firstChild);
    }
    fetch(url + "scan")
        .then(function (response) {
            return response.json()
        })
        .then(function (data) {
            console.log(data);

            let uniqueData = [];
            // sort by signal strength
            data.sort(function compareScanElements(a, b) { a.rssi - b.rssi });
            console.log(data);
            // remove duplicates
            let uniqueKeys = [];
            data.forEach((element) => {
                if (!uniqueKeys.includes(element.ssid)) {
                    uniqueKeys.push(element.ssid);
                    uniqueData.push(element)
                }
                // uniqueData contains ordered list of unique SSIDs
            });

            let tablebody = document.createElement("div")
            tablebody.setAttribute("class", "tableBody")
            for (var i = 0; i < uniqueData.length; i++) {
                tablebody.append(createScanRow(uniqueData[i], i))
            };
            scantable.append(tablebody);
            hideLoading(loader);
        })
        .catch(function (err) {
            console.log(err);
        });
}


// Returns a div object that makes up a row in the table with scans
function createScanRow(scandata, i) {
    console.log(scandata);
    let d = document.createElement("div");
    d.setAttribute("id", "networkRow" + i);
    d.setAttribute("class", "networkSelection tableRow");

    let d1 = document.createElement("div");
    d1.setAttribute("id", "SSID" + i);
    d1.setAttribute("class", "SSIDField tableField");
    let a = document.createElement('a');
    a.setAttribute("href", "#p");
    a.setAttribute("onClick", "c(this,\"" + scandata.pass + "\")");
    a.innerHTML = scandata.ssid;
    d1.appendChild(a);
    d.append(d1);

    let d2 = document.createElement("div");
    d2.setAttribute("id", "securityVal" + i);
    d2.setAttribute("class", "SecurityField tableField");

    // See esp_fifi_types.h
    // WIFI_AUTH_OPEN = 0,         /**< authenticate mode : open */
    // WIFI_AUTH_WEP,              /**< authenticate mode : WEP */
    // WIFI_AUTH_WPA_PSK,          /**< authenticate mode : WPA_PSK */
    // WIFI_AUTH_WPA2_PSK,         /**< authenticate mode : WPA2_PSK */
    // WIFI_AUTH_WPA_WPA2_PSK,     /**< authenticate mode : WPA_WPA2_PSK */
    // WIFI_AUTH_WPA2_ENTERPRISE,  /**< authenticate mode : WPA2_ENTERPRISE */
    // WIFI_AUTH_WPA3_PSK,         /**< authenticate mode : WPA3_PSK */
    // WIFI_AUTH_WPA2_WPA3_PSK,    /**< authenticate mode : WPA2_WPA3_PSK */
    // WIFI_AUTH_WAPI_PSK,         /**< authenticate mode : WAPI_PSK */
    // WIFI_AUTH_MAX

    let securityVal;
    switch (scandata.secure) {
        case 0:
            securityVal = "open";
            break;
        case 1:
            securityVal = "WEP";
            break;
        case 2:
            securityVal = "WPA PSK";
            break;
        case 3:
            securityVal = "WPA2 PSK";
            break;
        case 4:
            securityVal = "WPA WPA2 PSK";
            break;
        case 5:
            securityVal = "WPA2 ENTERPRISE";
            break;
        case 6:
            securityVal = "WPA3 PSK";
            break;
        case 7:
            securityVal = "WPA2 WPA3 PSK";
            break;
        case 8:
            securityVal = "WAPI PSK";
            break;
        default:
            securityVal = "unknown";
            console.log(scandata.secure)
    }

    d2.innerHTML = securityVal;
    d.append(d2);


    // CSS magic from CODEPEN.IO https://codepen.io/shanker-kumar/pen/MGqwoN



    wifiSymbol = document.createElement("div");
    wifiSymbol.setAttribute("id", "wifiSym" + i);
    wifiSymbol.setAttribute("class", "wifiSymbol tableField");
    const quality = rssiQuality(scandata.rssi)

    wifiSymbol.setAttribute("title", "Signal: " + quality + "% (rssi: " + scandata.rssi + ")");
    signalstr = document.createElement("div");
    if (quality > 90) {

        signalstr.setAttribute("class", "waveStrength waveStrength-4");
        wifiSymbol.setAttribute("class", "waveStrength waveStrength-4");
    } else if (quality > 70) {
        signalstr.setAttribute("class", "waveStrength waveStrength-3");
        wifiSymbol.setAttribute("class", "waveStrength waveStrength-3");
    } else if (quality > 40) {
        signalstr.setAttribute("class", "waveStrength waveStrength-2");
        wifiSymbol.setAttribute("class", "waveStrength waveStrength-2");
    } else if (quality > 10) {
        signalstr.setAttribute("class", "waveStrength waveStrength-1");
        wifiSymbol.setAttribute("class", "waveStrength waveStrength-1");
    } else {
        signalstr.setAttribute("class", "waveStrength waveStrength-0");
        wifiSymbol.setAttribute("class", "waveStrength waveStrength-0");
    }
    w0 = document.createElement("div");
    w0.setAttribute("class", "wv0 wave");
    w0.setAttribute("style", "color:transparent");

    w1 = document.createElement("div");
    w1.setAttribute("class", "wv1 wave");
    w1.appendChild(w0);

    w2 = document.createElement("div");
    w2.setAttribute("class", "wv2 wave");
    w2.setAttribute("style", "");
    w2.appendChild(w1);


    w3 = document.createElement("div");
    w3.setAttribute("class", "wv3 wave");
    w3.setAttribute("style", "");
    w3.appendChild(w2);


    w4 = document.createElement("div");
    w4.setAttribute("class", "wv4 wave");
    w4.setAttribute("style", "");
    w4.appendChild(w3);

    signalstr.appendChild(w4);
    wifiSymbol.append(getWifiSVG());
    d.append(wifiSymbol);


    return d;
}


function rssiQuality(rssi) {
    console.log(rssi)
    let quality = 0;

    if (rssi <= -100) {
        quality = 0;
    }
    else if (rssi >= -50) {
        quality = 100;
    }
    else {
        quality = 2 * (rssi + 100);
    }

    return quality;
}


function c(l, passwd) {
    let maxNets = document.getElementsByClassName('SSIDlabel').length;

    for (let i = 0; i < maxNets; i++) {

        if (document.getElementById("wifi_ssid" + i).value != "") {
            console.log("wifi_ssid" + i + " is defined" + document.getElementById("s" + i).value);

        } else {
            document.getElementById('wifi_ssid' + i).value = l.innerText || l.textContent;
            document.getElementById('wifi_pw' + i).value = passwd;
            document.getElementById('wifi_pw' + i).focus();
            break;
        }


    }




}

function resetSSID(i) {
    document.getElementById('s' + i).value = "";
    document.getElementById('p' + i).value = "";

}


function updatePage() {
    let nets;
    fetch(url + "configConfig")
        .then(function (response2) {
            return response2.json()
        })
        .then(function (data) {
            nets = data.maxNets;
            if (!data.mqtt) {
                // define ENABLE_MQTT is false in the main code.
                const mqtt = document.getElementById("MQTTSettings");
                while (mqtt.hasChildNodes()) {
                    mqtt.removeChild(mqtt.firstChild);
                }
            } else {
                mqttHost = document.getElementById("mqttHost");
                mqttHost.setAttribute("value", data.mqttHost);
                mqttPort = document.getElementById("mqttPort");
                mqttPort.setAttribute("value", data.mqttPort);
                mqttUser = document.getElementById("mqttUser");
                mqttUser.setAttribute("value", data.mqttUser);
                mqttPass = document.getElementById("mqttPass");
                mqttPass.setAttribute("value", data.mqttPass);
            }
            // Creates a the form in which we put the wifi networs.
            selecteNetworks = document.getElementById("selectedNetworks");


            for (let i = 0; i < data.maxNets; i++) {

                ssidline = document.createElement("div");
                ssidline.setAttribute("class", "tableRow");
                ssidfield = document.createElement("div");
                ssidfield.setAttribute("class", "tableField wifiLabel SSIDlabel");
                ls = document.createElement("label");
                ls.innerHTML = "SSID";
                ssidfield.appendChild(ls);
                ssidline.appendChild(ssidfield);
                ssidinputdiv = document.createElement("div");
                ssidinputdiv.setAttribute("class", "tableField wifiInputField");
                ip = document.createElement("input");
                ip.setAttribute("id", "wifi_ssid" + i);
                ip.setAttribute("name", "wifi_ssid" + i);

                ip.setAttribute("placeholder", "SSID" + i);
                ssidinputdiv.appendChild(ip);
                ssidline.appendChild(ssidinputdiv);

                redcross = document.createElement("div");
                redcross.setAttribute("class", "redCross tableField");
                crossimg = document.createElement("img");
                crossimg.setAttribute("width", "20px");
                crossimg.setAttribute("onClick", "resetSSID(" + i + ")");
                crossimg.setAttribute("src", "redCircleCrossed.svg");
                redcross.appendChild(crossimg)
                ssidline.appendChild(redcross);

                selecteNetworks.append(ssidline);

                passline = document.createElement("div");
                passline.setAttribute("class", "tableRow");
                passfield = document.createElement("div");
                passfield.setAttribute("class", "tableField wifiLabel");
                lp = document.createElement("label");
                lp.innerHTML = "Passwd";
                passfield.appendChild(lp);
                passline.appendChild(passfield);
                passinputdiv = document.createElement("div");
                passinputdiv.setAttribute("class", "tableField wifiInputField");
                ip = document.createElement("input");
                ip.setAttribute("id", "wifi_pw" + i);
                ip.setAttribute("name", "wifi_pw" + i);
                ip.setAttribute("placeholder", "Password" + i);
                passinputdiv.appendChild(ip);
                //empty filed

                passline.appendChild(passinputdiv);
                emptydiv = document.createElement("div");
                emptydiv.setAttribute("class", "redCross tableField");
                emptydiv.innerHTML = "&nbsp;"
                passline.appendChild(emptydiv);
                selecteNetworks.append(passline);


                //wrapper.append(redcross);
                //
                //redcross = document.createElement("div")


                // selecteNetworks.appendChild(wrapper);





            }


        })

        .catch(function (err) {
            console.log(err);
        });

}


function displayLoading(loader) {
    loader.classList.add("display");
    // to stop loading after some time
    setTimeout(() => {
        loader.classList.remove("display");
    }, 50000)
        ;
}
function hideLoading(loader) {
    loader.classList.remove("display");
}