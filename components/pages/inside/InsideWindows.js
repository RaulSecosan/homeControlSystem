import { StyleSheet, Text, View, ImageBackground, Button } from "react-native";

import Title from "../../Title";
import GroupButtons from "../../GroupButtons";
// import DirectionButton from "../../DirectionButton";
import { DirectionButton } from "../../GroupButtons";

export default function InsideWindows({ navigation }) {
  return (
    <View style={styles.container}>
      <ImageBackground
        source={require("../../../assets/images/background/InsideWindows.png")}
        resizeMode="cover"
        style={styles.backgroundImg}
      >
        <View style={styles.title}>
          <Title name="Windows" />
        </View>

        <View style={styles.buttonsCointainer}>
          <View style={styles.firstButtonCointainer}>
            <GroupButtons
              paragraphName="Living Window"
              buttonLeftName="Open"
              buttonLeftAction="openLivingWindow"
              buttonRightName="Close"
              buttonRightAction="closeLivingWindow"
            />
          </View>

          <View style={styles.secondButtonCointainer}>
            <GroupButtons
              paragraphName="Guest Window"
              buttonLeftName="Open"
              buttonLeftAction="openGuestWindow"
              buttonRightName="Close"
              buttonRightAction="closeGuestWindow"
            />
          </View>

          <View style={styles.thirdButtonCointainer}>
            <GroupButtons
              paragraphName="BedRoom Window"
              buttonLeftName="Open"
              buttonLeftAction="openBedRoomWindow"
              buttonRightName="Close"
              buttonRightAction="closeBedRoomWindow"
            />
          </View>
        </View>

        <View style={styles.backButton}>
          <DirectionButton
            name={"←"}
            link={"InsideSecondPage"}
            navigation={navigation}
          />
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
    top: -80,
    marginBottom: 10,
  },
  buttonsCointainer: {
    marginVertical: 20,
  },

  firstButtonCointainer: {
    top: -60,
  },

  secondButtonCointainer: {
    top: -10,
  },

  thirdButtonCointainer: {
    top: 50,
  },

  backButton: {
    marginTop: 65,
    bottom: -80,
  },
});
