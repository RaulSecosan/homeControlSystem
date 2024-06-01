import { StyleSheet, Text, View, ImageBackground, Button } from "react-native";

import Title from "../../Title";
import GroupButtons from "../../GroupButtons";
import DirectionButton from "../../DirectionButton";

export default function InsideDoors({ navigation }) {
  return (
    <View style={styles.container}>
      <ImageBackground
        source={require("../../../assets/images/background/InsideLights.png")}
        resizeMode="cover"
        style={styles.backgroundImg}
      >
        <View style={styles.title}>
          <Title name="Inside Doors" />
        </View>

        <View style={styles.buttonsCointainer}>
          <GroupButtons
            paragraphName="Main Door"
            buttonLeftName="Open"
            buttonLeftAction="openMainDoor"
            buttonRightName="Close"
            buttonRightAction="closeMainDoor"
          />

          <GroupButtons
            paragraphName="Guest Door"
            buttonLeftName="Open"
            buttonLeftAction="openGuestDoor"
            buttonRightName="Close"
            buttonRightAction="closeGuestDoor"
          />

          <GroupButtons
            paragraphName="BedRoom Door"
            buttonLeftName="Open"
            buttonLeftAction="openBedroomDoor"
            buttonRightName="Close"
            buttonRightAction="closeBedroomDoor"
          />
        </View>

        <View style={styles.backButton}>
          <DirectionButton name={"←"} link={"InsideSecondPage"} navigation={navigation} />
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

  title: {
    marginBottom: 10,
  },
  buttonsCointainer: {
    marginVertical: 20,
  },

  backButton: {
    marginTop: 65,
  },
});
