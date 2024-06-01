import {
  View,
  Text,
  StyleSheet,
  TextInput,
  Button,
  ActivityIndicator,
  KeyboardAvoidingView,
} from "react-native";
import React, { useState } from "react";
import { FIREBASE_AUTH } from "../../FirebaseConfig";
import { createUserWithEmailAndPassword } from "firebase/auth";

const SignUp = ({ navigation }) => {
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [repeatPassword, setRepeatPassword] = useState("");
  const [loading, setLoading] = useState(false);

  const signUp = async () => {
    if (password !== repeatPassword) {
      alert("Passwords aren't the same");
      return;
    }

    setLoading(true);
    try {
      const response = await createUserWithEmailAndPassword(
        FIREBASE_AUTH,
        email,
        password
      );
      console.log(response);
      alert("Sign Up successful!");
    } catch (error) {
      console.log(error);
      alert("Sign up failed " + error.message);
    } finally {
      setLoading(false);
    }
  };

  return (
    <View style={styles.container}>
      <KeyboardAvoidingView behavior="padding">
        <Text>Sign Up</Text>
        <TextInput
          value={email}
          style={styles.input}
          placeholder="Email"
          autoCapitalize="none"
          onChangeText={(text) => setEmail(text)}
        />
        <TextInput
          secureTextEntry={true}
          value={password}
          style={styles.input}
          placeholder="Password"
          autoCapitalize="none"
          onChangeText={(text) => setPassword(text)}
        />
        <TextInput
          secureTextEntry={true}
          value={repeatPassword}
          style={styles.input}
          placeholder="Repeat Password"
          autoCapitalize="none"
          onChangeText={(text) => setRepeatPassword(text)}
        />
        {loading ? (
          <ActivityIndicator size="large" color="#0000ff" />
        ) : (
          <>
            <Button title="Create account" onPress={signUp} />
            <Button
              title="Log In"
              onPress={() => navigation.navigate("Login")}
            />
          </>
        )}
      </KeyboardAvoidingView>
    </View>
  );
};

export default SignUp;

const styles = StyleSheet.create({
  container: {
    marginHorizontal: 20,
    flex: 1,
    justifyContent: "center",
  },
  input: {
    marginVertical: 4,
    height: 50,
    borderWidth: 1,
    borderRadius: 4,
    padding: 10,
    backgroundColor: "#fff",
  },
});
