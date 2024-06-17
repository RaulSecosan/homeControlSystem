import React from "react";
import { StatusBar } from "expo-status-bar";
import { NavigationContainer } from "@react-navigation/native";
import { createNativeStackNavigator } from "@react-navigation/native-stack";

import Home from "./components/pages/Home";
import Outside from "./components/pages/outside/Outside";
import Inside from "./components/pages/inside/InsideFirstPage";
import InsideSecondPage from "./components/pages/inside/InsideSecondPage";
import Sensors from "./components/pages/inside/Sensors";
import InsideLights from "./components/pages/inside/InsideLights";
import InsideDoors from "./components/pages/inside/InsideDoors";
import InsideWindows from "./components/pages/inside/InsideWindows";
import Login from "./components/pages/LoginRegister/Login";
import SignUp from "./components/pages/LoginRegister/SignUp";

const Stack = createNativeStackNavigator();

export default function App() {
  return (
    <>
      <StatusBar style="auto" />
      <NavigationContainer>
        <Stack.Navigator
          // initialRouteName="Login"
          initialRouteName="Home"
          screenOptions={{ headerShown: false }}
        >
          <Stack.Screen name="Login" component={Login} />
          <Stack.Screen name="SignUp" component={SignUp} />
          <Stack.Screen name="Home" component={Home} />
          <Stack.Screen name="Outside" component={Outside} />
          <Stack.Screen name="Inside" component={Inside} />
          <Stack.Screen name="InsideSecondPage" component={InsideSecondPage} />
          <Stack.Screen name="Sensors" component={Sensors} />
          <Stack.Screen name="InsideLights" component={InsideLights} />
          <Stack.Screen name="InsideDoors" component={InsideDoors} />
          <Stack.Screen name="InsideWindows" component={InsideWindows} />
        </Stack.Navigator>
      </NavigationContainer>
    </>
  );
}
