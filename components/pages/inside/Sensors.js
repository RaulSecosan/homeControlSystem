import { StyleSheet, Text, View, ImageBackground, Image } from "react-native";
import React, { useEffect, useState } from "react";
import { ref, onValue } from "firebase/database";
import { database } from "../../firebase";

import Title from "../../Title";
import DirectionButton from "../../DirectionButton";

export default function Sensors({ navigation }) {
  const [sensorData, setSensorData] = useState(null);
  const [fireBorderColor, setFireBorderColor] = useState("#741638"); // Initial color (no fire)
  const [gasBorderColor, setGasBorderColor] = useState("#741638"); // Initial color (no fire)

  useEffect(() => {
    const sensorDataRef = ref(database, "/sensorData");

    const unsubscribe = onValue(sensorDataRef, (snapshot) => {
      setSensorData(snapshot.val());
    });

    // Clean up listener on component unmount
    return () => unsubscribe();
  }, []);

  useEffect(() => {
    let fireColorInterval;
    let gasColorInterval;

    // Fire sensor logic
    if (sensorData && sensorData["fire"] === "Fire") {
      fireColorInterval = setInterval(() => {
        setFireBorderColor((prevColor) =>
          prevColor === "orange" ? "red" : "orange"
        );
      }, 1000);
    } else {
      clearInterval(fireColorInterval);
      setFireBorderColor("#741638"); // Reset to default color (no fire)
    }

    // Gas sensor logic
    if (sensorData && sensorData["gas"] === "Gas") {
      gasColorInterval = setInterval(() => {
        setGasBorderColor((prevColor) =>
          prevColor === "orange" ? "red" : "orange"
        );
      }, 1000);
    } else {
      clearInterval(gasColorInterval);
      setGasBorderColor("#741638"); // Reset to default color (no gas)
    }

    // Cleanup the intervals on component unmount or when sensor data changes
    return () => {
      clearInterval(fireColorInterval);
      clearInterval(gasColorInterval);
    };
  }, [sensorData]);

  return (
    <View style={styles.container}>
      <ImageBackground
        source={require("../../../assets/images/background/sensorsBackground.png")}
        style={styles.backgroundImg}
      >
        <View style={styles.title}>
          <Title name="Sensors" />
        </View>

        <View style={styles.sensorDangerGroupInsideSensors}>
          <View style={styles.sensorNameCenter}>
            <Text style={styles.sensorName}>Gas</Text>

            <View
              style={[
                styles.sensorDangerDetail,
                { borderColor: gasBorderColor },
              ]}
            >
              <Text style={styles.sensorDetailName}>
                {sensorData ? sensorData["gas"] : "Loading..."}
              </Text>
            </View>
          </View>

          <View style={styles.sensorNameCenter}>
            <Text style={styles.sensorName}>Fire</Text>

            <View
              style={[
                styles.sensorDangerDetail,
                { borderColor: fireBorderColor },
              ]}
            >
              <Text style={styles.sensorDetailName}>
                {sensorData ? sensorData["fire"] : "Loading..."}
              </Text>
            </View>
          </View>
        </View>

        <View style={styles.sensorNameCenterMotion}>
          <Text style={styles.sensorNameMotion}>Motion</Text>

          <View style={styles.sensorDetail}>
            <Text style={styles.sensorDetailNameMotion}>
              {sensorData ? sensorData["motion"] : "Loading..."}
            </Text>
          </View>
        </View>

        <View style={styles.temperatureBox}>
          <View style={styles.sensorNameCenter}>
            <Text style={styles.sensorName}>Temperature</Text>
            <View style={styles.sensorDetailTv}>
              <Image
                source={require("../../../assets/images/inside/sensorsTv.png")}
              />
              <View style={styles.sensorDetailTemperature}>
                <Text style={styles.sensorDetailNameTemperature}>
                  {sensorData ? sensorData["temperature"] + "°" : "Loading..."}
                </Text>
                <Text style={styles.sensorDetailHumidity}>
                  {sensorData ? sensorData["humidity"] + "%" : "Loading..."}
                </Text>
              </View>
            </View>
          </View>
        </View>

        <View style={styles.sensorDangerGroup}>
          <View style={styles.sensorNameCenter}>
            <Text style={styles.sensorName}>Day?</Text>

            <View style={styles.sensorDetail}>
              <Text style={styles.sensorDetailName}>
                {sensorData ? sensorData["isDay"] : "Loading..."}
              </Text>
            </View>
          </View>

          <View style={styles.sensorNameCenterDoorStatus}>
            <Text style={styles.sensorNameMotionDoorStatus}>Front Door</Text>

            <View style={styles.sensorDetail}>
              <Text style={styles.sensorDetailNameDoorStatus}>
                {sensorData ? sensorData["doorStatus"] : "Loading..."}
              </Text>
            </View>
          </View>

          <View style={styles.sensorNameCenter}>
            <Text style={styles.sensorName}>Rain</Text>

            <View style={styles.sensorDetail}>
              <Text style={styles.sensorDetailName}>
                {sensorData ? sensorData["rain"] : "Loading..."}
              </Text>
            </View>
          </View>
        </View>

        <View style={styles.backButton}>
          <DirectionButton name={"←"} link={"Inside"} navigation={navigation} />
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
    justifyContent: "center",
    alignItems: "center",
  },
  title: {
    marginBottom: -20,
  },
  sensorNameCenter: {
    alignItems: "center",
  },
  sensorNameCenterMotion: {
    marginTop: -50,
    alignItems: "center",
  },
  sensorName: {
    marginTop: 40,
    marginBottom: 10,
    color: "white",
    fontSize: 30,
  },

  
  sensorNameMotion: {
    marginTop: 20,
    marginBottom: 10,
    color: "white",
    fontSize: 28,
  },
  temperatureBox:{
    marginBottom: 30,
  },

  sensorNameCenterDoorStatus: {
    marginTop: -50,
    alignItems: "center",
  },

  sensorNameMotionDoorStatus: {
    // marginTop: 20,
    // marginBottom: 10,
    color: "white",
    fontSize: 28,
  },

  sensorDetailNameDoorStatus: {
    color: "white",
    fontSize: 19,
    // paddingHorizontal: 10,
  },

  sensorDangerGroupInsideSensors: {
    flexDirection: "row",
    gap: 200,
    marginTop: -25,
  },
  sensorDangerGroup: {
    flexDirection: "row",
    gap: 30,
    marginTop: 50,
  },
  sensorDangerDetail: {
    width: "120%",
    height: 48,
    // opacity: 0.8,
    backgroundColor: "#741638",
    justifyContent: "center",
    alignItems: "center",
    borderRadius: 10,
    borderWidth: 4,
  },
  sensorDetail: {
    width: "120%",
    height: 48,
    backgroundColor: "#741638",
    justifyContent: "center",
    alignItems: "center",
    borderRadius: 10,
  },

  sensorDetailTv: {
    marginTop: -15,
    marginBottom: -10,
    width: 200,
    justifyContent: "center",
    alignItems: "center",
    // position: "relative",
  },
  sensorDetailTemperature: {
    position: "absolute",
    top: 45,
    left: 50,
  },
  sensorDetailName: {
    color: "white",
    fontSize: 20,
  },

  sensorDetailNameTemperature: {
    color: "white",
    fontSize: 20,
    left:15
  },

  sensorDetailNameMotion: {
    color: "white",
    fontSize: 19,
    paddingHorizontal: 10,
  },
  sensorDetailHumidity: {
    color: "white",
    fontSize: 20,
    marginLeft: 13,
    opacity: "0.8",
  },
  backButton: {
    // marginTop: 40,
    marginBottom: -40,
  },
});
