import { StyleSheet, Text, View, ImageBackground, Image } from "react-native";
import React, { useEffect, useState } from 'react';
import { ref, onValue } from "firebase/database";
import { database } from '../../firebase';

import Title from "../../Title";
import DirectionButton from "../../DirectionButton";

export default function Sensors({ navigation }) {
  const [sensorData, setSensorData] = useState(null);

  useEffect(() => {
    const sensorDataRef = ref(database, '/sensorData');

    const unsubscribe = onValue(sensorDataRef, (snapshot) => {
      setSensorData(snapshot.val());
    });

    // Clean up listener on component unmount
    return () => unsubscribe();
  }, []);
  
  return (
    <View style={styles.container}>
      <ImageBackground
        source={require("../../../assets/images/background/sensorsBackground.png")}
        style={styles.backgroundImg}
      >
        <Title name="Sensors" />

      <View>

        <View style={styles.sensorDangerGroup}>
        <View style={styles.sensorNameCenter}>

          <Text style={styles.sensorName}>Gas</Text>

          <View style={styles.sensorDangerDetail}>
            <Text style={styles.sensorDetailName}>{sensorData ? sensorData['gas'] : 'Loading...'}</Text>
          </View>

        </View>     

        <View style={styles.sensorNameCenter}>
        <Text style={styles.sensorName}>Fire</Text>

          <View style={styles.sensorDangerDetail}>
            <Text style={styles.sensorDetailName}>{sensorData ? sensorData['fire'] : 'Loading...'}</Text>
          </View>
          </View>
        </View>
        </View>

        <View style={styles.sensorNameCenter}>
          <Text style={styles.sensorName}>Temperature</Text>
          <View style={styles.sensorDetailTv}>
            <Image
              source={require("../../../assets/images/inside/sensorsTv.png")}
            />
            <View style={styles.sensorDetailTemperature}>
              <Text style={styles.sensorDetailName}>{sensorData ? sensorData['temperature']+'°' : 'Loading...'}</Text>
            </View>
          </View>
        </View>

        <View style={styles.sensorNameCenter}>
          <Text style={styles.sensorName}>Umidity</Text>
          <View style={styles.sensorDetail}>
            <Text style={styles.sensorDetailName}>20</Text>
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
  sensorNameCenter: {
    alignItems: "center",
  },
  sensorName: {
    marginTop: 40,
    marginBottom: 10,
    color: "white",
    fontSize: 30,
  },
  sensorDangerGroup:{
    // display:'flex',
    flexDirection:'row',
    gap:50,
  },
  sensorDangerDetail: {
    width: '120%',
    height: 48,
    // opacity: 0.8,
    backgroundColor: "#741638",
    justifyContent: "center",
    alignItems: "center",
    borderRadius: 10,
  },
  sensorDetail: {
    width: 152,
    height: 48,
    // opacity: 0.8,
    backgroundColor: "#741638",
    justifyContent: "center",
    alignItems: "center",
    borderRadius: 30,
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
    top: 60,
    left: 45,
  },
  sensorDetailName: {
    color: "white",
    fontSize: 25,
  },
  backButton: {
    marginTop: 50,
  },
});
