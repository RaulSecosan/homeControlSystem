// const char MAIN_page[] PROGMEM = R"=====(
// <html>
//   <body>
//     <h1>Control BEC</h1>
//     <a href="/on">ON</a> |
//     <a href="/off">OFF</a>
//     <a href="/openFrontDoor">Open front door</a>
//     <a href="/closeFrontDoor">Close front door</a>
//   </body>
// </html>
// )=====";


const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <title>Control Servomotor</title>
  <script>
    function setServo(servoId) {
      var angle = document.getElementById(servoId).value;
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/setServo?servo=" + servoId + "&angle=" + angle , true);
      xhttp.send();
    }

    function sendRequest(url) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", url, true);
      xhttp.send();
    }
  </script>
</head>
<body>
  <h1>Control Servomotor</h1>
  <label for="bedRoom">BedRoom (0-180):</label>
  <input type="number" id="bedRoom" name="bedRoom" min="0" max="180">
  <button onclick="setServo('bedRoom')">Setează BedRoom</button>
  <br>
  <label for="frontDoor">FrontDoor (0-180):</label>
  <input type="number" id="frontDoor" name="frontDoor" min="0" max="180">
  <button onclick="setServo('frontDoor')">Setează FrontDoor</button>
  <br>
  <label for="guestDoor">GuestDoor (0-180):</label>
  <input type="number" id="guestDoor" name="guestDoor" min="0" max="180">
  <button onclick="setServo('guestDoor')">Setează GuestDoor</button>
  <hr>
  <button onclick="location.href='/on'">Aprinde LED-ul</button>
  <button onclick="location.href='/off'">Stinge LED-ul</button>
  <button onclick="sendRequest('/openBedRoomDoor')">Deschide BedRoom Door</button>
  <button onclick="sendRequest('/closeBedRoomDoor')">Închide BedRoom Door</button>
  <button onclick="sendRequest('/openGuestDoor')">Deschide Guest Door</button>
  <button onclick="sendRequest('/closeGuestDoor')">Închide Guest Door</button>
  <button onclick="sendRequest('/openFrontDoor')">Deschide Front Door</button>
  <button onclick="sendRequest('/closeFrontDoor')">Închide Front Door</button>
</body>
</html>

)=====";

