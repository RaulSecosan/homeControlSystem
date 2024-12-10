import { StyleSheet, Text, View, Image } from "react-native";
import { LinearGradient } from "expo-linear-gradient";

import ButtonOriginal from "./ButtonOriginal";
import { useState, useEffect } from "react";
export default function GroupButtons({
  paragraphName,
  buttonLeftName,
  buttonRightName,
  buttonLeftAction,
  buttonRightAction,
}) {
  return (
    <View style={styles.alignContainer}>
      <Text style={styles.paragraph}>{paragraphName}</Text>
      <View style={styles.buttonRow}>
        <View style={styles.buttonBox}>
          <LinearGradient
            colors={["#BB6666", "#552F2F"]}
            start={{ x: 0.0, y: 0.0 }}
            end={{ x: 1.0, y: 1.0 }}
            locations={[0, 0.8]}
            style={styles.buttonBoxGradient}
          >
            <ButtonOriginal name={buttonLeftName} action={buttonLeftAction} />
          </LinearGradient>
        </View>
        <View style={styles.buttonBox}>
          <LinearGradient
            colors={["#BB6666", "#552F2F"]}
            start={{ x: 0.0, y: 0.0 }}
            end={{ x: 1.0, y: 1.0 }}
            locations={[0, 0.8]}
            style={styles.buttonBoxGradient}
          >
            <ButtonOriginal name={buttonRightName} action={buttonRightAction} />
          </LinearGradient>
        </View>
      </View>
    </View>
  );
}

import { ref, onValue } from "firebase/database";
import { database } from "../components/firebase";

export function GroupButtonsWithAutoFunction({
  paragraphName,
  buttonLeftName,
  buttonRightName,
  buttonLeftAction,
  buttonRightAction,
}) {
  const [mode, setMode] = useState("turnOnAutoModeForHallLed");

  const [autoHallLed, setAutoHallLed] = useState(null);

  useEffect(() => {
    const sensorDataRef = ref(database, "/led/hallLedStatus");

    const unsubscribe = onValue(sensorDataRef, (snapshot) => {
      setAutoHallLed(snapshot.val());
    });

    return () => unsubscribe();
  }, []);


  const handlePress = () => {
    if (mode === "turnOnAutoModeForHallLed") {
      setMode("turnOffAutoModeForHallLed");
      return "turnOffAutoModeForHallLed";
    } else  {
      setMode("turnOnAutoModeForHallLed");
      return "turnOnAutoModeForHallLed";
    }
  };
  // console.log(autoHallLed)
  return (
    <View style={styles.alignContainer}>
      <Text style={styles.paragraph}>{paragraphName}</Text>
      <View style={styles.buttonRow}>
        <View style={styles.buttonBox}>
          <LinearGradient
            colors={["#BB6666", "#552F2F"]}
            start={{ x: 0.0, y: 0.0 }}
            end={{ x: 1.0, y: 1.0 }}
            locations={[0, 0.8]}
            style={styles.buttonBoxGradient}
          >
            <ButtonOriginal name={buttonLeftName} action={buttonLeftAction} />
          </LinearGradient>
        </View>

        <ButtonOriginal action={handlePress}>
          <Image
            source={
                autoHallLed == 'auto'
                ? require("../assets/images/inside/auto2.png")
                : require("../assets/images/inside/noAuto.png")
            }
            style={styles.image}
          />
        </ButtonOriginal>

        <View style={styles.buttonBox}>
          <LinearGradient
            colors={["#BB6666", "#552F2F"]}
            start={{ x: 0.0, y: 0.0 }}
            end={{ x: 1.0, y: 1.0 }}
            locations={[0, 0.8]}
            style={styles.buttonBoxGradient}
          >
            <ButtonOriginal name={buttonRightName} action={buttonRightAction} />
          </LinearGradient>
        </View>
      </View>
    </View>
  );
}

export function OneButtonForStatusPage({
  paragraphName,
  buttonLeftName,
  buttonLeftAction,
}) {
  return (
    <View style={styles.alignContainer}>
      <Text style={styles.paragraph}>{paragraphName}</Text>
      <View style={styles.buttonRow}>
        <View style={styles.buttonBox}>
          <LinearGradient
            colors={["#BB6666", "#552F2F"]}
            start={{ x: 0.0, y: 0.0 }}
            end={{ x: 1.0, y: 1.0 }}
            locations={[0, 0.8]}
            style={styles.buttonBoxGradient}
          >
            <ButtonOriginal name={buttonLeftName} action={buttonLeftAction} />
          </LinearGradient>
        </View>
      </View>
    </View>
  );
}

export function OneButton({ name, link, navigation }) {
  return (
    <LinearGradient
      colors={["#BB6666", "#552F2F"]}
      start={{ x: 0.0, y: 0.0 }}
      end={{ x: 1.0, y: 1.0 }}
      locations={[0, 0.8]}
      style={styles.buttonBoxGradientforOneButton}
    >
      <ButtonOriginal name={name} link={link} navigation={navigation} />
    </LinearGradient>
  );
}

export function StatusButton({ name, link, navigation }) {
  return (
    <LinearGradient
      colors={["#BB6666", "#552F2F"]}
      start={{ x: 0.0, y: 0.0 }}
      end={{ x: 1.0, y: 1.0 }}
      locations={[0, 0.8]}
      style={styles.buttonBoxGradientforStatusButton}
    >
      <ButtonOriginal name={name} link={link} navigation={navigation} />
    </LinearGradient>
  );
}

export function StatusPageButton({ name, link, navigation }) {
  return (
    <LinearGradient
      colors={["#4a3333", "#552F2F"]}
      // start={{ x: 0.0, y: 0.0 }}
      // end={{ x: 1.0, y: 1.0 }}
      // locations={[0, 0.8]}
      style={styles.buttonBoxGradientforStatusPage}
    >
      <ButtonOriginal name={name} link={link} navigation={navigation} />
    </LinearGradient>
  );
}

const styles = StyleSheet.create({
  alignContainer: {
    justifyContent: "center",
    alignItems: "center",
  },
  paragraph: {
    fontSize: 30,
    marginTop: 5,
    color: "white",
  },
  buttonRow: {
    flexDirection: "row",
    gap: 20,
  },
  buttonBoxGradient: {
    width: 154,
    height: 40,
    borderRadius: 50,
    borderWidth: 1,
    borderColor: "#BB6666",
    justifyContent: "center",
    alignItems: "center",
    marginTop: 10,
    opacity: 0.8,
  },
  buttonBoxGradientforOneButton: {
    marginTop: 20,
    width: 55,
    height: 50,
    backgroundColor: "#BB6666",
    justifyContent: "center",
    alignItems: "center",
    borderRadius: 30,
    marginRight: 10,
    borderWidth: 1,
    borderColor: "#BB6666",
  },

  buttonBoxGradientforStatusButton: {
    marginTop: 20,
    width: 55,
    height: 50,
    backgroundColor: "#BB6666",
    justifyContent: "center",
    alignItems: "center",
    borderRadius: 10,
    // marginRight: 10,
    borderWidth: 1,
    borderColor: "#BB6666",
  },
  buttonBoxGradientforStatusPage: {
    marginTop: 20,
    width: 55,
    height: 50,
    backgroundColor: "#4a3333",
    justifyContent: "center",
    alignItems: "center",
    borderRadius: 30,
    marginRight: 10,
    borderWidth: 1,
    borderColor: "#BB6666",
  },

  image: {
    width: 35,
    height: 35,
    marginTop: 10,
    marginHorizontal: -15,
    borderColor: "red",
    borderWidth: 0.5,
    borderRadius: 20,
  },
});
