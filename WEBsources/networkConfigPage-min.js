function getWifiSVG(){const e=document.createElement("template");return e.innerHTML=' <svg width="100%" height="100%" class="wifiRaw" viewBox="0 0 39 26" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" xml:space="preserve" xmlns:serif="http://www.serif.com/" style="fill-rule:evenodd;clip-rule:evenodd;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:1.5;">     <g transform="matrix(1,0,0,1,-18.1604,-42.0371)">         <g id="w1" transform="matrix(0.00112589,1.0194,-1.0194,0.00112589,98.065,25.8788)">             <path d="M34.669,55.467C33.151,56.343 32.216,57.962 32.216,59.715C32.216,61.467 33.151,63.086 34.669,63.962" style="fill:rgb(,0,0);fill-opacity:0;stroke:rgb(0,0,0);stroke-width:3.43px;"/>         </g>         <g id="w2" transform="matrix(0.00225177,2.0388,-2.0388,0.00225177,158.899,-12.0296)">             <path d="M34.669,55.467C33.151,56.343 32.216,57.962 32.216,59.715C32.216,61.467 33.151,63.086 34.669,63.962" style="fill:rgb(0,0,0);fill-opacity:0;stroke:rgb(0,0,0);stroke-width:1.72px;"/>         </g>         <g id="w3" transform="matrix(0.00337766,3.0582,-3.0582,0.00337766,219.733,-49.9379)">             <path d="M34.669,55.467C33.151,56.343 32.216,57.962 32.216,59.715C32.216,61.467 33.151,63.086 34.669,63.962" style="fill:rgb(0,0,0);fill-opacity:0;stroke:rgb(0,0,0);stroke-width:1.14px;"/>         </g>         <g id="w4" transform="matrix(0.00450354,4.0776,-4.0776,0.00450354,280.567,-87.8462)">             <path d="M34.669,55.467C33.151,56.343 32.216,57.962 32.216,59.715C32.216,61.467 33.151,63.086 34.669,63.962" style="fill:rgb(0,0,0);fill-opacity:0;stroke:rgb(0,0,0);stroke-width:0.86px;"/>         </g>         <g id="w0" transform="matrix(2.40204,0,0,2.40204,-58.4881,-101.337)">             <circle cx="39.849" cy="69.11" r="1.01" style="fill:rgb(0,0,0);"/>         </g>     </g> </svg>',e.content}function UpdateScanTable(){const e=document.querySelector("#loading");displayLoading(e);const t=document.getElementById("scantable");for(t.setAttribute("class","tableTable");t.hasChildNodes();)t.removeChild(t.firstChild);fetch(url+"scan").then((function(e){return e.json()})).then((function(s){let i=[];s.sort((function(e,t){e.rssi,t.rssi}));let l=[];s.forEach((e=>{l.includes(e.ssid)||(l.push(e.ssid),i.push(e))}));let n=document.createElement("div");n.setAttribute("class","tableBody");for(var a=0;a<i.length;a++)n.append(createScanRow(i[a],a));t.append(n),hideLoading(e)})).catch((function(e){}))}function createScanRow(e,t){let s=document.createElement("div");s.setAttribute("id","networkRow"+t),s.setAttribute("class","networkSelection tableRow");let i=document.createElement("div");i.setAttribute("id","SSID"+t),i.setAttribute("class","SSIDField tableField");let l=document.createElement("a");l.setAttribute("href","#p"),l.setAttribute("onClick",'c(this,"'+e.pass+'")'),l.innerHTML=e.ssid,i.appendChild(l),s.append(i);let n,a=document.createElement("div");switch(a.setAttribute("id","securityVal"+t),a.setAttribute("class","SecurityField tableField"),e.secure){case 0:n="open";break;case 1:n="WEP";break;case 2:n="WPA PSK";break;case 3:n="WPA2 PSK";break;case 4:n="WPA WPA2 PSK";break;case 5:n="WPA2 ENTERPRISE";break;case 6:n="WPA3 PSK";break;case 7:n="WPA2 WPA3 PSK";break;case 8:n="WAPI PSK";break;default:n="unknown"}a.innerHTML=n,s.append(a),wifiSymbol=document.createElement("div"),wifiSymbol.setAttribute("id","wifiSym"+t),wifiSymbol.setAttribute("class","wifiSymbol tableField");const r=rssiQuality(e.rssi);return wifiSymbol.setAttribute("title","Signal: "+r+"% (rssi: "+e.rssi+")"),signalstr=document.createElement("div"),r>90?(signalstr.setAttribute("class","waveStrength waveStrength-4"),wifiSymbol.setAttribute("class","waveStrength waveStrength-4")):r>70?(signalstr.setAttribute("class","waveStrength waveStrength-3"),wifiSymbol.setAttribute("class","waveStrength waveStrength-3")):r>40?(signalstr.setAttribute("class","waveStrength waveStrength-2"),wifiSymbol.setAttribute("class","waveStrength waveStrength-2")):r>10?(signalstr.setAttribute("class","waveStrength waveStrength-1"),wifiSymbol.setAttribute("class","waveStrength waveStrength-1")):(signalstr.setAttribute("class","waveStrength waveStrength-0"),wifiSymbol.setAttribute("class","waveStrength waveStrength-0")),w0=document.createElement("div"),w0.setAttribute("class","wv0 wave"),w0.setAttribute("style","color:transparent"),w1=document.createElement("div"),w1.setAttribute("class","wv1 wave"),w1.appendChild(w0),w2=document.createElement("div"),w2.setAttribute("class","wv2 wave"),w2.setAttribute("style",""),w2.appendChild(w1),w3=document.createElement("div"),w3.setAttribute("class","wv3 wave"),w3.setAttribute("style",""),w3.appendChild(w2),w4=document.createElement("div"),w4.setAttribute("class","wv4 wave"),w4.setAttribute("style",""),w4.appendChild(w3),signalstr.appendChild(w4),wifiSymbol.append(getWifiSVG()),s.append(wifiSymbol),s}function rssiQuality(e){let t=0;return t=e<=-100?0:e>=-50?100:2*(e+100),t}function c(e,t){let s=document.getElementsByClassName("SSIDlabel").length;for(let i=0;i<s;i++)if(""==document.getElementById("s"+i).value){document.getElementById("s"+i).value=e.innerText||e.textContent,document.getElementById("p"+i).value=t,document.getElementById("p"+i).focus();break}}function resetSSID(e){document.getElementById("s"+e).value="",document.getElementById("p"+e).value=""}function updatePage(){let e;fetch(url+"configConfig").then((function(e){return e.json()})).then((function(t){if(e=t.maxNets,t.mqtt)mqttHost=document.getElementById("mqttHost"),mqttHost.setAttribute("value",t.mqttHost),mqttPort=document.getElementById("mqttPort"),mqttPort.setAttribute("value",t.mqttPort),mqttUser=document.getElementById("mqttUser"),mqttUser.setAttribute("value",t.mqttUser),mqttPass=document.getElementById("mqttPass"),mqttPass.setAttribute("value",t.mqttPass);else{const e=document.getElementById("MQTTSettings");for(;e.hasChildNodes();)e.removeChild(e.firstChild)}selecteNetworks=document.getElementById("selectedNetworks");for(let e=0;e<t.maxNets;e++)ssidline=document.createElement("div"),ssidline.setAttribute("class","tableRow"),ssidfield=document.createElement("div"),ssidfield.setAttribute("class","tableField wifiLabel SSIDlabel"),ls=document.createElement("label"),ls.innerHTML="SSID",ssidfield.appendChild(ls),ssidline.appendChild(ssidfield),ssidinputdiv=document.createElement("div"),ssidinputdiv.setAttribute("class","tableField wifiInputField"),ip=document.createElement("input"),ip.setAttribute("id","s"+e),ip.setAttribute("name","s"+e),ip.setAttribute("placeholder","SSID"+e),ssidinputdiv.appendChild(ip),ssidline.appendChild(ssidinputdiv),redcross=document.createElement("div"),redcross.setAttribute("class","redCross tableField"),crossimg=document.createElement("img"),crossimg.setAttribute("width","20px"),crossimg.setAttribute("onClick","resetSSID("+e+")"),crossimg.setAttribute("src","redCircleCrossed.svg"),redcross.appendChild(crossimg),ssidline.appendChild(redcross),selecteNetworks.append(ssidline),passline=document.createElement("div"),passline.setAttribute("class","tableRow"),passfield=document.createElement("div"),passfield.setAttribute("class","tableField wifiLabel"),lp=document.createElement("label"),lp.innerHTML="Passwd",passfield.appendChild(lp),passline.appendChild(passfield),passinputdiv=document.createElement("div"),passinputdiv.setAttribute("class","tableField wifiInputField"),ip=document.createElement("input"),ip.setAttribute("id","p"+e),ip.setAttribute("name","p"+e),ip.setAttribute("placeholder","Password"+e),passinputdiv.appendChild(ip),passline.appendChild(passinputdiv),emptydiv=document.createElement("div"),emptydiv.setAttribute("class","redCross tableField"),emptydiv.innerHTML="&nbsp;",passline.appendChild(emptydiv),selecteNetworks.append(passline)})).catch((function(e){}))}function displayLoading(e){e.classList.add("display"),setTimeout((()=>{e.classList.remove("display")}),5e4)}function hideLoading(e){e.classList.remove("display")}window.addEventListener("load",(function(){updatePage(),UpdateScanTable()}));