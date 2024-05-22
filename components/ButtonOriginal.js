import { StyleSheet, Text, View, Pressable } from "react-native";
import { NavigationContainer } from "@react-navigation/native";

import { turnOn } from "./SensorsCommunication";
import { turnOff } from "./SensorsCommunication";

export default function ButtonOriginal({
  navigation,
  name,
  link = "",
  action,
}) {
  function pressHandlerOutside() {
    console.log(action);
    // action === 'turnOn' ? turnOn() :  turnOff() ;
    if (action === "turnOn") return turnOn();
    else if (action === "turnOff") return turnOff();
    // console.log(link.length);
    link.length !== 0 ? navigation.navigate(link) : "";
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
    opacity: 0.7,
  },
});
