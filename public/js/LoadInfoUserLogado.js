// Buttons

var logOutButton = document.getElementById('logOutButton');
var LinkFertirriga = document.getElementById('LinkFertirriga');

// Inputs
var emailInput = document.getElementById('emailInput');
var passwordInput = document.getElementById('passwordInput');

// Displays
var displayName = document.getElementById('displayName');

//img
var IMGphotoURL = document.getElementById('photoURL');

//Variaveis da tabela
var tableList = [];
var IndexTable = 0;
// Initialize the FirebaseUI Widget using Firebase.
//var ui = new firebaseui.auth.AuthUI(firebase.auth());


var user;
timerUser = setInterval(ftimer , 2000);//location.reload()

function ftimer(){
    user = firebase.auth().currentUser;
    if(user.uid){
       
        displayName.innerText = 'Bem vindo, ' + user.displayName;
        
        LoadDataFromUser(user.uid);
           
    console.log(user.uid);
    console.log(user.displayName); 
    console.log(user.email); 
    
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

 