import React from "react";
import { StyleSheet, Text, View, Button, Alert } from "react-native";

const ESP8266_IP = "http://192.168.3.213"; 

export const turnOn = async () => {
    try {
      await fetch(`${ESP8266_IP}/on`);
    //   Alert.alert("Bec aprins");
    } catch (error) {
      Alert.alert("Eroare la aprinderea becului");
    }
  };

 export const turnOff = async () => {
    try {
      await fetch(`${ESP8266_IP}/off`);
    //   Alert.alert("Bec stins");
    } catch (error) {
      Alert.alert("Eroare la stingerea becului");
    }
  };
// export default function Bec() {
//   return (
//     <View style={styles.container}>
//       <Text style={styles.header}>Control BEC</Text>
//       <View style={styles.buttonContainer}>
//         <Button title="Aprinde" onPress={turnOn} />
//         <Button title="Stinge" onPress={turnOff} />
//       </View>
//     </View>
//   );
// }

// const styles = StyleSheet.create({
//   container: {
//     flex: 1,
//     justifyContent: "center",
//     alignItems: "center",
//     backgroundColor: "#fff",
//   },
//   header: {
//     fontSize: 24,
//     marginBottom: 20,
//   },
//   buttonContainer: {
//     flexDirection: "row",
//     justifyContent: "space-between",
//     width: "60%",
//   },
// });
