import {
  StyleSheet,
  Text,
  View,
  ImageBackground,
  TextInput,
} from "react-native";
import Slider from "@react-native-community/slider";

import Title from "../../Title";
import GroupButtons from "../../GroupButtons";
import DirectionButton from "../../DirectionButton";
import { Fragment, useState } from "react";

export default function InsideLights({ navigation }) {
  const [sliderValueLiving, setSliderValueLiving] = useState(100);
  const [sliderValueBedRoom, setSliderValueBedRoom] = useState(100);
  const [sliderValueGuest, setSliderValueGuest] = useState(100);

  return (
    <View style={styles.container}>
      <ImageBackground
        source={require("../../../assets/images/background/InsideLights.png")}
        resizeMode="cover"
        style={styles.backgroundImg}
      >
        <View style={styles.title}>
          <Title name="Inside Lights" />
        </View>

        <View style={styles.group}>
          <GroupButtons
            paragraphName="Hall"
            buttonLeftName="On"
            buttonLeftAction="turnOnHallLights"
            buttonRightName="Off"
            buttonRightAction="turnOffHallLights"
          />
        </View>

        {/* <View style={styles.groupButtons}> */}
        <GroupButtons
          paragraphName="Living"
          buttonLeftName="On"
          buttonRightName="Off"
        />
        <View style={styles.shape}>
          <Text style={styles.text}>{sliderValueLiving}%</Text>
          <Slider
            // style={styles.slider}
            style={{
              width: 270,
              position: "absolute",
              bottom: -20,
              right: 10,
            }}
            minimumValue={0}
            maximumValue={100}
            minimumTrackTintColor="#C55656"
            // maximumTrackTintColor="#A35658"
            thumbTintColor="#E35658"
            value={sliderValueLiving}
            onValueChange={(value) => setSliderValueLiving(value.toFixed(0))}
          />
        </View>
        {/* </View> */}

        {/* <View style={styles.groupButtons}> */}

        <GroupButtons
          paragraphName="BedRoom"
          buttonLeftName="On"
          buttonLeftAction="turnOnBedroomLights"
          buttonRightName="Off"
          buttonRightAction="turnOffBedroomLights"
        />

        <View style={styles.shape}>
          <Text style={styles.text}>{sliderValueBedRoom}%</Text>
          <Slider
            style={styles.slider}
            minimumValue={0}
            maximumValue={100}
            minimumTrackTintColor="#C55656"
            // maximumTrackTintColor="#A35658"
            thumbTintColor="#E35658"
            value={sliderValueBedRoom}
            onValueChange={(value) => setSliderValueBedRoom(value.toFixed(0))}
          />
        </View>
        {/* </View> */}

        {/* <View style={styles.groupButtons}> */}
        <GroupButtons
          paragraphName="Kitchen/Guest"
          buttonLeftName="On"
          buttonLeftAction="turnOnKitchenLights"
          buttonRightName="Off"
          buttonRightAction="turnOffKitchenLights"
        />
        <View style={styles.shape}>
          <Text style={styles.text}>{sliderValueGuest}%</Text>
          <Slider
            style={{
              width: 270,
              position: "absolute",
              bottom: -20,
              right: 10,
            }}
            minimumValue={0}
            maximumValue={100}
            minimumTrackTintColor="#C55656"
            // maximumTrackTintColor="#A35658"
            thumbTintColor="#E35658"
            value={sliderValueGuest}
            onValueChange={(value) => setSliderValueGuest(value.toFixed(0))}
          />
        </View>
        {/* </View> */}
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
  },
  backgroundImg: {
    flex: 1,
    width: "100%",
    zIndex: 1000,
    justifyContent: "center",
    alignItems: "center",
  },
  // group:{marginTop:10},
  group: { marginBottom: -30 },
  title: {
    marginTop: 140,
    marginBottom: -30,
  },
  // groupButtons: {
  //   // marginTop: 50,
  //   // marginBottom: -35, ///problema la marginBootom la slider, nu mai merge, daca introduc o valoare cu '-' nu mai merge
  //   marginBottom: -35,
  //   // borderWidth:2,
  //   width: "100%",
  //   justifyContent: "center",
  //   alignItems: "center",
  // },
  slider: {
    width: 270,
    position: "absolute",
    bottom: -20,
    right: 10,
  },
  shape: {
    marginTop: 15,
    width: "75%",
    height: 50,
    backgroundColor: "rgba(85, 17, 17, 0.5)",
    borderRadius: 80,
    borderWidth: 1,
    borderColor: "#C55656",
    justifyContent: "center",
    position: "relative",
  },
  text: {
    color: "#FFDDDD",
    fontSize: 35,
    fontWeight: "bold",
    textAlign: "center",
  },

  backButton: {
    // marginTop: 35,
    marginBottom: 85,
  },
});
