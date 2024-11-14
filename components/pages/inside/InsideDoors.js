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
          <View style={styles.firstButtonCointainer}>
            <GroupButtons
              paragraphName="Main Door"
              buttonLeftName="Open"
              buttonLeftAction="openFrontDoor"
              buttonRightName="Close"
              buttonRightAction="closeFrontDoor"
            />
          </View>

          <View style={styles.secondButtonCointainer}>
            <GroupButtons
              paragraphName="Guest Door"
              buttonLeftName="Open"
              buttonLeftAction="openGuestDoor"
              buttonRightName="Close"
              buttonRightAction="closeGuestDoor"
            />
            </View>

            <View style={styles.thirdButtonCointainer}>
            <GroupButtons
              paragraphName="BedRoom Door"
              buttonLeftName="Open"
              buttonLeftAction="openBedRoomDoor"
              buttonRightName="Close"
              buttonRightAction="closeBedRoomDoor"
            />
          </View>

          <View style={styles.forthButtonCointainer}>
            <GroupButtons
              paragraphName="Living room Fan"
              buttonLeftName="On"
              buttonLeftAction="openLivingFan"
              buttonRightName="Off"
              buttonRightAction="closeLivingFan"
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
    marginBottom: 10,
    top: -50,
  },
  buttonsCointainer: {
    marginVertical: 30,
  },
  firstButtonCointainer:{
    top: -60
  },

  secondButtonCointainer:{
    top: -10
  },

  thirdButtonCointainer:{
    top: 50
  },
  forthButtonCointainer:{
    top: 100
  },
  backButton: {
    marginTop: 65,
    bottom: -60
  },
});
