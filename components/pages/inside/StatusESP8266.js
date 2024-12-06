import { StyleSheet, Text, View } from "react-native";

import { TitleForStatusPage } from "../../Title";
import { DirectionButtonStatus } from "../../DirectionButton";
import GroupButtons, {
  OneButton,
  OneButtonForStatusPage,
  StatusPageButton,
} from "../../GroupButtons";
import { useState, useEffect } from "react";
import { ref, onValue } from "firebase/database";
import { database } from "../../firebase";

export default function Status({ navigation }) {
  const [sensorData, setSensorData] = useState(null);

  useEffect(() => {
    const sensorDataRef = ref(database, "/statusESP8266");

    const unsubscribe = onValue(sensorDataRef, (snapshot) => {
      setSensorData(snapshot.val());
    });

    // Clean up listener on component unmount
    return () => unsubscribe();
  }, []);

  return (
    <View style={styles.container}>
      <View style={styles.title}>
        <TitleForStatusPage name="Status ESP8266" />
      </View>

      <View style={styles.rowBox}>
        <View style={styles.displayBox}>
          <Text style={styles.displayTitleBoxFor2Rows}>RAM</Text>
          <View style={styles.displayRowInsideBoxFor2RowsRAM}>
            <Text style={styles.displayNumberFor2RowRAM}>
              {sensorData
                ? "Free \n " + sensorData["ram"]["freeRAM"]
                : "Loading..."}
            </Text>
            <Text style={styles.displayNumberFor2RowRAM}>
              {sensorData
                ? "Total \n " + sensorData["ram"]["totalRAM"]
                : "Loading..."}
            </Text>
          </View>
        </View>

        <View style={styles.displayBox}>
          <Text style={styles.displayTitleBox}>CPU Frequencies</Text>
          <Text style={styles.displayNumber}>
            {sensorData ? sensorData["cpuFreq"] : "Loading..."}
          </Text>
        </View>
      </View>

      <View style={styles.rowBox}>
        <View style={styles.displayBox}>
          <Text style={styles.displayTitleBoxFor2Rows}>ROM</Text>
          <View style={styles.displayRowInsideBoxFor2Rows}>
            <Text style={styles.displayNumberFor2Rows}>
              {sensorData
                ? "Free \n " + sensorData["flash"]["freeSize"]
                : "Loading..."}
            </Text>
            <Text style={styles.displayNumberFor2Rows}>
              {sensorData
                ? "Total \n " + sensorData["flash"]["totalSize"]
                : "Loading..."}
            </Text>
          </View>
        </View>
      </View>
      {console.log(sensorData)}
      <View style={styles.rowBox}>
        <View style={styles.displayBox}>
          <Text style={styles.displayTitleBox}>Wi-Fi Signal</Text>
          <Text style={styles.displayNumber}>
            {sensorData ? sensorData["wifiSignal"] : "Loading..."}
          </Text>
        </View>

        <View style={styles.displayBox}>
          <Text style={styles.displayTitleBox}>Reset Status</Text>
          <Text style={styles.displayNumber}>
            {sensorData ? sensorData["resetReason"] : "Loading..."}
          </Text>
        </View>
      </View>

      <View style={styles.rowBox}>
        <View style={styles.displayBox}>
          <Text style={styles.displayTitleBox}>Ip Address</Text>
          <Text style={styles.displayNumber}>
            {sensorData ? sensorData["IP"] : "Loading..."}
          </Text>
        </View>

        <View style={styles.displayBox}>
          <Text style={styles.displayTitleBox}>MAC Address</Text>
          <Text style={styles.displayNumber}>
            {sensorData ? sensorData["macAddress"] : "Loading..."}
          </Text>
        </View>
      </View>

      {/* <View style={styles.rowBox}>
        <View style={styles.displayBox}>
          <Text style={styles.displayTitleBox}>Door status</Text>
          <Text style={styles.displayNumber}>Closed</Text>
        </View>

        <View style={styles.displayBox}>
          <Text style={styles.displayTitleBox}>ok</Text>
          <Text style={styles.displayNumber}>3 bar</Text>
        </View>
      </View> */}

      <View style={styles.button}>
        <OneButtonForStatusPage
          paragraphName="Reset ESP8266"
          buttonLeftName="Reset"
          buttonLeftAction="resetESP"
        />
      </View>

      <View style={styles.bottomButtons}>
        <DirectionButtonStatus
          name={"←"}
          link={"Inside"}
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
    justifyContent: "center",
  },
  title: {
    top: -15,
  },

  rowBox: {
    flexDirection: "row",
  },

  //   displayBox: {
  //     borderColor: "red",
  //     margin: 10,
  //     backgroundColor: "#382929",
  //     padding: 15,
  //     width: 150,
  //     // maxWidth: "80%",
  //     // minWidth: "40%",

  //     // height: 80,
  //     height: "90%",
  //     borderRadius: 20,
  //   },

  displayBox: {
    borderColor: "red",
    // borderWidth: 1,
    margin: 10,
    backgroundColor: "#382929",
    padding: 15,
    // width: 150,
    width: "60%",
    // height: 80,
    height: "90%",
    flexShrink: 1,
    borderRadius: 20,
  },

  displayTitleBox: {
    color: "white",
    fontSize: 20,
  },
  displayTitleBoxFor2Rows: {
    textAlign: "center",
    color: "white",
    fontSize: 20,
  },

  displayNumber: {
    color: "white",
    fontSize: 20,
    fontWeight: "600",
  },

  displayNumberFor2Rows: {
    color: "white",
    fontSize: 20,
    fontWeight: "600",
    textAlign: "center",
  },

  displayRowInsideBox: {
    flexDirection: "row",
  },
  displayRowInsideBoxFor2Rows: {
    flexDirection: "row",
    justifyContent: "center",
    gap: 30,
  },

  displayRowInsideBoxFor2RowsRAM: {
    flexDirection: "row",
    justifyContent: "center",
    gap: 20,
  },

  displayNumberFor2RowRAM: {
    color: "white",
    fontSize: 16,
    fontWeight: "600",
    textAlign: "center",
  },

  button: {
    marginBottom: -20,
  },
  bottomButtons: {
    flexDirection: "row-reverse",
    alignItems: "center",
    bottom: -40,
  },
});
