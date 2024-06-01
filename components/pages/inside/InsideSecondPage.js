import { StyleSheet, Text, View, Image, Pressable } from "react-native";

import Title from "../../Title";
import GroupButtons from "../../GroupButtons";
import DirectionButton from "../../DirectionButton";
import { OneButton } from "../../GroupButtons";
import { LinearGradient } from "expo-linear-gradient";

export default function InsideSecondPage({ navigation }) {
  function pressHandlerDoors() {
    navigation.navigate("InsideDoors");
  }
  function pressHandlerWindows() {
    navigation.navigate("InsideWindows");
  }
  return (
    <View style={styles.container}>
      <View style={styles.title}>
        <Title name="Inside" />
      </View>

      <View style={styles.insideBox}>
        <Image
          source={require("../../../assets/images/inside/SecondPageDoor.png")}
        />
        {/* Text Box */}
        <Pressable
          onPress={pressHandlerDoors}
          style={({ pressed }) => (pressed ? [styles.pressed] : "")}
        >
          <View style={styles.buttonBox}>
            <Text style={styles.button}>Doors</Text>
          </View>
        </Pressable>
      </View>

      <View style={styles.insideBox}>
        <Image
          source={require("../../../assets/images/inside/SecondPageWindow.png")}
        />
        {/* Text Box */}
        <Pressable
          onPress={pressHandlerWindows}
          style={({ pressed }) => (pressed ? [styles.pressed] : "")}
        >
          <View style={styles.buttonBox}>
            <Text style={styles.button}>Windows</Text>
          </View>
        </Pressable>
      </View>

      <View style={styles.backButton}>
        <DirectionButton name={"←"} link={"Inside"} navigation={navigation} />
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
  title: {
    // marginTop: 30,
    top: -20,
  },

  insideBox: {
    borderTopLeftRadius: 20,
    borderTopRightRadius: 20,
    alignItems: "center",
    shadowColor: "black",
    shadowOffset: { width: 0, height: 4 },
    shadowRadius: 6,
    shadowOpacity: 1,
    marginBottom: 30,
    marginTop: 30,
  },
  buttonBox: {
    marginTop: -65,
    backgroundColor: "#883838",
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

  pressed: {
    opacity: 0.7,
  },
  backButton:{
    bottom: -30,
  }
});
