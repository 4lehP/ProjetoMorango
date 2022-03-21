var usersList = document.getElementById('usersList');
var nameInput = document.getElementById('nameInput');
var ageInput = document.getElementById('ageInput');
var addButton = document.getElementById('addButton');
var VAdb1flexSwitch = document.getElementById('VAdb1flexSwitch');
var Vadb1label = document.getElementById('Vadb1label');

setInterval(VerDigitais, 4*1000);



//var db = firebase.database()
// Pega o objeto para manipular os dados
//var db = firebase.firestore();
var database = firebase.database();

//firebase.database().ref().child('users').push(data);
function AttDigitais(name, status) {
    
 
   // var newPostKey = firebase.database().ref().child('Digitais/').push().key;

    var updates = {};
    updates['Digitais/' + name +'/Estado' ] = status;
    return firebase.database().ref().update(updates);
    }

    function VerDigitais() {
       // alert("10 seg");
      
       
      firebase.database().ref('Digitais/VAdb1').on('value', (snapshot) => {
    
            
             Vadb1label.innerHTML = snapshot.val().Estado;
          
        });
       /*


        firebase.database().ref('Digitais/VAdb1').on('value', (snapshot) => {
    
            snapshot.forEach( function childSnapshot (item) {
             console.log(item.val().Descrição+ " " + item.val().Estado);
             Vadb1label.innerHTML = item.val().Estado;
            });
    
        });*/
       
    };
   
   
   
VAdb1flexSwitch.addEventListener('click', function () {
    if(VAdb1flexSwitch.checked){
        alert('Ligando Válvula!');
        AttDigitais("VAdb1","Ligado") ;
    }else{
        AttDigitais("VAdb1","Desligado");
    }
 
});





