// Buttons
var authEmailPassButton = document.getElementById('authEmailPassButton');
var authGoogleButton = document.getElementById('authGoogleButton');
var authAnonymouslyButton = document.getElementById('authAnonymouslyButton');
var createUserButton = document.getElementById('createUserButton');
var logOutButton = document.getElementById('logOutButton');
var LinkFertirriga = document.getElementById('LinkFertirriga');

// Inputs
var emailInput = document.getElementById('emailInput');
var passwordInput = document.getElementById('passwordInput');

// Displays
var displayName = document.getElementById('displayName');

// Initialize the FirebaseUI Widget using Firebase.
//var ui = new firebaseui.auth.AuthUI(firebase.auth());



// Criar novo usuário
createUserButton.addEventListener('click',(e)=> {
    e.preventDefault();
    firebase
        .auth()
        .createUserWithEmailAndPassword(emailInput.value, passwordInput.value)
        .then((userCredential) => {
            // Signed in
            var user = userCredential.user;
           
            alert('Bem vindo ' + emailInput.value);
            LinkFertirriga.innerHTML='  <a class="text-center" id="LinkFertirriga" href="IFCfertirriga.html">Acesso Liberado ao Controle de Válvulas</a> '; 
        })
        .catch(function (error) {
            console.error(error.code);
            console.error(error.message);
            alert('Falha ao cadastrar, verifique o erro no console.')
        });
});

// Autenticar com E-mail e Senha
authEmailPassButton.addEventListener('click',(e)=> {
    e.preventDefault();
    firebase
        .auth()
        .signInWithEmailAndPassword(emailInput.value, passwordInput.value)
        .then((userCredential) => {
            // Signed in
            var user = userCredential.user;
            console.log(user);
            displayName.innerText = 'Bem vindo, ' + emailInput.value ;
            //inserir link Fertirriga
            LinkFertirriga.innerHTML='  <a class="text-center" id="LinkFertirriga" href="IFCfertirriga.html">Acesso Liberado ao Controle de Válvulas</a> '; 
                    
            alert('Autenticado ' + emailInput.value);
        })
        .catch(function (error) {
            console.error(error.code);
            console.error(error.message);
            alert('Falha ao autenticar, verifique o erro no console.')
        });
});

// Logout
logOutButton.addEventListener('click', (e)=> {
    e.preventDefault();
    firebase
        .auth()
        .signOut()
        .then(() => {
           
            displayName.innerText = 'Você não está autenticado';
            alert('Você se deslogou');
        }).catch((error) =>{ 
            console.error(error);
        });
});




// Autenticar Anônimo
authAnonymouslyButton.addEventListener('click', () => {
    firebase
        .auth()
        .signInAnonymously()
        .then(() => {
            
            displayName.innerText = 'Bem vindo, Autenticado Anonimamente';
            LinkFertirriga.innerHTML='  <a class="text-center" id="LinkFertirriga" href="IFCfertirriga.html">Acesso Liberado ao Controle de Válvulas</a> '; 
            alert('Autenticado Anonimamente');
        })
        .catch((error) => {
            console.error(error.code);
            console.error(error.message);
            alert('Falha ao autenticar, verifique o erro no console.')
        });
});



// Autenticar com Google
authGoogleButton.addEventListener('click', ()=> {
    // Providers
    var provider = new firebase.auth.GoogleAuthProvider();
    signIn(provider);
});


function signIn(provider) {
    firebase.auth()
        .signInWithPopup(provider)
        .then((result)=> {
            console.log(result);
            var token = result.credential.accessToken;
            displayName.innerText = 'Bem vindo, ' + result.user.displayName;
        }).catch(function (error) {
            console.log(error);
            alert('Falha na autenticação');
        });
}