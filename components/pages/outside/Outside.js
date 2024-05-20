import { StatusBar } from "expo-status-bar";
import { StyleSheet, Text, View, Image, Button } from "react-native";
// import homeInside from 'assets/images/homePage/homeInside.png'
export default function App() {
  return (
    <View style={styles.container}>
      <StatusBar style="auto" />
      <Text>Outside</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#531225",
    alignItems: "center",
    justifyContent: "center",
  },

});
