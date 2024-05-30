import { StyleSheet, View } from "react-native";
import ButtonOriginal from "./ButtonOriginal";

export default function DirectionButton({ name, link, navigation }) {
  return (
    <View style={styles.backButton}>
      <ButtonOriginal name={name} link={link} navigation={navigation} />
    </View>
  );
}

const styles = StyleSheet.create({
  backButton: {
    marginTop: 20,
    width: 165,
    height: 50,
    backgroundColor: "#BB6666",
    justifyContent: "center",
    alignItems: "center",
    borderRadius: 30,
  },
});
