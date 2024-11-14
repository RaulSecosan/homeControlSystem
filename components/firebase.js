// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getDatabase } from "firebase/database";

const firebaseConfig = {
  apiKey: "AIzaSyCVz5625ik6fPC0nNe04JLAAfIwqGz5lzU",
  authDomain: "licentalivedb.firebaseapp.com",
  projectId: "licentalivedb",
  storageBucket: "licentalivedb.firebasestorage.app",
  messagingSenderId: "777565726696",
  appId: "1:777565726696:web:e16f5d0902ec521a93a65d",
  measurementId: "G-E3YPRSSWHP"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);

// Initialize Realtime Database and export
export const database = getDatabase(app);


