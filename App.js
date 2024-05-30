import { StatusBar } from "expo-status-bar";
import { NavigationContainer } from "@react-navigation/native";
import { createNativeStackNavigator } from "@react-navigation/native-stack";

import Home from "./components/pages/Home";
import Outside from "./components/pages/outside/Outside";
import Bec from "./components/SensorsCommunication";
import Inside from "./components/pages/inside/InsideFirstPage";
const Stack = createNativeStackNavigator();

export default function App() {
  return (
    <>
      <StatusBar style="auto" />

      <NavigationContainer>
        <Stack.Navigator
          initialRouteName="Home"
          screenOptions={{ headerShown: false }}
        >
          <Stack.Screen name="Home" component={Home} />
          <Stack.Screen name="Outside" component={Outside} />
          <Stack.Screen name="Inside" component={Inside} />
        </Stack.Navigator>
      </NavigationContainer>
      {/* <Bec/> */}
    </>
  );
}
