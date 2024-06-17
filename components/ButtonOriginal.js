import { StyleSheet, Text, View, Pressable } from "react-native";
import { NavigationContainer } from "@react-navigation/native";

import { turnOn, turnOff, openFrontDoor, closeFrontDoor, openGuestDoor, closeGuestDoor, openBedRoomDoor, closeBedRoomDoor } from "./SensorsCommunication";


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
      case "turnOn":
        turnOn();
        break;
      case "turnOff":
        turnOff();
        break;
      case "openFrontDoor":
        openFrontDoor();
        break;
      case "closeFrontDoor":
        closeFrontDoor();
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
