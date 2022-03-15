var usersList = document.getElementById('usersList');
var nameInput = document.getElementById('nameInput');
var ageInput = document.getElementById('ageInput');
var addButton = document.getElementById('addButton');
var VAdb1flexSwitch = document.getElementById('VAdb1flexSwitch');


//var db = firebase.database()
// Pega o objeto para manipular os dados
var db = firebase.firestore();


VAdb1flexSwitch.addEventListener('click', function () {
    if(VAdb1flexSwitch.checked){
        alert('check');
        AttDigitais("VAdb1","true");
    }else{
        AttDigitais("VAdb1","false");
    }
 
});




function AttDigitais(name, status) {
    var data = {
        status: status
    };
   // alert('send')
   return db.collection('Digitais').doc(name).set(data);
}




;
// Função para criar um registro no Firebase 
function create(name, age) {
    var data = {
        name: name,
        age: age
    };
   // alert('send')
   return db.collection('Cadastros').add(data);
}




    
//const db = firebase.firestore();
addButton.addEventListener('click',(e)=> {
    e.preventDefault();
    create(nameInput.value, ageInput.value);
});

