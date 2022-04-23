var usersList = document.getElementById('usersList');
var nameInput = document.getElementById('nameInput');
var ageInput = document.getElementById('ageInput');
var addButton = document.getElementById('addButton');
var VAdb1flexSwitch = document.getElementById('VAdb1flexSwitch');
var Vadb1label = document.getElementById('Vadb1label');


firebase.database().ref('User/Uid/Projetos').on('value', (snapshot) => {
    Vadb1label.innerHTML = snapshot.val().Estado;

});

function SelectAllDataFrom(colection){
    firebase.database().ref(colection).once('value',
    function (AllRecords){
        AllRecords.forEach(
            function(CurrentRecord){
                var Projeto = CurrentRecord.val().Projeto;
                var Descrição = CurrentRecord.val().Descrição;
                var Autorização =CurrentRecord.val().Autorização;
                AddItemsToTable(Projeto,Descrição,Autorização);
            }
        );
    });
};

window.onload= SelectAllDataFrom('User/Uid/Projetos');

function AddItemsToTable(Projeto,Descrição,Autorização){
    var tbody = document.getElementById('tbody');
    var trow = document.createElement('tr');
    var td1 = document.createElement('td');
    var td2 = document.createElement('td');
    var td3 = document.createElement('td');
    
    td1.innerHTML = Projeto;
    td2.innerHTML = Descrição;
    td3.innerHTML = Autorização;
    
    trow.appendChild(td1); 
    trow.appendChild(td2); 
    trow.appendChild(td3);  
    tbody.appendChild(trow);

}
