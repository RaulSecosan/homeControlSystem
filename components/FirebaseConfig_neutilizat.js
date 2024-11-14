// // Import the functions you need from the SDKs you need
// import { FirebaseError, initializeApp } from "firebase/app";
// import { getAuth } from "firebase/auth";

// const firebaseConfig = {
//   apiKey: "AIzaSyAvcUd29QMSMorERkiKnoLfFQN02P6Xuag",
//   authDomain: "licentaauth.firebaseapp.com",
//   projectId: "licentaauth",
//   storageBucket: "licentaauth.appspot.com",
//   messagingSenderId: "886168889574",
//   appId: "1:886168889574:web:17090889e7729ec895b327"
// };

// // Initialize Firebase
// export const FIREBASE_APP = initializeApp(firebaseConfig);
// export const FIREBASE_AUTH = getAuth(FIREBASE_APP);

// Import the functions you need from the SDKs you need
import { initializeApp } from 'firebase/app';
import { initializeAuth, getReactNativePersistence } from 'firebase/auth';
import AsyncStorage from '@react-native-async-storage/async-storage';

// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyAvcUd29QMSMorERkiKnoLfFQN02P6Xuag",
  authDomain: "licentaauth.firebaseapp.com",
  projectId: "licentaauth",
  storageBucket: "licentaauth.appspot.com",
  messagingSenderId: "886168889574",
  appId: "1:886168889574:web:17090889e7729ec895b327"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);

// Initialize Firebase Auth with AsyncStorage persistence
export const FIREBASE_AUTH = initializeAuth(app, {
  persistence: getReactNativePersistence(AsyncStorage)
});
