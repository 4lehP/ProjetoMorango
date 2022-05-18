//var valveList = document.getElementById('valveList');
var valveList = [];
var IndexTable = 0;
var code = document.getElementById('code');
var description = document.getElementById('description');
var SwitchEstado = document.getElementById('SwitchEstado'); 
var Vadb1label = document.getElementById('Vadb1label');
var tbody = document.getElementById('tbody');
var IMGphotoURL = document.getElementById('photoURL');
var logOutButton = document.getElementById('logOutButton');
var UsuarioAtivo = document.getElementById('UsuarioAtivo');
var ControladorAtivo = document.getElementById('ControladorAtivo');    
var primeiro = true;
window.onload = SelectDataToTable('Digitais/',1);
window.onload = AtualizaStatusDispositivo('Dispositivos/ESP32/') ;
window.onload = SetDispOffline('Dispositivos/ESP32');


var user = firebase.auth().currentUser;
timerUser = setInterval(ftimer , 2000);

function ftimer(){
    user = firebase.auth().currentUser;
    if(user.uid){
       UsuarioAtivo.innerText= 'Usuário: '+user.displayName+'.';
       LoadDataFromUser(user.uid); 
       console.log(user.uid);
        console.log(user.displayName);
        ArmazenarDadosUsuario(user); 
        clearInterval(timerUser); 
    }
   
}

function LoadDataFromUser(useruid) {
    firebase.database().ref("User/"+useruid).on('value',    //  .on() define que a função ocorrerá sepre que um dado for alterado na tabela
        function (Record) {
             var displayName = Record.val().displayName;
             var email = Record.val().email;
             var photoURL =  Record.val().photoURL;
             console.log(photoURL);
             IMGphotoURL.src = photoURL; 
            console.log(displayName);
        });
};
function ArmazenarDadosUsuario(user){
    console.log("armazena");
    console.log(user.uid);
    console.log(user.displayName);
    console.log(user.email);
    console.log(user.phoneNumber);
    console.log(user.photoURL);
    console.log(user);
    var dataUser = {
        uid: user.uid,
        displayName: user.displayName,
        email: user.email,
        photoURL: user.photoURL,
    };
    
    firebase.database().ref("User/"+user.uid).update(dataUser);



    
    
};

timerDisp = setInterval(ftimerDisp , 10000);//
function ftimerDisp(){
    SetDispOffline('Dispositivos/ESP32/'); 
}

function SelectDataToTable(colection, first) {
    
    firebase.database().ref(colection).on('value',    //  .on() define que a função ocorrerá sepre que um dado for alterado na tabela
        function (AllRecords) {
            while(valveList.length) {// Limpa a lista de válvulas para os botões poderem se atualizar
                valveList.pop();
              }
            var i=0;
            AllRecords.forEach(
                function (CurrentRecord) {
                    var Nome = CurrentRecord.val().Nome;
                    var Descrição = CurrentRecord.val().Descrição;
                    var Estado = CurrentRecord.val().Estado;
                    if(first){
                    AddItemsToTable(Nome, Descrição, Estado) // Cria a tabela pela 
                    }else{valveList.push([Nome, Descrição, Estado]);
                        

                    }
                    document.querySelectorAll("tbody td:nth-child(4)")[i].innerText = Estado;
                             
                             console.log(i +" "+Estado);
                             i++;
                        
                }
            );
            first =false
        });
};

function AddItemsToTable(Nome, Descrição, Estado) {
    user = firebase.auth().currentUser;

    var tbody = document.getElementById('tbody');

    var trow = document.createElement('tr');

    var td1 = document.createElement('td');
    var td2 = document.createElement('td');
    var td3 = document.createElement('td');
    var td4 = document.createElement('td');

    valveList.push([Nome, Descrição, Estado]);

    td1.innerHTML = Nome;
    td2.innerHTML = Descrição;
    td3.innerHTML = '<button type="button" class="btn btn-primary my-2"   onclick="AttDigitais(' + IndexTable + ')">On/Off</button>';
    td4.innerHTML = Estado;
    IndexTable++

    trow.appendChild(td1);
    trow.appendChild(td2);
    trow.appendChild(td3);
    trow.appendChild(td4);

    tbody.appendChild(trow);


}



var contOFFLINE=0;

function AtualizaStatusDispositivo(colection) {
    firebase.database().ref(colection).on('value',    //  .on() define que a função ocorrerá sepre que um dado for alterado na tabela
        function (Record) {
             var Estado = Record.val().Estado;
             if(Estado=="Offline")contOFFLINE++;
             if(Estado=="Online"){
                 contOFFLINE=0;
                 
                 ControladorAtivo.innerText= 'Dispositivo ESP32 está '+Estado+'. ';
             }
             if(contOFFLINE>2){
                    
                    contOFFLINE=0;
                    ControladorAtivo.innerText= 'Dispositivo ESP32 está '+Estado+'. ';
             }
            
                    console.log(Estado);   
        });
};
function SetDispOffline(colection) {
    
    var isOfflineForDatabase = {
        Estado: 'Offline',
        last_changed: firebase.database.ServerValue.TIMESTAMP,
    };
    firebase.database().ref(colection).set(isOfflineForDatabase);
    
};



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
  
  return 0;
     

} 

// Create a reference to this user's specific status node.
// This is where we will store data about being online/offline.
var userStatusDatabaseRef = firebase.database().ref('/status/' + user.uid);

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

logOutButton.addEventListener('click', (e) => {
    e.preventDefault();
    firebase
        .auth()
        .signOut()
        .then(() => {
            UsuarioAtivo.innerText= 'Você não está autenticado';
            alert('Você se deslogou');
            window.location.href = '/public/authentication.html';
        }).catch((error) => {
            console.error(error);
        });
});




