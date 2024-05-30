import { StyleSheet, Text, View } from "react-native";
import { LinearGradient } from "expo-linear-gradient";

import ButtonOriginal from "./ButtonOriginal";
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

export function OneButton({ name, link,navigation }) {
  return (
    <LinearGradient
      colors={["#BB6666", "#552F2F"]}
      start={{ x: 0.0, y: 0.0 }}
      end={{ x: 1.0, y: 1.0 }}
      locations={[0, 0.8]}
      style={styles.buttonBoxGradientforOneButton}
    >
      <ButtonOriginal name={name} link={link}  navigation={navigation}/>
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
});
