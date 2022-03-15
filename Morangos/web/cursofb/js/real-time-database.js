var usersList = document.getElementById('usersList');
var nameInput = document.getElementById('nameInput');
var ageInput = document.getElementById('ageInput');
var addButton = document.getElementById('addButton');


//var db = firebase.database()
// Pega o objeto para manipular os dados
var db = firebase.firestore();

//const db = firebase.firestore();
addButton.addEventListener('click',(e)=> {
    e.preventDefault();
    create(nameInput.value, ageInput.value);
})
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

db.collection('Cadastros').orderBy("name").onSnapshot(function(data){
    usersList.innerHTML = 'Nome Idade'+`<br>`;
    data.docs.map(function(item){
        usersList.innerHTML+= `${item.data().name}` +" "+`${item.data().age}<br>`;
      
    });
});

