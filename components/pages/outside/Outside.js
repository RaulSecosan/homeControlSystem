import { StyleSheet, Text, View, ImageBackground, Button } from "react-native";
import { LinearGradient } from "expo-linear-gradient";

import ButtonOriginal from "../../ButtonOriginal";
import Title from "../../Title";
import GroupButtons from "../../GroupButtons";
import DirectionButton from "../../DirectionButton";

export default function App({ navigation }) {
  return (
    <View style={styles.container}>
      <ImageBackground
        source={require("../../../assets/images/background/outsideBackground.png")}
        resizeMode="cover"
        style={styles.backgroundImg}
      >
        <Title name="Outside" />

        <GroupButtons
          paragraphName="Main Door"
          buttonLeftName="Open"
          buttonLeftAction="openFrontDoor"
          buttonRightName="Close"
          buttonRightAction="closeFrontDoor"
        />
        <GroupButtons
          paragraphName="Lumini de pe alee"
          buttonLeftName="On"
          buttonRightName="Off"
        />
        <GroupButtons
          paragraphName="Lumini de pe Casa"
          buttonLeftName="On"
          buttonLeftAction="turnOn"
          buttonRightName="Off"
          buttonRightAction="turnOff"
        />
        <GroupButtons
          paragraphName="Usa garaj"
          buttonLeftName="Open"
          buttonLeftAction="openGate"
          buttonRightName="Close"
          buttonRightAction="closeGate"
        />

        <DirectionButton name={"←"} link={"Home"} navigation={navigation} />
      </ImageBackground>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#531225",
    opacityColor: 0.15,
    alignItems: "center",
    justifyContent: "center",
    zIndex: 1,
  },
  backgroundImg: {
    flex: 1,
    width: "100%",
    zIndex: 1000,
    justifyContent: "center",
    alignItems: "center",
  },

  backButton: {
    marginTop: 60,
    width: 165,
    height: 50,
    backgroundColor: "#9D5757",
    justifyContent: "center",
    alignItems: "center",
    borderRadius: 30,
  },
});
