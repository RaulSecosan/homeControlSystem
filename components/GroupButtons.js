import { StyleSheet, Text, View } from "react-native";
import { LinearGradient } from "expo-linear-gradient";

import ButtonOriginal from "./ButtonOriginal";
export default function GroupButtons({ paragraphName,buttonLeftName,buttonRightName }) {
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
            <ButtonOriginal name={buttonLeftName} action={"openFrontDoor"} />
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
            <ButtonOriginal name={buttonRightName} action={"closeFrontDoor"} />
          </LinearGradient>
        </View>
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  alignContainer: {
    justifyContent: "center",
    alignItems: "center",
  },
  paragraph: {
    fontSize: 30,
    marginTop: 40,
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
    borderWidth: 0.5,
    borderColor: "#BB6666",
    justifyContent: "center",
    alignItems: "center",
    marginTop: 20,
    opacity: 0.8,
  },
});
