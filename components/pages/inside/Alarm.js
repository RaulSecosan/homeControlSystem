import { StyleSheet, Text, View } from "react-native";

import { TitleForStatusPage } from "../../Title";
import { DirectionButtonStatus } from "../../DirectionButton";
import GroupButtons, { StatusPageButton } from "../../GroupButtons";
import { useState, useEffect } from "react";
import { ref, onValue } from "firebase/database";
import { database } from "../../firebase";

export default function Alarm({ navigation }) {
  const [sensorData, setSensorData] = useState("Loading..");
  const [fireBorderColor, setBorderColor] = useState("#741638");

  useEffect(() => {
    const sensorDataRef = ref(database, "/security");

    const unsubscribe = onValue(sensorDataRef, (snapshot) => {
      setSensorData(snapshot.val());
    });

    // Clean up listener on component unmount
    return () => unsubscribe();
  }, []);

  useEffect(() => {
    let colorInterval;

    if ((sensorData && sensorData["alert"] !== "Disarmed") && (sensorData && sensorData["alert"] !== "Clear")) {
      colorInterval = setInterval(() => {
        setBorderColor((prevColor) =>
          prevColor === "orange" ? "red" : "orange"
        );
      }, 1000);
    } else {
      clearInterval(colorInterval);
      setBorderColor("#741638"); // Reset to default color
    }

    return () => {
      clearInterval(colorInterval);
    };
  }, [sensorData]);

  return (
    <View style={styles.container}>
      <View style={styles.title}>
        <TitleForStatusPage name="Home Alarm" />
      </View>

      <View>
        <Text style={styles.text}>
          Status From Firebase: {sensorData["arm"]}
        </Text>
        <Text style={styles.text}>
          Status From ESP8266: {sensorData["status"]}
        </Text>
      </View>

      <View style={styles.box}>
        <View style={[styles.displayBox, { borderColor: fireBorderColor }]}>
          <Text style={styles.displayTitleBoxFor2Rows}>
            Intrusion Detected: {sensorData["alert"]}
          </Text>
        </View>
      </View>

      <View style={styles.button}>
        <GroupButtons
          buttonLeftName="Arm"
          buttonLeftAction="armHouse"
          buttonRightName="Disarm"
          buttonRightAction="disarmHouse"
        />
      </View>

      <View style={styles.bottomButtons}>
        <DirectionButtonStatus
          name={"→"}
          link={"Status"}
          navigation={navigation}
        />
        <StatusPageButton name="H" link="Inside" navigation={navigation} />
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#171212",
    alignItems: "center",
    justifyContent: "space-evenly",
  },

  text: {
    color: "white",
    fontSize: 25,
    fontStyle: "italic",
  },

  box: {
    flexDirection: "row",
  },

  displayBox: {
    borderColor: "red",
    margin: 10,
    backgroundColor: "#382929",
    padding: 15,
    width: "60%",
    height: "90%",
    flexShrink: 1,
    borderRadius: 20,
    borderWidth: 4,
  },

  displayTitleBoxFor2Rows: {
    textAlign: "center",
    color: "white",
    fontSize: 20,
  },

  button: {
    top: -80,
  },
  bottomButtons: {
    flexDirection: "row-reverse",
    bottom: -40,
  },
});
