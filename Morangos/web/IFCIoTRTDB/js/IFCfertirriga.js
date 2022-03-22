var valveList = document.getElementById('valveList');
var code = document.getElementById('code');
var description = document.getElementById('description');
var addButton = document.getElementById('addButton');
var selButton = document.getElementById('selButton');
var delButton = document.getElementById('delButton');
var VAdb1flexSwitch = document.getElementById('VAdb1flexSwitch');
var Vadb1label = document.getElementById('Vadb1label');
var tbody = document.getElementById('tbody');


//setInterval(VerDigitais, 4*1000);


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
              
    };
   
   
   


function SelectAllDataFrom(colection){
    firebase.database().ref(colection).once('value',
    function (AllRecords){
        AllRecords.forEach(
            function(CurrentRecord){
                var Nome = CurrentRecord.val().Nome;
                var Descrição = CurrentRecord.val().Descrição;
                var Estado =CurrentRecord.val().Estado;
                AddItemsToTable(Nome,Descrição,Estado);
            }
        );
    });
};

window.onload= SelectAllDataFrom('Digitais/');

function AddItemsToTable(Nome,Descrição,Estado){
    var tbody = document.getElementById('tbody');
    var trow = document.createElement('tr');
   //trow.setAttribute("table-active") ;
    var td1 = document.createElement('td');
    var td2 = document.createElement('td');
    var td3 = document.createElement('td');
    var td4 = document.createElement('td');
    td1.innerHTML = Nome;
    td2.innerHTML = Descrição;
    td3.innerHTML = "botaozinho";
    td4.innerHTML = Estado;
    trow.appendChild(td1); 
    trow.appendChild(td2); 
    trow.appendChild(td3); 
    trow.appendChild(td4); 

    var ControlDiv = document.createElement("div");
    ControlDiv.innerHTML = '<button type="button" class="btn btn-primary" data-toggle="modal" data-target="#exampleModalCenter">'
    ControlDiv.innerHTML += '<button type="button" class="btn btn-primary" data-toggle="modal" data-target="#exampleModalCenter">'

    trow.appendChild(ControlDiv);
    tbody.appendChild(trow);

}

/*
function Ready(){
    code = document.getElementById('code').value;
    description = document.getElementById('description').value;
}

//------------- Adicionar ------------------------------------------
document.getElementById('addButton').onclick = function(){
    Ready();
    firebase.database().ref('Digitais/'+code).set({
        Descrição: description,
        Estado: 'Desligado',
        Nome: code
    });
}

//------------- Selecionar ------------------------------------------
document.getElementById("selButton").onclick = function(){
    Ready();
    firebase.database().ref('Digitais/'+code).once('value', function(snapshot){
        snapshot.forEach(function(childSnapsht){
            code = document.getElementById('code').value = snapshot.val().Nome;
             description = document.getElementById('description').value = snapshot.val().Descrição;
        });
        
    });
}

//------------- Deletar ------------------------------------------
document.getElementById('delButton').onclick = function(){
    Ready();
    firebase.database().ref('Digitais/'+code).remove({

    });
}
*/

