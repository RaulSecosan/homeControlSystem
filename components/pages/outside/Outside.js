import { StyleSheet, View, ImageBackground, SafeAreaView } from "react-native";

import ButtonOriginal from "../../ButtonOriginal";
import Title from "../../Title";
import GroupButtons from "../../GroupButtons";
import DirectionButton from "../../DirectionButton";

export default function App({ navigation }) {
  return (
    <View style={styles.container}>
    {/* <SafeAreaView style={styles.safeArea}> */}
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
        {/* <GroupButtons
          paragraphName="Lumini de pe alee"
          buttonLeftName="On"
          buttonRightName="Off"
        /> */}
        <GroupButtons
          paragraphName="Lumini de la Intrare"
          buttonLeftName="On"
          buttonLeftAction="turnOnDoorLed"
          buttonRightName="Off"
          buttonRightAction="turnOffDoorLed"
        />

        <GroupButtons
          paragraphName="Lumini de la Garaj"
          buttonLeftName="On"
          buttonLeftAction="turnOnGarageLed"
          buttonRightName="Off"
          buttonRightAction="turnOffGarageLed"
        />

        <GroupButtons
          paragraphName="Usa garaj"
          buttonLeftName="Open"
          buttonLeftAction="openGarageGate"
          buttonRightName="Close"
          buttonRightAction="closeGarageGate"
        />

    <View style={styles.backButton}>
        <DirectionButton name={"←"} link={"Home"} navigation={navigation} />
    </View>
      </ImageBackground>
    {/* </SafeAreaView> */}
    </View>
  );
}

const styles = StyleSheet.create({
  // safeArea:{
  //   flex:1,
  // },
  container: {
    flex: 1,
    backgroundColor: "#531225",
    // opacityColor: 0.15,
    // alignItems: "center",
    // justifyContent: "center",
    // zIndex: 1,
  },
  backgroundImg: {
    flex: 1,
    // width: "100%",
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
