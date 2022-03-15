var usersList = document.getElementById('usersList');
var nameInput = document.getElementById('nameInput');
var ageInput = document.getElementById('ageInput');
var addButton = document.getElementById('addButton');
var VAdb1flexSwitch = document.getElementById('VAdb1flexSwitch');


//var db = firebase.database()
// Pega o objeto para manipular os dados
//var db = firebase.firestore();
var database = firebase.database();

//firebase.database().ref().child('users').push(data);
function AttDigitais(name, status) {
    firebase.database().ref('Digitais/').child(name).set({
      estado: status
           });
  }


VAdb1flexSwitch.addEventListener('click', function () {
    if(VAdb1flexSwitch.checked){
        alert('Ligando Válvula!');
        AttDigitais("VAdb1","Ligado") ;
    }else{
        AttDigitais("VAdb1","Desligado");
    }
 
});




firebase.database().ref('Digitais/').on('value', (snapshot) => {
    
    snapshot.forEach( function childSnapshot)  {
     console.log(item.val().estado);
    });

  // var data = snapshot.val();
  //console.log(data);
 // alert(data.estado);
});


firebase.database().ref('users').on('value', function (snapshot) {
    usersList.innerHTML = '';
    snapshot.forEach(function (item) {
        var li = document.createElement('li');
        li.appendChild(document.createTextNode(item.val().name + ': ' + item.val().age));
        usersList.appendChild(li);
    });
});
