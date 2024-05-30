import { StyleSheet, Text, View, ImageBackground, Button } from "react-native";
import { LinearGradient } from "expo-linear-gradient";

import ButtonOriginal from "../../ButtonOriginal";
import Title from "../../Title";
import GroupButtons from "../../GroupButtons";

export default function App({ navigation }) {
  return (
    <View style={styles.container}>
      <ImageBackground
        source={require("../../../assets/images/background/outsideBackground.png")}
        resizeMode="cover"
        style={styles.backgroundImg}
      >
        <Title name="Outside" />

        <GroupButtons paragraphName='Main Door' buttonLeftName='Open' buttonRightName='Close'/>
        <GroupButtons paragraphName='Lumini de pe alee' buttonLeftName='On' buttonRightName='Off'/>
        <GroupButtons paragraphName='Lumini de pe Casa' buttonLeftName='On' buttonRightName='Off'/>
        <GroupButtons paragraphName='Usa garaj' buttonLeftName='Open' buttonRightName='Close'/>

        {/* <View style={styles.alignContainer}>
          <Text style={styles.paragraph}>Usa de la intrare</Text>
          <View style={styles.buttonRow}>
            <View style={styles.buttonBox}>
              <LinearGradient
                colors={["#BB6666", "#552F2F"]}
                start={{ x: 0.0, y: 0.0 }}
                end={{ x: 1.0, y: 1.0 }}
                locations={[0, 0.8]}
                style={styles.buttonBoxGradient}
              >
                <ButtonOriginal name={"Open"} action={"openFrontDoor"} />
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
                <ButtonOriginal name={"Closed"} action={"closeFrontDoor"} />
              </LinearGradient>
            </View>
          </View>
        </View> */}

        
        {/* <View style={styles.alignContainer}>
          <Text style={styles.paragraph}>Lumini de pe Alee</Text>
          <View style={styles.buttonRow}>
            <View style={styles.buttonBox}>
              <LinearGradient
                colors={["#BB6666", "#552F2F"]}
                start={{ x: 0.0, y: 0.0 }}
                end={{ x: 1.0, y: 1.0 }}
                locations={[0, 0.8]}
                style={styles.buttonBoxGradient}
              >
                <ButtonOriginal name={"Open"} />
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
                <ButtonOriginal name={"Closed"} />
              </LinearGradient>
            </View>
          </View>
        </View> */}

        {/* <View style={styles.alignContainer}>
          <Text style={styles.paragraph}>Lumini de pe Casa</Text>
          <View style={styles.buttonRow}>
            <View style={styles.buttonBox}>
              <LinearGradient
                colors={["#BB6666", "#552F2F"]}
                start={{ x: 0.0, y: 0.0 }}
                end={{ x: 1.0, y: 1.0 }}
                locations={[0, 0.8]}
                style={styles.buttonBoxGradient}
              >
                <ButtonOriginal name={"Open"} action={"turnOn"} />
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
                <ButtonOriginal name={"Closed"} action={"turnOff"} />
              </LinearGradient>
            </View>
          </View>
        </View> */}

        {/* <View style={styles.alignContainer}>
          <Text style={styles.paragraph}>Usa Garaj</Text>
          <View style={styles.buttonRow}>
            <View style={styles.buttonBox}>
              <LinearGradient
                colors={["#BB6666", "#552F2F"]}
                start={{ x: 0.0, y: 0.0 }}
                end={{ x: 1.0, y: 1.0 }}
                locations={[0, 0.8]}
                style={styles.buttonBoxGradient}
              >
                <ButtonOriginal name={"Open"} />
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
                <ButtonOriginal name={"Closed"} />
              </LinearGradient>
            </View>
          </View>
        </View> */}

        <View style={styles.backButton}>
          <ButtonOriginal name={"←"} link={"Home"} navigation={navigation} />
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
    zIndex: 1000,
    justifyContent: "center",
    alignItems: "center",
  },

  // alignContainer: {
  //   justifyContent: "center",
  //   alignItems: "center",
  // },

  // paragraph: {
  //   fontSize: 30,
  //   marginTop: 40,
  //   color: "white",
  // },
  // buttonRow: {
  //   flexDirection: "row",
  //   gap: 20,
  // },

  // buttonBoxGradient: {
  //   width: 154,
  //   height: 40,
  //   borderRadius: 50,
  //   borderWidth: 0.5,
  //   borderColor: "#BB6666",
  //   justifyContent: "center",
  //   alignItems: "center",
  //   marginTop: 20,
  //   opacity: 0.8,
  // },

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
