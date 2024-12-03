import { StyleSheet, Text, View, Pressable } from "react-native";

import {
  turnOnDoorLed,
  turnOffDoorLed,
  openFrontDoor,
  closeFrontDoor,
  openGuestDoor,
  closeGuestDoor,
  openBedRoomDoor,
  closeBedRoomDoor,
  turnOnGarageLed,
  turnOffGarageLed,
  openGarageGate,
  closeGarageGate,
  turnOffHallLed,
  turnOnHallLed,
  turnOnLivingLed,
  turnOffLivingLed,
  bedRoomSlider,
  guestSlider,
  openGuestWindow,
  closeGuestWindow,
  openBedRoomtWindow,
  closeBedRoomWindow,
  openLivingWindow,
  closeLivingWindow,
  openLivingFan,
  closeLivingFan,
  resetESP,
} from "./SensorsCommunication";

export default function ButtonOriginal({
  navigation,
  name,
  link = "",
  action,
}) {
  function pressHandlerOutside() {
    console.log(action);
    // console.log(link.length);
    link.length !== 0 ? navigation.navigate(link) : "";

    switch (action) {
      case "turnOnDoorLed":
        turnOnDoorLed();
        break;
      case "turnOffDoorLed":
        turnOffDoorLed();
        break;
      case "turnOnGarageLed":
        turnOnGarageLed();
        break;
      case "turnOffGarageLed":
        turnOffGarageLed();
        break;
      case "turnOnHallLed":
        turnOnHallLed();
        break;
      case "turnOffHallLed":
        turnOffHallLed();
        break;
      case "turnOnLivingLed":
        turnOnLivingLed();
        break;
      case "turnOffLivingLed":
        turnOffLivingLed();
        break;
      case "turnOnBedRoomLed":
        // turnOnBedroomLed();
        bedRoomSlider(100);
        break;
      case "turnOffBedRoomLed":
        // turnOffBedroomLed();
        bedRoomSlider(0);
        break;

      case "turnOnGuestLed":
        // turnOnGuestLed();
        guestSlider(100);
        break;
      case "turnOffGuestLed":
        // turnOffGuestLed();
        guestSlider(0);

        break;

      //Doors
      case "openFrontDoor":
        openFrontDoor();
        break;
      case "closeFrontDoor":
        closeFrontDoor();
        break;
      case "openGarageGate":
        openGarageGate();
        break;
      case "closeGarageGate":
        closeGarageGate();
        break;

      case "openGuestDoor":
        openGuestDoor();
        break;
      case "closeGuestDoor":
        closeGuestDoor();
        break;
      case "openBedRoomDoor":
        openBedRoomDoor();
        break;
      case "closeBedRoomDoor":
        closeBedRoomDoor();
        break;

      //Windows
      case "openGuestWindow":
        openGuestWindow();
        break;
      case "closeGuestWindow":
        closeGuestWindow();
        break;
      case "openBedRoomWindow":
        openBedRoomtWindow();
        break;
      case "closeBedRoomWindow":
        closeBedRoomWindow();
        break;
      case "openLivingWindow":
        openLivingWindow();
        break;
      case "closeLivingWindow":
        closeLivingWindow();
        break;
      case "openLivingFan":
        openLivingFan();
        break;
      case "closeLivingFan":
        closeLivingFan();
        break;
      case "resetESP":
        resetESP();
        break;
      default:
        console.log("ai gresit comanda sau nu a fost inca initializata");
    }
  }

  return (
    <Pressable
      onPress={pressHandlerOutside}
      style={({ pressed }) => (pressed ? [styles.pressed] : "")}
    >
      <Text style={styles.button}>{name}</Text>
    </Pressable>
  );
}

const styles = StyleSheet.create({
  button: {
    fontSize: 30,
    color: "white",
  },

  pressed: {
    opacity: 0.3,
  },
});
