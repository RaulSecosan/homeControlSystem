import { StyleSheet, View, ImageBackground, SafeAreaView } from "react-native";

import Title from "../../Title";
import GroupButtons from "../../GroupButtons";
import { DirectionButton } from "../../GroupButtons";

export default function App({ navigation }) {
  return (
    <View style={styles.container}>
      <ImageBackground
        source={require("../../../assets/images/background/outsideBackground.png")}
        resizeMode="cover"
        style={styles.backgroundImg}
      >
      <View style={styles.title}>
        <Title name="Outside" />
      </View>

        <GroupButtons
          paragraphName="Main Door"
          buttonLeftName="Open"
          buttonLeftAction="openFrontDoor"
          buttonRightName="Close"
          buttonRightAction="closeFrontDoor"
        />

        <GroupButtons
          paragraphName="Front Door Lights"
          buttonLeftName="On"
          buttonLeftAction="turnOnDoorLed"
          buttonRightName="Off"
          buttonRightAction="turnOffDoorLed"
        />

        <GroupButtons
          paragraphName="Garage Lights"
          buttonLeftName="On"
          buttonLeftAction="turnOnGarageLed"
          buttonRightName="Off"
          buttonRightAction="turnOffGarageLed"
        />

        <GroupButtons
          paragraphName="Garage Gate"
          buttonLeftName="Open"
          buttonLeftAction="openGarageGate"
          buttonRightName="Close"
          buttonRightAction="closeGarageGate"
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
  },
  backgroundImg: {
    flex: 1,
    justifyContent: "center",
    alignItems: "center",
    justifyContent: 'space-around',

  },
title:{
  marginTop: 40,
  marginBottom: -20,
},
  backButton: {
    marginTop: -20,
    marginBottom: 20,
  },
});
