import {
  StyleSheet,
  Text,
  View,
  ImageBackground,
  Button,
  Image,
} from "react-native";

import { ref, onValue } from "firebase/database";
import { database } from "../../../components/firebase";

import Title from "../../Title";
import GroupButtons, { GroupButtonsWithAutoFunction } from "../../GroupButtons";
// import DirectionButton from "../../DirectionButton";
import { useEffect, useState } from "react";
import Slider from "@react-native-community/slider";
import ButtonOriginal from "../../ButtonOriginal";
import { fanTemperatureSlider } from "../../SensorsCommunication";
import { DirectionButton } from "../../GroupButtons";


export default function InsideDoors({ navigation }) {
  const [sliderForStaringFanTemperature, setSliderForStaringFanTemperature] =
    useState(30);
  const [mode, setMode] = useState("turnOnAutoFanMode");
  const [autoFanMode, setAutoFanMode] = useState(null);

  useEffect(() => {
    const sensorDataRef = ref(database, "/motor/fanStatus");

    const unsubscribe = onValue(sensorDataRef, (snapshot) => {
      setAutoFanMode(snapshot.val());
    });

    return () => unsubscribe();
  }, []);

  const handlePress = () => {
    if (mode === "turnOnAutoFanMode") {
      setMode("turnOffAutoFanMode");
      return "turnOffAutoFanMode";
    } else {
      setMode("turnOnAutoFanMode");
      return "turnOnAutoFanMode";
    }
  };
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
          

              buttonLeftAction="openBedRoomDoor"
              buttonRightName="Close"
              buttonRightAction="closeBedRoomDoor"
            />
          </View>

          <View style={styles.thirdButtonCointainer}>
            <GroupButtons
              paragraphName="BedRoom Door"
              buttonLeftName="Open"
              buttonLeftAction="openGuestDoor"
              buttonRightName="Close"
              buttonRightAction="closeGuestDoor"
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

            <View style={styles.bottomButtonGroup}>
              <ButtonOriginal action={handlePress}>
                <Image
                  source={
                    autoFanMode == "auto"
                      ? require("../../../assets/images/inside/auto2.png")
                      : require("../../../assets/images/inside/noAuto.png")
                  }
                  style={styles.image}
                />
              </ButtonOriginal>

              <View
                style={[
                  styles.sliderContainer,
                  autoFanMode !== "auto" && {
                    opacity: "0.5",
                  }, // Schimbă stilul chenarului
                ]}
              >
                <Text
                  style={[
                    styles.sliderText,
                    autoFanMode !== "auto" && { color: "#7A7A7A" },
                  ]}
                >
                  {`${sliderForStaringFanTemperature.toFixed(0)}°`}
                </Text>
                <Slider
                  style={[
                    styles.slider,
                    autoFanMode !== "auto" && { opacity: 0.5 },
                  ]}
                  minimumValue={15}
                  maximumValue={50}
                  minimumTrackTintColor="#FF5C58"
                  maximumTrackTintColor="#FF8C84"
                  thumbTintColor="#FF5C58"
                  value={sliderForStaringFanTemperature}
                  disabled={autoFanMode !== "auto"} // Slider blocat când nu e auto
                  onValueChange={(value) => {
                    if (autoFanMode === "auto") {
                      setSliderForStaringFanTemperature(value);
                      fanTemperatureSlider(value.toFixed(0)); // Trimite valoarea către Firebase
                    }
                  }}
                />
              </View>
            </View>
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
    marginBottom: 60,
    // top: 20,
  },
  buttonsCointainer: {
    marginVertical: 30,
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
  forthButtonCointainer: {
    top: 100,
  },

  image: {
    width: 55,
    height: 55,
    marginTop: 20,
    borderColor: "red",
    borderWidth: 0.3,
    borderRadius: 130,
    marginLeft: 20,
  },
  bottomButtonGroup: {
    flexDirection: "row",
  },
  sliderContainer: {
    marginLeft: 20,
    marginVertical: 15,
    alignItems: "center",
    // justifyContent: 'center',
    borderWidth: 1,
    borderRadius: 30,
    borderColor: "#C55656",
    backgroundColor: "rgba(85, 17, 17, 0.5)",
    height: 50,
    width: "70%",
  },

  sliderText: {
    marginTop: 7,
    fontSize: 30,
    fontWeight: "bold",
    color: "#FFDDDD",
  },
  slider: {
    width: "80%",
    // height: 40,
    top: -14,
    // bottom: 2,
    // position: 'absolute',
  },

  backButton: {
    marginTop: 35,
    bottom: -40,
  },
});
