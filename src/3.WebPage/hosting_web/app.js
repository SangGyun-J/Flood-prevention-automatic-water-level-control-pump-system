(function () {
  var firebaseConfig = {
    apiKey: "AIzaSyD7_tTx6Vt2F_LoRdRg2wc0-uHBWZd1Ukc",
    authDomain: "esp8266-sensor-readings-23fd0.firebaseapp.com",
    databaseURL:
      "https://esp8266-sensor-readings-23fd0-default-rtdb.firebaseio.com",
    projectId: "esp8266-sensor-readings-23fd0",
    storageBucket: "esp8266-sensor-readings-23fd0.appspot.com",
    messagingSenderId: "128895296309",
    appId: "1:128895296309:web:d11ab9b945c4407ba0a27c",
    measurementId: "G-RRYMTCG7N7",
  };
  // Initialize Firebase
  firebase.initializeApp(firebaseConfig);
  //testPump_in_server_DB
  // document에서 element를 가져와서 태그 아이디로 database_test를 넘긴다.
  const preObject = document.getElementById("testPump_in_server_DB");
  const AUTO = document.getElementById("AUTO");
  const PUMP = document.getElementById("PUMP");

  //데이터베이스 참조시키기. ref는 db의 메인루트로 접근하게하고 .child는 메인루트의 객체를 생성한다.
  const dbRefObject = firebase.database().ref("testPump_in_server_DB");
  //const auto1 = firebase.database().ref().child('AUTO');
  //const pump1 = firebase.database().ref().child('PUMP');
  //const dbRefObject = firebase.database().ref().child('testPump_in_server_DB');
  //const AUTOLIST = firebase.database().ref().child('AUTO');
  //const PUMPLIST = firebase.database().ref().child('PUMP');

  //var user = document.getElementById('testPump_in_server_DB').value;

  /* const tempRef = database.ref('DHT22');
    tempRef.limitToLast(1).on('value', function(snapshot) {
         snapshot.forEach(function(childSnapshot) {
             var childData = childSnapshot.val().Date;
             var timeData  = childSnapshot.val().AUTO;
             var temp      = childSnapshot.val().PUMP;
             tempElement.innerText = childData;
         });
     });
   */

  // const AUTOLIST = dbRefObject.child('AUTO');
  // const PUMPLIST = dbRefObject.child('PUMP');

  // (on 속성으로 데이터들을 실시간 업데이트한다, once 속성도 있다.)
  //value.on('이벤트타입', 콜백함수)
  //snap은 데이터 스냅샷. 키 이름, 자식요소, 반복등을 반환한다.
  //snap.val() -> 스냅의 값을 얻기 위해서 .val() 함수를 호출한d.

  dbRefObject.on("value", (snap) => {
    var autoval = snap.val().AUTO;
    var pumpval = snap.val().PUMP;
    var dis = snap.val().DISTANCE;

    document.getElementById("DISTANCE").innerHTML = dis;
    document.getElementById("AUTO").innerHTML = autoval;
    document.getElementById("PUMP").innerHTML = pumpval;
    console.log(autoval);

    if (autoval == '"OFF"') {
      document.getElementById("AUTO").style.color = "red";
    } else {
      document.getElementById("AUTO").style.color = "blue";
    }

    if (pumpval == '"OFF"') {
      document.getElementById("PUMP").style.color = "red";
    } else {
      document.getElementById("PUMP").style.color = "blue";
    }
    //preObject.innerHTML = snap.val()
  });

  // auto1.on('child_changed', snap => {
  //     AUTO.innerHTML = snap.val()
  //  });

  // pump1.on('child_changed', snap => {
  //     PUMP.innerHTML = snap.val()
  // });
  //AUTOLIST.on('child_changed', snap => {

  //});

  /*dbRefObject.on('value & child_changed & child', snap => {
        preObject.innerText = snap.val() 
     }); */
  /*dbRefObject.on('value', (snapshot) => {
        preObject.innerText = snapshot.val();
    });*/

  /*dbRefObject.on('value', snap => {
        preObject.innerText = JSON.stringify(snap.val(), null, 3)
    }); */
})();
