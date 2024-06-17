const ESP8266_IP = "http://192.168.3.213";

export const turnOn = async () => {
  try {
    await fetch(`${ESP8266_IP}/on`);
  } catch (error) {
    Alert.alert("Eroare la aprinderea becului");
  }
};

export const turnOff = async () => {
  try {
    await fetch(`${ESP8266_IP}/off`);
  } catch (error) {
    Alert.alert("Eroare la stingerea becului");
  }
};

export const openFrontDoor = async () => {
  try {
    await fetch(`${ESP8266_IP}/openFrontDoor`);
  } catch (error) {
    Alert.alert("Eroare la deschiderea usii principale");
  }
};

export const closeFrontDoor = async () => {
  try {
    await fetch(`${ESP8266_IP}/closeFrontDoor`);
  } catch (error) {
    Alert.alert("Eroare la inchiderea usii principale");
  }
};
export const openGuestDoor = async () => {
  try {
    await fetch(`${ESP8266_IP}/openGuestDoor`);
  } catch (error) {
    Alert.alert("Eroare la deschiderea usii principale");
  }
};

export const closeGuestDoor = async () => {
  try {
    await fetch(`${ESP8266_IP}/closeGuestDoor`);
  } catch (error) {
    Alert.alert("Eroare la inchiderea usii principale");
  }
};
export const openBedRoomDoor = async () => {
  try {
    await fetch(`${ESP8266_IP}/openBedRoomDoor`);
  } catch (error) {
    Alert.alert("Eroare la deschiderea usii bedRoom");
  }
};

export const closeBedRoomDoor = async () => {
  try {
    await fetch(`${ESP8266_IP}/closeBedRoomDoor`);
  } catch (error) {
    Alert.alert("Eroare la inchiderea usii bedRoom");
  }
};
