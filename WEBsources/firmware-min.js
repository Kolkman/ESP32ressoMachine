function parseState(e){test.innerHTML="Temperature "+e.T,timeData.push(e.time),powerData.push(e.heaterPower),tempData.push(e.measuredTemperature),tempData.length>1e3&&(tempData=tempData.splice(0,1),powerData=powerData.splice(0,1),timeData=timeData.splice(0,1))}window.addEventListener("load",(function(){fetch("api/v1/firmware").then((function(e){return e.json()})).then((function(e){!function(e){document.getElementById("firmware").innerHTML="Firmware:"+e.version}(e)})).catch((function(e){}))}));