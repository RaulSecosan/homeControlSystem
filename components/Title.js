import { StyleSheet, Text, View } from "react-native";

export default function Title({ name }) {
  return (
    <View style={styles.titleBox}>
      <Text style={styles.title}>{name}</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  titleBox: {
    // width: 138,
    height: 50,
    backgroundColor: "#9D5757",
    justifyContent: "center",
    alignItems: "center",
    borderRadius: 15,
  },
  title: {
    color: "white",
    fontSize: 30,
    paddingHorizontal: 25,
  },
});
