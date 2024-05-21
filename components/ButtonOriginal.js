import { StyleSheet, Text, View, Pressable } from "react-native";
import { NavigationContainer } from "@react-navigation/native";


export default function ButtonOriginal({ name, action,navigation}) {
  function pressHandlerOutside() {
    console.log("PresOutside");

    navigation.navigate(action);
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
