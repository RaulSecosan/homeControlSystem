import { StatusBar } from "expo-status-bar";
import { StyleSheet, Text, View, Image, Pressable } from "react-native";
// import homeInside from 'assets/images/homePage/homeInside.png'
export default function App({ navigation }) {
  function pressHandlerInside() {
    console.log("Press");
  }
  function pressHandlerOutside() {
    // console.log("PresOutside");
    navigation.navigate("Outside");
  }
  return (
    <View style={styles.container}>
      <StatusBar style="auto" />
      {/* insideBox */}
      <View style={styles.insideBox}>
        <Image
          source={require("../../assets/images/homePage/homeInside.png")}
        />
        {/* Text Box */}
        <Pressable
          onPress={pressHandlerInside}
          style={({ pressed }) => (pressed ? [styles.pressed] : "")}
        >
          <View style={styles.buttonBox}>
            <Text style={styles.button}>Inside</Text>
          </View>
        </Pressable>
      </View>

      {/* OutsideBox */}
      <View style={styles.insideDownBox}>
        {/* Text Box */}
        <Pressable
          onPress={pressHandlerOutside}
          style={({ pressed }) => (pressed ? [styles.pressed] : "")}
        >
          <View style={styles.buttonBox}>
            <Text style={styles.button}>Outside</Text>
          </View>
        </Pressable>

        <Image
          source={require("../../assets/images/homePage/homeOutside.png")}
          style={styles.downImage}
        />
      </View>
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
  insideBox: {
    backgroundColor: "#C47676",
    height: 333,
    borderTopLeftRadius: 20,
    borderTopRightRadius: 20,
    alignItems: "center",
    shadowColor: "black",
    shadowOffset: { width: 0, height: 4 },
    shadowRadius: 6,
    shadowOpacity: 1,
  },
  buttonBox: {
    marginTop: 40,
    backgroundColor: "#9D5757",
    width: 138,
    height: 50,
    justifyContent: "center",
    alignItems: "center",
    borderRadius: 15,
  },
  button: {
    fontSize: 30,
    color: "white",
  },

  //
  insideDownBox: {
    marginTop: 60,
    backgroundColor: "#C47676",
    height: 333,
    borderBottomLeftRadius: 20,
    borderBottomRightRadius: 20,
    alignItems: "center",
    shadowOffset: { width: 0, height: -3 },
    shadowRadius: 6,
    shadowOpacity: 1,
  },

  downImage: {
    // marginBottom: 100,
    bottom: -36,
  },

  pressed: {
    opacity: 0.7,
  },
});
