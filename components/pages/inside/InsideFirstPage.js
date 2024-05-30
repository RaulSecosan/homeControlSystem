import { StyleSheet, Text, View, ImageBackground, Button } from "react-native";
import { LinearGradient } from "expo-linear-gradient";

import ButtonOriginal from "../../ButtonOriginal";
import Title from "../../Title";
import GroupButtons from "../../GroupButtons";

export default function Inside({ navigation }) {
  return (
    <View style={styles.container}>
      <Title name="Inside" />
      <GroupButtons paragraphName='Hall Lights' buttonLeftName='On' buttonRightName='Off'/>
      


      <View style={styles.backButton}>
        <ButtonOriginal name={"←"} link={"Home"} navigation={navigation} />
      </View>
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
    zIndex: 1000,
    justifyContent: "center",
    alignItems: "center",
  },

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

  backButton: {
    marginTop: 60,
    width: 165,
    height: 50,
    backgroundColor: "#9D5757",
    justifyContent: "center",
    alignItems: "center",
    borderRadius: 30,
  },
});
