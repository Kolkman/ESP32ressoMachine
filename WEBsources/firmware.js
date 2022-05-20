function parseState(data) {
    var dataLength = 1000;
    test.innerHTML = "Temperature " + data.T;
    timeData.push(data.time);
    powerData.push(data.heaterPower);
    tempData.push(data.measuredTemperature);
    if (tempData.length > dataLength) {
        tempData = tempData.splice(0, 1);
        powerData = powerData.splice(0, 1);
        timeData = timeData.splice(0, 1);
    }
}


window.addEventListener("load", function () {
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
    function parseFirmware(data) {
        var divFirmware = document.getElementById("firmware")
        divFirmware.innerHTML = "Firmware:" + data.version;
    }
}
);