import { StyleSheet, Text, View, Pressable } from "react-native";
import { NavigationContainer } from "@react-navigation/native";

import { turnOn } from "./SensorsCommunication";
import { turnOff } from "./SensorsCommunication";
import { openFrontDoor } from "./SensorsCommunication";
import { closeFrontDoor } from "./SensorsCommunication";

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
      default:
        console.log("ai gresit comanda sau nu a fost inca initializata");
    }
  }

  return (
    <Pressable
      onPress={pressHandlerOutside}
      style={({ pressed }) => (pressed ? [styles.pressed] : "")}
    >
      <View style={styles.buttonBox}>
        <Text style={styles.button}>{name}</Text>
      </View>
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
