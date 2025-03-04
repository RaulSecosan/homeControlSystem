import { StyleSheet, Text, View, Image, Pressable } from "react-native";
import { LinearGradient } from "expo-linear-gradient";

import Slider from "@react-native-community/slider";
import { fanTemperatureSlider } from "./SensorsCommunication";

import { useState, useEffect } from "react";

import {
  turnOnDoorLed,
  turnOffDoorLed,
  openFrontDoor,
  closeFrontDoor,
  openGuestDoor,
  closeGuestDoor,
  openBedRoomDoor,
  closeBedRoomDoor,
  turnOnGarageLed,
  turnOffGarageLed,
  openGarageGate,
  closeGarageGate,
  turnOffHallLed,
  turnOnHallLed,
  turnOnAutoModeForHallLed,
  turnOffAutoModeForHallLed,
  turnOnAutoFanMode,
  turnOffAutoFanMode,
  turnOnLivingLed,
  turnOffLivingLed,
  bedRoomSlider,
  guestSlider,
  openGuestWindow,
  closeGuestWindow,
  openBedRoomtWindow,
  closeBedRoomWindow,
  openLivingWindow,
  closeLivingWindow,
  openLivingFan,
  closeLivingFan,
  resetESP,
  armHouse,
  disarmHouse,
} from "./SensorsCommunication";

function ButtonWithGradient({ name, action, link = "", navigation }) {
  return (
    <Pressable
      onPress={() => executeAction(action)}
      style={({ pressed }) => (pressed ? [styles.pressed] : "")}
    >
      <LinearGradient
        colors={["#BB6666", "#552F2F"]}
        start={{ x: 0.0, y: 0.0 }}
        end={{ x: 1.0, y: 1.0 }}
        locations={[0, 0.8]}
        style={styles.buttonBoxGradient}
      >
        <Text style={styles.button}>{name}</Text>
      </LinearGradient>
    </Pressable>
  );
}

function ButtonWithGradientAndLink({
  name,
  link = "",
  navigation,
  stil,
  colors = ["#BB6666", "#552F2F"],
}) {
  function handlePress() {
    if (link.length !== 0 && navigation) {
      navigation.navigate(link);
    }
  }

  return (
    <Pressable
      onPress={handlePress}
      style={({ pressed }) => (pressed ? [styles.pressed] : "")}
    >
      <LinearGradient
        colors={colors}
        start={{ x: 0.0, y: 0.0 }}
        end={{ x: 1.0, y: 1.0 }}
        locations={[0, 0.8]}
        style={stil}
      >
        <Text style={styles.button}>{name}</Text>
      </LinearGradient>
    </Pressable>
  );
}

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
        <ButtonWithGradient name={buttonLeftName} action={buttonLeftAction} />
        <ButtonWithGradient name={buttonRightName} action={buttonRightAction} />
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

  const [autoHallLed, setAutoHallLed] = useState("turnOnAutoModeForHallLed");

  useEffect(() => {
    const sensorDataRef = ref(database, "/led/hallLedStatus");

    const unsubscribe = onValue(sensorDataRef, (snapshot) => {
      setAutoHallLed(snapshot.val());
    });

    return () => unsubscribe();
  }, []);

  const handlePress = () => {
    const newMode =
      mode === "turnOnAutoModeForHallLed"
        ? "turnOffAutoModeForHallLed"
        : "turnOnAutoModeForHallLed";
    setMode(newMode);
    executeAction(newMode);
  };
  return (
    <View style={styles.alignContainer}>
      <Text style={styles.paragraph}>{paragraphName}</Text>
      <View style={styles.buttonRow}>
        <ButtonWithGradient name={buttonLeftName} action={buttonLeftAction} />

        <Pressable onPress={handlePress}>
          <Image
            source={
              autoHallLed === "auto"
                ? require("../assets/images/inside/auto2.png")
                : require("../assets/images/inside/noAuto.png")
            }
            style={styles.image}
          />
        </Pressable>

        <ButtonWithGradient name={buttonRightName} action={buttonRightAction} />
      </View>
    </View>
  );
}

export function GroupButtonsWithAutoFanFunction({}) {
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
    const newMode =
      mode === "turnOnAutoFanMode" ? "turnOffAutoFanMode" : "turnOnAutoFanMode";
    setMode(newMode);
    executeAction(newMode);
  };

  return (
    <Pressable onPress={handlePress}>
      <Image
        source={
          autoFanMode === "auto"
            ? require("../assets/images/inside/auto2.png")
            : require("../assets/images/inside/noAuto.png")
        }
        style={styles.image1}
      />
    </Pressable>
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
      <ButtonWithGradient name={buttonLeftName} action={buttonLeftAction} />
    </View>
  );
}

export function OneButton({ name, link, navigation }) {
  return (
    <ButtonWithGradientAndLink
      name={name}
      link={link}
      navigation={navigation}
      stil={styles.buttonBoxGradientforOneButton}
    />
  );
}

export function StatusButton({ name, link, navigation }) {
  return (
    <ButtonWithGradientAndLink
      name={name}
      link={link}
      navigation={navigation}
      stil={styles.buttonBoxGradientforStatusButton}
    />
  );
}

export function StatusPageButton({ name, link, navigation }) {
  return (
    <ButtonWithGradientAndLink
      name={name}
      link={link}
      navigation={navigation}
      colors={["#4a3333", "#552F2F"]}
      stil={styles.buttonBoxGradientforStatusPage}
    />
  );
}

export function DirectionButton({ name, link, navigation }) {
  function handlePress() {
    if (link.length !== 0 && navigation) {
      navigation.navigate(link);
    }
  }
  return (
    <Pressable
      onPress={handlePress}
      style={({ pressed }) => (pressed ? [styles.pressed] : "")}
    >
      <View style={styles.backButton}>
        <Text style={styles.button}>{name}</Text>
      </View>
    </Pressable>
  );
}

export function DirectionButtonStatus({ name, link, navigation }) {
  function handlePress() {
    if (link.length !== 0 && navigation) {
      navigation.navigate(link);
    }
  }
  return (
    <Pressable
      onPress={handlePress}
      style={({ pressed }) => (pressed ? [styles.pressed] : "")}
    >
      <View style={styles.backButtonStatus}>
        <Text style={styles.button}>{name}</Text>
      </View>
    </Pressable>
  );
}

function executeAction(action) {
  // console.log(action);

  //pentru GroupButtonsWithAutoFunction
  if (typeof action === "function") {
    action = action();
  }

  switch (action) {
    case "turnOnDoorLed":
      turnOnDoorLed();
      break;
    case "turnOffDoorLed":
      turnOffDoorLed();
      break;
    case "turnOnGarageLed":
      turnOnGarageLed();
      break;
    case "turnOffGarageLed":
      turnOffGarageLed();
      break;
    case "turnOnHallLed":
      turnOnHallLed();
      break;
    case "turnOffHallLed":
      turnOffHallLed();
      break;
    case "turnOnAutoModeForHallLed":
      turnOnAutoModeForHallLed();
      break;
    case "turnOffAutoModeForHallLed":
      turnOffAutoModeForHallLed();
      break;
    case "turnOnLivingLed":
      turnOnLivingLed();
      break;
    case "turnOffLivingLed":
      turnOffLivingLed();
      break;
    case "turnOnBedRoomLed":
      // turnOnBedroomLed();
      bedRoomSlider(100);
      break;
    case "turnOffBedRoomLed":
      // turnOffBedroomLed();
      bedRoomSlider(0);
      break;

    case "turnOnGuestLed":
      // turnOnGuestLed();
      guestSlider(100);
      break;
    case "turnOffGuestLed":
      // turnOffGuestLed();
      guestSlider(0);

      break;

    //Doors
    case "openFrontDoor":
      openFrontDoor();
      break;
    case "closeFrontDoor":
      closeFrontDoor();
      break;
    case "openGarageGate":
      openGarageGate();
      break;
    case "closeGarageGate":
      closeGarageGate();
      break;

    case "openGuestDoor":
      openGuestDoor();
      break;
    case "closeGuestDoor":
      closeGuestDoor();
      break;
    case "openBedRoomDoor":
      openBedRoomDoor();
      break;
    case "closeBedRoomDoor":
      closeBedRoomDoor();
      break;

    //Windows
    case "openGuestWindow":
      openGuestWindow();
      break;
    case "closeGuestWindow":
      closeGuestWindow();
      break;
    case "openBedRoomWindow":
      openBedRoomtWindow();
      break;
    case "closeBedRoomWindow":
      closeBedRoomWindow();
      break;
    case "openLivingWindow":
      openLivingWindow();
      break;
    case "closeLivingWindow":
      closeLivingWindow();
      break;
    case "openLivingFan":
      openLivingFan();
      break;

    case "turnOnAutoFanMode":
      turnOnAutoFanMode();
      break;
    case "turnOffAutoFanMode":
      turnOffAutoFanMode();
      break;
    case "closeLivingFan":
      closeLivingFan();
      break;
    case "resetESP":
      resetESP();
      break;

    case "armHouse":
      armHouse();
      break;
    case "disarmHouse":
      disarmHouse();
    default:
      console.log("ai gresit comanda sau nu a fost inca initializata");
  }
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
  button: {
    fontSize: 30,
    color: "white",
  },

  pressed: {
    opacity: 0.3,
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
  backButton: {
    marginTop: 20,
    width: 165,
    height: 50,
    backgroundColor: "#BB6666",
    justifyContent: "center",
    alignItems: "center",
    borderRadius: 30,
  },

  backButtonStatus: {
    marginTop: 20,
    width: 165,
    height: 50,
    backgroundColor: "#4a3333",
    justifyContent: "center",
    alignItems: "center",
    borderRadius: 30,
  },
  image1: {
    width: 55,
    height: 55,
    marginTop: 14,
    borderColor: "red",
    borderWidth: 0.3,
    borderRadius: 130,
    marginLeft: 20,
  },
});
