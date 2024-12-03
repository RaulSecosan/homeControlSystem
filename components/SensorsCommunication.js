import { ref, set } from "firebase/database";
import { database } from "./firebase";
import { Alert } from "react-native";

// LED Control Functions
export const turnOnDoorLed = async () => {
  try {
    await set(ref(database, "/led/doorLed"), "on");
  } catch (error) {
    Alert.alert("Eroare la aprinderea becului de la intrare");
  }
};

export const turnOffDoorLed = async () => {
  try {
    await set(ref(database, "/led/doorLed"), "off");
  } catch (error) {
    Alert.alert("Eroare la stingerea becului de la intrare");
  }
};

export const turnOnGarageLed = async () => {
  try {
    await set(ref(database, "/led/garageLed"), "on");
  } catch (error) {
    Alert.alert("Eroare la aprinderea becului de la garaj");
  }
};

export const turnOffGarageLed = async () => {
  try {
    await set(ref(database, "/led/garageLed"), "off");
  } catch (error) {
    Alert.alert("Eroare la stingerea becului de la garaj");
  }
};

export const turnOnGuestLed = async () => {
  try {
    await set(ref(database, "/led/guestLed"), "on");
  } catch (error) {
    Alert.alert("Eroare la aprinderea becului de la oaspeți");
  }
};

export const turnOffGuestLed = async () => {
  try {
    await set(ref(database, "/led/guestLed"), "off");
  } catch (error) {
    Alert.alert("Eroare la stingerea becului de la oaspeți");
  }
};

export const turnOnBedroomLed = async () => {
  try {
    await set(ref(database, "/led/bedRoomLed"), "on");
  } catch (error) {
    Alert.alert("Eroare la aprinderea becului de la dormitor");
  }
};

export const turnOffBedroomLed = async () => {
  try {
    await set(ref(database, "/led/bedRoomLed"), "off");
  } catch (error) {
    Alert.alert("Eroare la stingerea becului de la dormitor");
  }
};

export const turnOnLivingLed = async () => {
  try {
    await set(ref(database, "/led/livingLed"), "on");
  } catch (error) {
    Alert.alert("Eroare la aprinderea becului de la dormitor");
  }
};

export const turnOffLivingLed = async () => {
  try {
    await set(ref(database, "/led/livingLed"), "off");
  } catch (error) {
    Alert.alert("Eroare la stingerea becului de la dormitor");
  }
};

export const turnOnHallLed = async () => {
  try {
    await set(ref(database, "/led/hallLed"), "on");
  } catch (error) {
    Alert.alert("Eroare la aprinderea becului de la dormitor");
  }
};

export const turnOffHallLed = async () => {
  try {
    await set(ref(database, "/led/hallLed"), "off");
  } catch (error) {
    Alert.alert("Eroare la stingerea becului de la dormitor");
  }
};

//Sliders
export const bedRoomSlider = async (value) => {
  try {
    await set(ref(database, "/led/bedRoom/ledDim"), value);
  } catch (error) {
    Alert.alert(
      "Eroare",
      "Nu s-a putut actualiza valoarea slider-ului în Firebase"
    );
  }
};

export const guestSlider = async (value) => {
  try {
    await set(ref(database, "/led/guest/ledDim"), value);
  } catch (error) {
    Alert.alert(
      "Eroare",
      "Nu s-a putut actualiza valoarea slider-ului în Firebase"
    );
  }
};

// Door Control Functions
export const openFrontDoor = async () => {
  try {
    await set(ref(database, "/door/frontDoor"), "open");
  } catch (error) {
    Alert.alert("Eroare la deschiderea ușii principale");
  }
};

export const closeFrontDoor = async () => {
  try {
    await set(ref(database, "/door/frontDoor"), "close");
  } catch (error) {
    Alert.alert("Eroare la închiderea ușii principale");
  }
};

export const openGarageGate = async () => {
  try {
    await set(ref(database, "/door/garage"), "open");
  } catch (error) {
    Alert.alert("Eroare la deschiderea ușii principale");
  }
};

export const closeGarageGate = async () => {
  try {
    await set(ref(database, "/door/garage"), "close");
  } catch (error) {
    Alert.alert("Eroare la închiderea ușii principale");
  }
};

export const openGuestDoor = async () => {
  try {
    await set(ref(database, "/door/guestDoor"), "open");
  } catch (error) {
    Alert.alert("Eroare la deschiderea ușii pentru oaspeți");
  }
};

export const closeGuestDoor = async () => {
  try {
    await set(ref(database, "/door/guestDoor"), "close");
  } catch (error) {
    Alert.alert("Eroare la închiderea ușii pentru oaspeți");
  }
};

export const openBedRoomDoor = async () => {
  try {
    await set(ref(database, "/door/bedRoomDoor"), "open");
  } catch (error) {
    Alert.alert("Eroare la deschiderea ușii bedRoom");
  }
};

export const closeBedRoomDoor = async () => {
  try {
    await set(ref(database, "/door/bedRoomDoor"), "close");
  } catch (error) {
    Alert.alert("Eroare la închiderea ușii bedRoom");
  }
};

//Fan
export const openLivingFan = async () => {
  try {
    await set(ref(database, "/motor/fan"), "start");
  } catch (error) {
    Alert.alert("Eroare la deschiderea ușii bedRoom");
  }
};

export const closeLivingFan = async () => {
  try {
    await set(ref(database, "/motor/fan"), "stop");
  } catch (error) {
    Alert.alert("Eroare la închiderea ușii bedRoom");
  }
};

// Window Control Functions
export const openGuestWindow = async () => {
  try {
    await set(ref(database, "/window/guestWindow"), "open");
  } catch (error) {
    Alert.alert("Eroare la deschiderea ferestrei pentru oaspeți");
  }
};

export const closeGuestWindow = async () => {
  try {
    await set(ref(database, "/window/guestWindow"), "close");
  } catch (error) {
    Alert.alert("Eroare la închiderea ferestrei pentru oaspeți");
  }
};

export const openBedRoomtWindow = async () => {
  try {
    await set(ref(database, "/window/bedRoomWindow"), "open");
  } catch (error) {
    Alert.alert("Eroare la deschiderea ferestrei din bedRoom");
  }
};

export const closeBedRoomWindow = async () => {
  try {
    await set(ref(database, "/window/bedRoomWindow"), "close");
  } catch (error) {
    Alert.alert("Eroare la închiderea ferestrei din bedRoom");
  }
};

export const openLivingWindow = async () => {
  try {
    await set(ref(database, "/window/livingWindow"), "open");
  } catch (error) {
    Alert.alert("Eroare la deschiderea ferestrei din bedRoom");
  }
};

export const closeLivingWindow = async () => {
  try {
    await set(ref(database, "/window/livingWindow"), "close");
  } catch (error) {
    Alert.alert("Eroare la închiderea ferestrei din bedRoom");
  }
};
// Motor Control Functions
export const startFan = async () => {
  try {
    await set(ref(database, "/motor/fan"), "start");
  } catch (error) {
    Alert.alert("Eroare la pornirea ventilatorului");
  }
};

export const stopFan = async () => {
  try {
    await set(ref(database, "/motor/fan"), "stop");
  } catch (error) {
    Alert.alert("Eroare la oprirea ventilatorului");
  }
};

// Function to control garage door using stepper motor
export const moveForward = async () => {
  try {
    await set(ref(database, "/door/garage"), "forward");
  } catch (error) {
    Alert.alert("Eroare la deschiderea ușii de la garaj");
  }
};

export const moveBackward = async () => {
  try {
    await set(ref(database, "/door/garage"), "backward");
  } catch (error) {
    Alert.alert("Eroare la închiderea ușii de la garaj");
  }
};

export const resetESP = async () => {
  try {
    await set(ref(database, "/statusESP8266/reset"), "reset");
  } catch (error) {
    Alert.alert("Eroare la transmiterea comenzii pentru resetare");
  }
};
