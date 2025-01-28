import { StyleSheet, Text, View, Image, Pressable } from "react-native";

import Title from "../../Title";
import GroupButtons, { StatusButton } from "../../GroupButtons";
import DirectionButton from "../../DirectionButton";
import { OneButton } from "../../GroupButtons";

export default function Inside({ navigation }) {
  function pressHandlerSensors() {
    navigation.navigate("Sensors");
  }
  function pressHandlerLights() {
    navigation.navigate("InsideLights");
  }
  return (
    <View style={styles.container}>
      <View style={styles.title}>
        <Title name="Inside" />
      </View>

      <View style={styles.topButton}>
        <StatusButton name="X" link="Alarm" navigation={navigation} />
        {/* <StatusButton name="X" link="Status" navigation={navigation} /> */}
      </View>

      <GroupButtons
        paragraphName="Hallway Lights"
        buttonLeftName="On"
        buttonLeftAction="turnOnHallLed"
        buttonRightName="Off"
        buttonRightAction="turnOffHallLed"
      />

      <View style={styles.insideBox}>
        <Image
          source={require("../../../assets/images/inside/sensorsPhoto.png")}
        />
        {/* Text Box */}
        <Pressable
          onPress={pressHandlerSensors}
          style={({ pressed }) => (pressed ? [styles.pressed] : "")}
        >
          <View style={styles.buttonBox}>
            <Text style={styles.button}>Sensors</Text>
          </View>
        </Pressable>
      </View>

      <View style={styles.insideBox}>
        <Image
          source={require("../../../assets/images/inside/lightsPhoto.png")}
        />
        {/* Text Box */}
        <Pressable
          onPress={pressHandlerLights}
          style={({ pressed }) => (pressed ? [styles.pressed] : "")}
        >
          <View style={styles.buttonBox}>
            <Text style={styles.button}>Lights</Text>
          </View>
        </Pressable>
      </View>

      <View style={styles.bottomButtons}>
        <DirectionButton
          name={"→"}
          link={"InsideSecondPage"}
          navigation={navigation}
        />
        <OneButton name="H" link="Home" navigation={navigation} />
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
    marginTop: 30,
  },

  insideBox: {
    borderTopLeftRadius: 20,
    borderTopRightRadius: 20,
    alignItems: "center",
    shadowColor: "black",
    shadowOffset: { width: 0, height: 4 },
    shadowRadius: 6,
    shadowOpacity: 1,
    // marginBottom: 20,
    marginTop: 40,
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
  bottomButtons: {
    flexDirection: "row-reverse",
    alignItems: "center",
  },
  topButton: {
    position: "absolute",
    top: "7%",
    left: "5%",
    opacity: 0.4,
  },
});
