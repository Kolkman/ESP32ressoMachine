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





window.addEventListener("load", function () {
    console.log("scantable.js executing");
    updatePage();
    UpdateScanTable();

});


function UpdateScanTable() {
    console.log("Updating Scan Table");
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
            const scantable = document.getElementById("scantable");

            while (scantable.hasChildNodes()) {
                scantable.removeChild(scantable.firstChild);
            }


            for (var i = 0; i < uniqueData.length; i++) {
                scantable.append(createScanRow(uniqueData[i], i))
            };
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
            securityVal = "WPA_PSK";
            break;
        case 3:
            securityVal = "WPA2_PSK";
            break;
        case 4:
            securityVal = "WPA_WPA2_PSK";
            break;
        case 5:
            securityVal = "WPA2_ENTERPRISE";
            break;
        case 6:
            securityVal = "WPA3_PSK";
            break;
        case 7:
            securityVal = "WPA2_WPA3_PSK";
            break;
        case 8:
            securityVal = "WAPI_PSK";
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
    } else if (quality > 70) {
        signalstr.setAttribute("class", "waveStrength waveStrength-3");
    } else if (quality > 40) {
        signalstr.setAttribute("class", "waveStrength waveStrength-2");
    } else if (quality > 10) {
        signalstr.setAttribute("class", "waveStrength waveStrength-1");
    } else {
        signalstr.setAttribute("class", "waveStrength waveStrength-0");
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
    wifiSymbol.appendChild(signalstr);
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
    let maxNets = document.getElementsByClassName('wrapperblock').length;
    for (let i = 0; i < maxNets; i++) {

        if (document.getElementById("s" + i).value != "") {
            console.log("s" + i + " is defined" + document.getElementById("s" + i).value);

        } else {
            document.getElementById('s' + i).value = l.innerText || l.textContent;
            document.getElementById('p' + i).value = passwd;
            document.getElementById('p' + i).focus();
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
            if (! data.mqtt){
                // define ENABLE_MQTT is false in the main code.
                const mqtt = document.getElementById("MQTTSettings");
                while (mqtt.hasChildNodes()) {
                    mqtt.removeChild(mqtt.firstChild);
                }
            }else{
                mqttHost=document.getElementById("mqttHost");
                mqttHost.setAttribute("value",data.mqttHost);
                mqttPort=document.getElementById("mqttPort");
                mqttPort.setAttribute("value",data.mqttPort);
                mqttUser=document.getElementById("mqttUser");
                mqttUser.setAttribute("value",data.mqttUser);
                mqttPass=document.getElementById("mqttPass");
                mqttPass.setAttribute("value",data.mqttPass);
            }
            // Creates a the form in which we put the wifi networs.
            selecteNetworks = document.getElementById("selectedNetworks");


            for (let i = 0; i < data.maxNets; i++) {
                wrapper = document.createElement("fieldset");
                wrapper.setAttribute("id", "wrapper" + i);
                wrapper.setAttribute("class", "wrapperblock");
                ssidline = document.createElement("div");
                ssidline.setAttribute("class", "entryline");
                ssidfield = document.createElement("div");
                ssidfield.setAttribute("class", "nettableLabel");
                ls = document.createElement("label");
                ls.innerHTML = "SSID";
                ssidfield.appendChild(ls);
                ssidline.appendChild(ssidfield);
                ip = document.createElement("input");
                ip.setAttribute("id", "s" + i);
                ip.setAttribute("name", "s" + i);
                ip.setAttribute("placeholder", "SSID" + i);
                ssidline.appendChild(ip);


                redcross = document.createElement("div");
                redcross.setAttribute("class", "redCross");
                crossimg = document.createElement("img");
                crossimg.setAttribute("width", "20px");
                crossimg.setAttribute("onClick", "resetSSID(" + i + ")");
                crossimg.setAttribute("src", "redCircleCrossed.svg");
                ssidline.appendChild(crossimg);

                wrapper.append(ssidline);

                passline = document.createElement("div");
                passline.setAttribute("class", "entryline");
                passfield = document.createElement("div");
                passfield.setAttribute("class", "nettableLabel");
                lp = document.createElement("label");
                lp.innerHTML = "Passwd";
                passfield.appendChild(lp);
                passline.appendChild(passfield);
                ip = document.createElement("input");
                ip.setAttribute("id", "p" + i);
                ip.setAttribute("name", "p" + i);
                ip.setAttribute("placeholder", "Password" + i);
                passline.appendChild(ip);

                wrapper.append(passline);


                //wrapper.append(redcross);
                //
                //redcross = document.createElement("div")


                selecteNetworks.appendChild(wrapper);





            }
           
  
        })

        .catch(function (err) {
            console.log(err);
        });

}