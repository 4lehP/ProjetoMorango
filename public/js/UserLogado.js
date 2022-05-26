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
        SelectDataToTable(user.uid);    
    console.log(user.uid);
    console.log(user.displayName); 
    console.log(user.email); 
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

function SelectDataToTable(useruid) {
    
    firebase.database().ref("User/"+useruid+"/Projetos").on('value',    //  .on() define que a função ocorrerá sepre que um dado for alterado na tabela
        function (AllRecords) {
            while(tableList.length){
                List.pop(); //****Verificar oq está ocorrendo de errado**** 
            }
            var i=0;
            AllRecords.forEach(
                function (CurrentRecord) {
                    var Nome = CurrentRecord.val().Nome;
                    var Descrição = CurrentRecord.val().Descrição;
                    var Status = CurrentRecord.val().Status;
                    var Link= CurrentRecord.val().Link;
                    AddItemsToTable(Nome, Descrição, Status, Link) // Cria a tabela pela 
                    //tableList.push([Nome, Descrição, Status]);
                       

                  
                   // document.querySelectorAll("tbodyProjetosProjetos td:nth-child(4)")[i].innerText = Estado;
                 
                        
                }
            );
                        
        });
};

function AddItemsToTable(Nome, Descrição, Status, Link) {
    user = firebase.auth().currentUser;

    var tbodyProjetos = document.getElementById('tbodyProjetos');

    var trow = document.createElement('tr');

    var td1 = document.createElement('td');
    var td2 = document.createElement('td');
    var td3 = document.createElement('td');
    var td4 = document.createElement('td');

    tableList.push([Nome, Descrição, Status, Link]);

    td1.innerHTML = Nome;
    td2.innerHTML = Descrição;
    td3.innerHTML = Status;
    td4.innerHTML = '<a type="button" class="btn btn-primary my-2" href="IFCfertirriga.html"><i class="bi bi-cursor-fill"></i></a> ';
    //td4.innerHTML ='<a type="button" class="btn btn-primary my-2"   onclick="GotoLink(' + IndexTable + ')">Ir</a>';
    IndexTable++

    trow.appendChild(td1);
    trow.appendChild(td2);
    trow.appendChild(td3);
    trow.appendChild(td4);

    tbodyProjetos.appendChild(trow);


}

function GotoLink(IndexTable){
    Link = tableList[IndexTable][3];
}

logOutButton.addEventListener('click', (e)=> {
    e.preventDefault();
    firebase
        .auth()
        .signOut()
        .then(() => {
           
            displayName.innerText = 'Você não está autenticado';
            alert('Você se deslogou');
            window.location.href = '/public/authentication.html';
        }).catch((error) =>{ 
            console.error(error);
        });
});

