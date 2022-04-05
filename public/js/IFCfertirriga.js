//var valveList = document.getElementById('valveList');
var valveList = [];
var IndexTable = 0;
var code = document.getElementById('code');
var description = document.getElementById('description');
var SwitchEstado = document.getElementById('SwitchEstado'); 
var Vadb1label = document.getElementById('Vadb1label');
var tbody = document.getElementById('tbody');
window.onload = SelectAllDataFrom('Digitais/');




timer = setInterval(ftimer , 10000);//location.reload()
function ftimer(){
    //console.log("timer");
    SelectAllDataToValveList('Digitais/'); /// da pra reduzir
    SelectAllEstados('Digitais/');
}

//var db = firebase.database()
// Pega o objeto para manipular os dados
//var db = firebase.firestore();
var database = firebase.database();

//firebase.database().ref().child('users').push(data);
/*
firebase.database().ref('Digitais/VAdb1').on('value', (snapshot) => {
    
    snapshot.forEach( function childSnapshot (item) {
     console.log(item.val().Descrição+ " " + item.val().Estado);
     Vadb1label.innerHTML = item.val().Estado;
    });

});
*/

function VerDigitais() {
    // alert("10 seg");


    firebase.database().ref('Digitais/').on('value', (snapshot) => {
        var tbody = document.getElementById('tbody');
        tbody ="";
        Vadb1label.innerHTML = snapshot.val().Estado;
        
    });

};





function SelectAllDataFrom(colection) {
    //document.getElementById("tbody1").innerHTML="";

    firebase.database().ref(colection).once('value',
        function (AllRecords) {
            AllRecords.forEach(
                function (CurrentRecord) {
                    var Nome = CurrentRecord.val().Nome;
                    var Descrição = CurrentRecord.val().Descrição;
                    var Estado = CurrentRecord.val().Estado;
                    AddItemsToTable(Nome, Descrição, Estado);
                }
            );
        });
};


function SelectAllDataToValveList(colection) {
    //document.getElementById("tbody1").innerHTML="";
    
    firebase.database().ref(colection).once('value',
        function (AllRecords) {
            
            while(valveList.length) {
                valveList.pop();
              }
            AllRecords.forEach(
                function (CurrentRecord) {
                    var Nome = CurrentRecord.val().Nome;
                    var Descrição = CurrentRecord.val().Descrição;
                    var Estado = CurrentRecord.val().Estado;
                    valveList.push([Nome, Descrição, Estado]);
                }
            );
        });
};




//var senha = window.prompt("Insira a senha");

//if (senha == 'inovamorango') {
    function AddItemsToTable(Nome, Descrição, Estado) {
        var tbody = document.getElementById('tbody');
        
        var trow = document.createElement('tr');
        //trow.setAttribute("table-active") ;
        var td1 = document.createElement('td');
        var td2 = document.createElement('td');
        var td3 = document.createElement('td');
        var td4 = document.createElement('td');
            
        valveList.push([Nome, Descrição, Estado]);
     
        td1.innerHTML = Nome;
        td2.innerHTML = Descrição;
        td3.innerHTML = '<button type="button" class="btn btn-primary my-2"   onclick="AttDigitais('+IndexTable+')">On/Off</button>';
        td4.innerHTML = Estado;
        IndexTable++
    
        trow.appendChild(td1);
        trow.appendChild(td2);
        trow.appendChild(td3);
        trow.appendChild(td4);
    
        tbody.appendChild(trow);
    
    }
//}
//else{
//    window.alert("Senha Incorreta!");
//}

function SelectAllEstados(colection) {
    firebase.database().ref(colection).once('value',
        function (AllRecords) {
            var i=0;
            AllRecords.forEach(
                
                function (CurrentRecord, posição) {
                    var Estado = CurrentRecord.val().Estado;
                     document.querySelectorAll("tbody td:nth-child(4)")[i].innerText = Estado;
                     
                     console.log(i +" "+Estado);
                     i++;
                }
            );
        });
};


function AttEstados(Estado) {
    var tbody = document.getElementById('tbody');
    document.querySelectorAll("tbody td:nth-child(1)")[1].innerText = Estado;
   // document.querySelectorAll("tbody td:nth-child(1)")[1].innerHTML.style.color = 'blue'; //tentativa de colcar cor
   
}

function ReloadTable(){
    tbody.innerHTML = "";
    SelectAllDataFrom('Digitais/');
    //alert("table ReloadTable");
   

}

function Ready() {
    code = document.getElementById('code').value;
    description = document.getElementById('description').value;
}




function AttDigitais(IndexTable) {
    // var newPostKey = firebase.database().ref().child('Digitais/').push().key;
    name = valveList[IndexTable][0];
    status = valveList[IndexTable][2];
    console.log(status);
    console.log(name);
    if (status =="Desligado") {
        status="Ligado";
    } else {
        status ="Desligado";
    }
    console.log("depois");
    console.log(status);

    // Get a key for a new Post.
  
  var updates = {};
  updates['Digitais/' + name +'/Estado'] = status;
  firebase.database().ref().update(updates);
  SelectAllDataToValveList('Digitais/');
  SelectAllEstados('Digitais/');
  return 0;//ReloadTable()
  
     

}  
    

       

// Fetch the current user's ID from Firebase Authentication.
var uid = firebase.auth().currentUser.uid;

console.log(uid);

// Create a reference to this user's specific status node.
// This is where we will store data about being online/offline.
var userStatusDatabaseRef = firebase.database().ref('/status/' + uid);

// We'll create two constants which we will write to
// the Realtime database when this device is offline
// or online.
var isOfflineForDatabase = {
    state: 'offline',
    last_changed: firebase.database.ServerValue.TIMESTAMP,
};

var isOnlineForDatabase = {
    state: 'online',
    last_changed: firebase.database.ServerValue.TIMESTAMP,
};

// Create a reference to the special '.info/connected' path in
// Realtime Database. This path returns `true` when connected
// and `false` when disconnected.
firebase.database().ref('.info/connected').on('value', function(snapshot) {
    // If we're not currently connected, don't do anything.
    if (snapshot.val() == false) {
        return;
    };

    // If we are currently connected, then use the 'onDisconnect()'
    // method to add a set which will only trigger once this
    // client has disconnected by closing the app,
    // losing internet, or any other means.
    userStatusDatabaseRef.onDisconnect().set(isOfflineForDatabase).then(function() {
        // The promise returned from .onDisconnect().set() will
        // resolve as soon as the server acknowledges the onDisconnect()
        // request, NOT once we've actually disconnected:
        // https://firebase.google.com/docs/reference/js/firebase.database.OnDisconnect

        // We can now safely set ourselves as 'online' knowing that the
        // server will mark us as offline once we lose connection.
        userStatusDatabaseRef.set(isOnlineForDatabase);
    });
});


