import firebase from "firebase/compat/app";

const firebaseConfig = {
  apiKey: "AIzaSyBIKQ6DzjZmbwrMfRdf5n19aLt8cFIwdt8",
  authDomain: "aquaponics-48aea.firebaseapp.com",
  databaseURL: "https://aquaponics-48aea-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "aquaponics-48aea",
};

firebase.initializeApp(firebaseConfig);
const db = firebase.database(app);
