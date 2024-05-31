import { StyleSheet, Text, View, ImageBackground, Button } from "react-native";
import { LinearGradient } from "expo-linear-gradient";

import ButtonOriginal from "../../ButtonOriginal";
import Title from "../../Title";
import GroupButtons from "../../GroupButtons";
import DirectionButton from "../../DirectionButton";

export default function InsideLights({ navigation }) {
  return (
    <View style={styles.container}>
      <ImageBackground
        source={require("../../../assets/images/background/InsideLights.png")}
        resizeMode="cover"
        style={styles.backgroundImg}
      >
        <Title name="Inside Lights" />

        <GroupButtons
          paragraphName="Hall"
          buttonLeftName="On"
          buttonLeftAction="turnOnHallLights"
          buttonRightName="Off"
          buttonRightAction="turnOffHallLights"
        />
        <GroupButtons
          paragraphName="Living"
          buttonLeftName="On"
          buttonRightName="Off"
        />
        <GroupButtons
          paragraphName="BedRoom"
          buttonLeftName="On"
          buttonLeftAction="turnOnBedroomLights"
          buttonRightName="Off"
          buttonRightAction="turnOffBedroomLights"
        />
        <GroupButtons
          paragraphName="Kitchen/Guest"
          buttonLeftName="On"
          buttonLeftAction="turnOnKitchenLights"
          buttonRightName="Off"
          buttonRightAction="turnOffKitchenLights"
        />

        <View style={styles.backButton}>
          <DirectionButton name={"←"} link={"Home"} navigation={navigation} />
        </View>

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
    marginTop: 35,
   
  },
});
