const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <title>Control Servo și LED-uri</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin-top: 50px;
    }
    button {
      display: inline-block;
      padding: 10px 20px;
      margin: 10px;
      font-size: 16px;
      cursor: pointer;
    }
  </style>
   <script>
    function sendRequest(url) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          console.log(this.responseText);
        }
      };
      xhttp.open("GET", url, true);
      xhttp.send();
    }

    function updateTemperatureHumidity() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          var data = JSON.parse(this.responseText);
          document.getElementById("temperature").innerText = data.temperature + " °C";
          document.getElementById("humidity").innerText = data.humidity + " %";
        }
      };
      xhttp.open("GET", "/temperature_humidity", true);
      xhttp.send();
    }

    function updateSensorsStatus() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("sensors-status").innerText = this.responseText;
        }
      };
      xhttp.open("GET", "/sensors_status", true);
      xhttp.send();
    }


    function motionStatus() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("motion-status").innerText = this.responseText;
        }
      };
      xhttp.open("GET", "/motion_status", true);
      xhttp.send();
    }


    // Actualizează temperatura, umiditatea și starea senzorilor la fiecare 5 secunde
    setInterval(function() {
      updateTemperatureHumidity();
      updateSensorsStatus();
    }, 5000);

    setInterval(function() {
      motionStatus();
    }, 2000);

function sendCustomDegrees() {
    const degrees = document.getElementById("degreesInput").value;
    if (degrees) {
        sendRequest("/moveCustomDegrees?degrees=" + degrees);
    } else {
        alert("Please enter a valid number of degrees.");
    }
}

  </script>

</head>
<body>
  <h1>Control Servo și LED-uri</h1>
  
  <h2>Temperatura și Umiditatea</h2>
  <p>Temperatura curentă: <span id="temperature">--</span></p>
  <p>Umiditatea curentă: <span id="humidity">--</span></p>
  
  <h2>Status senzori</h2>
  <p id="sensors-status">--</p>
  
  <p id="motion-status">--</p>


  <h2>Doors</h2>
  <button onclick="sendRequest('/openGuestDoor')">Deschide Guest</button>
  <button onclick="sendRequest('/closeGuestDoor')">Închidere Guest</button>
  <button onclick="sendRequest('/openFrontDoor')">Deschide front</button>
  <button onclick="sendRequest('/closeFrontDoor')">Închidere front</button>
  <button onclick="sendRequest('/openBedRoomDoor')">Deschide bedroom</button>
  <button onclick="sendRequest('/closeBedRoomDoor')">Închidere bedroom</button>
  
  <h2>Windows</h2>
  <button onclick="sendRequest('/openGuestWindow')">Deschide Guest Window</button>
  <button onclick="sendRequest('/closeGuestWindow')">Închidere Guest Window</button>
  <button onclick="sendRequest('/openLivingWindow')">Deschide Living window</button>
  <button onclick="sendRequest('/closeLivingWindow')">Închidere Living window</button>
  <button onclick="sendRequest('/openBedRoomWindow')">Deschide bedroom Window</button>
  <button onclick="sendRequest('/closeBedRoomWindow')">Închidere bedroom Window</button>
     
  <h2>LED-uri</h2>
  <button onclick="sendRequest('/garageLedOn')">Garage led</button>
  <button onclick="sendRequest('/garageLedOff')">Garage led off</button>
  <button onclick="sendRequest('/guestLedOn')">Guest Led</button>
  <button onclick="sendRequest('/guestLedOff')">Guest Led Off</button>

  <button onclick="sendRequest('/doorLeftLedOn')">Door Led</button>
  <button onclick="sendRequest('/doorLeftLedOff')">Door Led Off</button>
  <button onclick="sendRequest('/bedRoomLedOn')">BedRoom Led</button>
  <button onclick="sendRequest('/bedRoomLedOff')">BedRoom Led Off</button>

  <button onclick="sendRequest('/hallLedOn')">Hall Led On</button>
  <button onclick="sendRequest('/hallLedOff')">Hall Led Off</button>

  <button onclick="sendRequest('/livingLedOn')">Living Led On</button>
  <button onclick="sendRequest('/livingLedOff')">Living Led Off</button>

  <h2>Buzzer</h2>
  <button onclick="sendRequest('/onBuzzer')">On Buzzer</button>
  <button onclick="sendRequest('/OffBuzzer')">Off Buzzer</button>

  <h2>Stepper Motor</h2>
  <button onclick="sendRequest('/moveForward')">Move Forward 190 Degrees</button>
  <button onclick="sendRequest('/moveBackward')">Move Backward 190 Degrees</button>
<h2>Stepper Motor grade</h2>
<input type="number" id="degreesInput" placeholder="Enter degrees (positive or negative)">
<button onclick="sendCustomDegrees()">Move Custom Degrees</button>

  <h2>Control Ventilator</h2>
  <button onclick="sendRequest('/startFan')">Start Ventilator</button>
  <button onclick="sendRequest('/stopFan')">Stop ventilator</button>

</body>
</html>
)=====";