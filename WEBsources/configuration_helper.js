
// Global Variables.
const configNames = ["tset", "tband", "eqPwr", "pgain", "igain", "dgain",
    "apgain", "aigain", "adgain", "PidInterval", "HeaterInterval",
    "sensorSampleInterval", "maxCool", "powerOffMode"];

const tuningNames = ["tunethres", "tunestep", "tuningOn"]
let configuration = {};
let configerror = false;

// The Eventlistner that populates
// the configuration page after load
window.addEventListener("load", function () {
    fetch(url + '/api/v1/authenticated')
        .then(function (response) {

            return response.json()
        })
        .then(function (data) {
            console.log(data);
                    loginRequired=  document.getElementById("LoginRequired");
                    PidConfigParamsForm = document.getElementById("PidConfigParamsForm");
                    configAPI = document.getElementById("configAPI");
                    tuningForm = document.getElementById("tuningForm");
                    console.log(data["authenticated"]);
                    if (data["authenticated"]){
                        loginRequired.style.display="none";
                        configAPI.style.display="block";
                        PidConfigParamsForm.style.display="inline-block";
                        tuningForm.style.display="block";
                    }else{
                        loginRequired.style.display="block";
                        configAPI.style.display="none";
                        PidConfigParamsForm.style.display="none";
                        tuningForm.style.display="none";
                    }
        
        })
        .catch(function (err) {
            console.log('error' + err);
        });


    setFormValues();
    configForm.addEventListener('submit', (event) => {

        console.log("setFormValues")
        event.preventDefault(); //stop form submission
        // chec on all input
        configerror = false;
        let argObj = { form: configForm, errordiv: 'confform-errormessage' };
        configNames.forEach(GetFormValues, argObj);
        Errordiv = document.getElementById("confform-errormessage");
     
        if (!configerror) {
            param = new URLSearchParams(configuration);
            let setapi = url + "/api/v1/set?" + param.toString();
            console.log(setapi);
                fetch(setapi)
                .then(function (response) {
                    return response.json()
                })
                .then(function (data) {
                    console.log(data["authenticated"]);
                    if (data['authenticated']==false) {
                        console.log("We were not authorized");
                        Errordiv.textContent = "Not Authorized";
                       
                    } else {
                        console.log("We are authorized");
                        setFormDefaults(data, configForm, configNames);
                        Errordiv.textContent = "New Values Set"
                    }
                })
                .catch(function (err) {
                    console.log(err);
                });

            setTimeout(function () {
                Errordiv = document.getElementById("confform-errormessage");
                Errordiv.textContent = ""
            }, 3000);
        }
    }, false);
    tuningForm.addEventListener('submit', (event) => {
        configerror = false;
        event.preventDefault(); //stop form submission
        let argObj = { form: tuningForm, errordiv: 'tuneform-errormessage' };
        tuningNames.forEach(GetFormValues, argObj);
        if (!configerror) {
            param = new URLSearchParams(configuration);
            let setapi = url + "/api/v1/set?" + param.toString();
            fetch(setapi)
                .then(function (response) {
                    return response.json()
                })
                .then(function (data) {
                    setFormDefaults(data, tuningForm, tuningNames);
                })
                .catch(function (err) {
                    console.log(err);
                });
            Errordiv = document.getElementById("tuneform-errormessage");
            Errordiv.textContent = "New Values Set[2]"
        }
    }, false);
}); // End of window.addEventListener


// Power Switch Script 
function pwrSwitch() {
    if (document.getElementById("toggle--pwr").checked) {
        // (Heater is OFF)
        fetch(url + '/api/v1/pwr?powerOffMode=false');
    } else {
        // .(HEATER is ON)
        fetch(url + '/api/v1/pwr?powerOffMode=true');

    }
}

// Tuning Switch Script 
function tnrSwitch() {
    if (document.getElementById("toggle--tuner").checked) {
        // (Tuning is OFF)
        fetch(url + '/api/v1/set?tuningOn=true')
        updateTuningStats();
    } else {
        // .(Tuning is ON)
        fetch(url + '/api/v1/set?tuningOn=false');
        fetch(url + '/api/v1/get?pgain&igain&dgain')
            .then(function (response) {

                return response.json()
            })
            .then(function (data) {
                let statsdiv = document.getElementById("StatsReporting");
                statsdiv.innerHTML = "";
                let d1 = document.createElement("div");
                let d2 = document.createElement("div");
                let d3 = document.createElement("div");
                d1.textContent = "New P: " + data.pgain
                statsdiv.append(d1);
                d2.textContent = "New I: " + data.igain
                statsdiv.append(d2);
                d3.textContent = "New D: " + data.dgain
                statsdiv.append(d3);
                setFormValues();
            })
            .catch(function (err) {
                console.log('error' + err);
            });
    }
}

function updateTuningStats() {
    fetch(url + "/api/v1/get?tuningOn&tunestep&tunethres&tuneCount&timeElapsed&averagePeriod&lowerAverage&upperAverage")
        .then(function (response) {
            return response.json()
        })
        .then(function (data) {
            if (data.tuningOn) {
                let statsdiv = document.getElementById("StatsReporting");
                statsdiv.innerHTML = "";
                let d1 = document.createElement("div");
                let d2 = document.createElement("div");
                let d3 = document.createElement("div");
                let d4 = document.createElement("div");
                let d5 = document.createElement("div");
                d4.textContent = "timeElapsed: " + data.timeElapsed
                statsdiv.append(d4);
                d5.textContent = "tuneCount: " + data.tuneCount
                statsdiv.append(d5);
                d1.textContent = "averagePeriod: " + data.averagePeriod.toPrecision(3)
                statsdiv.append(d1);
                d2.textContent = "lowerAverage: " + data.lowerAverage.toPrecision(3)
                statsdiv.append(d2);
                d3.textContent = "upperAverage: " + data.upperAverage.toPrecision(3)
                statsdiv.append(d3);
                setTimeout(function () { updateTuningStats(); }, 5000);
            }
        })
        .catch(function (err) {
            console.log(err);
        });
}

// Configuration file Helper functions
function LoadConf() {
    const configForm = document.getElementById('configForm');
    let configdiv = document.getElementById("ConfigReporting");
    fetch(url + "/api/v1/config?load")
        .then(function (response) {
            return response.json()
        })
        .then(function (data) {
            if (data.load) {
                configdiv.innerHTML = "Config loaded from file"
                setFormValues();
            } else {
                configdiv.innerHTML = "Failed Loading config"
            }
        });

}
function SaveConf() {
    const configForm = document.getElementById('configForm');
    let configdiv = document.getElementById("ConfigReporting");
    fetch(url + "/api/v1/config?save")
        .then(function (response) {
            return response.json()
        })
        .then(function (data) {
            if (data.save) {
                configdiv.innerHTML = "Config saved to file"
            } else {
                configdiv.innerHTML = "Failed saving config"
            }
        });
}

function DefaultConf() {
    const configForm = document.getElementById('configForm');
    let configdiv = document.getElementById("ConfigReporting");
    fetch(url + "/api/v1/config?default")
        .then(function (response) {
            return response.json()
        })
        .then(function (data) {
            if (data.default) {
                configdiv.innerHTML = "Config reset to defaults"
                setFormValues();
            } else {
                configdiv.innerHTML = "Failed setting defaults"
            }

        });
}

// Set's the content of the Form to its default velues
function setFormValues() {
    const configForm = document.getElementById('configForm');
    const tuningForm = document.getElementById('tuningForm');
    let getapi = url + "/api/v1/get?"
    fieldnames = configNames.concat(tuningNames);
    configNames.concat(tuningNames).forEach(ConstructGetAPI);
    function ConstructGetAPI(value) {
        getapi += value + "&";
    }
    getapi = getapi.slice(0, -1); //strim extraneous &
    fetch(getapi)
        .then(function (response) {
            return response.json()
        })
        .then(function (data) {
            setFormDefaults(data, configForm, configNames);
            setFormDefaults(data, tuningForm, tuningNames);
        })
        .catch(function (err) {
            console.log('error' + err);
        });
}

// Helper function for the setFormValues, does the actual work
function setFormDefaults(data, form, fieldnames) {
    fieldnames.forEach(setFormValues);
    function setFormValues(value) {
        // First handle special values
        if (value == "powerOffMode") {
            let pidModeSwitch = document.getElementById("toggle--pwr");
            if (data.powerOffMode) {
                pidModeSwitch.checked = false;
            } else {
                pidModeSwitch.checked = true;
            }
        } else if (value == "tuningOn") {
            let tunerSwitch = document.getElementById("toggle--tuner");
            if (data.tuningOn) {
                tunerSwitch.checked = true;
                updateTuningStats();
            } else {
                tunerSwitch.checked = false;
            }
        }
        else {
            if (typeof data[value] !== 'undefined') {

                form.elements[value].value = data[value];
                //reset the colors
                form.elements[value].style.backgroundColor = "cornsilk";
                form.elements[value].addEventListener('click', function onClick(event) {
                    form.elements[value].style.backgroundColor = "#E0BFB8";
                });

            } else {
                console.log("value not found: " + value)
            }
        }
    }
    // some final touch ups.
    if (form == configForm) {
        trangemax = document.getElementById("trangemax");
        trangemin = document.getElementById("trangemin");
        trangemin.innerHTML = data.tset - data.tband
        trangemax.innerHTML = data.tset + data.tband
    }


};

// Get's the submitted values from the form and 
// sets those in the configurateion.
function GetFormValues(value) { // NB thisArg sets this with the forEach() function
    Errordiv = document.getElementById(this['errordiv']);
    if (value == "powerOffMode") return;
    if (value == "tuningOn") return;
    const e = this['form'].elements[value];
    let n = parseFloat(e.value);
    if (isNaN(n)) {
        Errordiv.textContent = "Error " + value + " should be a number (" + n + ")";
        e.style.backgroundColor = "red";
        configerror = true;
        return
    }
    if (n < 0) {
        Errordiv.textContent = "Error: " + value + " should be positive";
        e.style.backgroundColor = "red";
        configerror = true;
        return
    }

    if (value === "tset" && n > 110) {
        Errordiv.textContent = "Warning: tset set to maximum allowed of 110";
        e.style.backgroundColor = "orange";
        e.value = 110;
        configuration["tset"] = 110;
    }
    configuration[value] = n;
};
