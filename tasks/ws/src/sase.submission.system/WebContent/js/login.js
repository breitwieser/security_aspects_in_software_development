function securePassword(name, pass, target) {
  server = createServerUrlFromPath(getContextPath());
  var fetcher = new Condiments(server, name.value);
  fetcher.fetchCondiments();
  var digestedPassword = digestPassword(pass.value, fetcher.salt, fetcher.iter);
  target.value = digestedPassword.toString();
}

var nameValidator = new Validator(
    'j_username',
    /[\w\d-_=]{1,20}/,
    'name_error_desc',
    'The username must be between 1 and 20 characters.'
        + 'Allowable characters are alphanumerical values plus \'-\',\'_\', and \'=\'.');

var passValidator = new Validator(
    'password',
    /[\w\d]{6,20}/,
    'pwd_error_desc',
    'Password must be between 6 and 20 characters and may only contain alphanumeric characters!');

function logIn() {
  var name = document.getElementById("j_username");
  var pass = document.getElementById("password");
  var target = document.getElementById("j_password");
  securePassword(name, pass, target);
  pass.value = "";
  var login_form = document.getElementById("login_form");
  login_form.submit();
}

var logIn = new FormSubmitter('login_form', logIn, [ nameValidator,
    passValidator ]);

function init() {
  logIn.registerOnEnter('password');
  logIn.registerOnEnter('login_form_submit_button');
  logIn.register('click', 'login_form_button');
}

window.addEventListener('load', init, false);