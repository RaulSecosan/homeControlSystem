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
  </script>
</head>
<body>
  <h1>Control Servo și LED-uri</h1>
  <h2>Servo Motor</h2>
  <button onclick="sendRequest('/openGuestDoor')">Deschide Guest</button>
  <button onclick="sendRequest('/closeGuestDoor')">Închidere Guest</button>
  <button onclick="sendRequest('/openFrontDoor')">Deschide front</button>
  <button onclick="sendRequest('/closeFrontDoor')">Închidere front</button>
  <button onclick="sendRequest('/openBedRoomDoor')">Deschide bedroom</button>
  <button onclick="sendRequest('/closeBedRoomDoor')">Închidere bedroom</button>
  
  <h2>LED-uri</h2>
  <button onclick="sendRequest('/firstLedOn')">Aprinde led 1</button>
  <button onclick="sendRequest('/firstLedOff')">Stinge LED 1</button>
  <button onclick="sendRequest('/on')">Aprinde LED QH</button>
  <button onclick="sendRequest('/off')">Stinge LED QH</button>

  <h2>Stepper Motor</h2>
  <button onclick="sendRequest('/moveForward')">Move Forward 190 Degrees</button>
  <button onclick="sendRequest('/moveBackward')">Move Backward 190 Degrees</button>
</body>
</html>
)=====";
