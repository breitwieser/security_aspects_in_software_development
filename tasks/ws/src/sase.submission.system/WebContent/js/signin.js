var nameValidator = new Validator(
    'name',
    /^[\w\d-_ ]{1,20}$/,
    'name_error_desc',
    'The username must be between 1 and 20 characters. Allowable characters are alphanumerical values plus \'-\',\'_\', and \'=\'.');

var passValidator = new Validator(
    'plainpass',
    /^[\w\d]{6,20}$/,
    'pass_error_desc',
    'Password must be between 6 and 20 characters and may only contain alphanumeric characters!');

var emailValidator = new Validator('email', /^\w+(.\w+)*@\w+(.\w+)*$/,
    'email_error_desc',
    'Email must be a valid email address of the form name(.more)@domain(.more).');

var groupValidator = new Validator('group', /[\d]{1,3}/, 'goup_error_desc',
    'Group must be a number between 1 and 999');

function signIn() {
  var plainPass = document.getElementById('plainpass');
  var salt = document.getElementById('salt');
  var iter = document.getElementById('iter');
  var digestPass = document.getElementById('pass');
  var digestedPassword = digestPassword(plainPass.value, salt.value, iter.value);
  digestPass.value = digestedPassword.toString();
  plainPass.value = "";
  var login_form = document.getElementById("signin_form");
  login_form.submit();
}

var signInSubmitter = new FormSubmitter('signin_form', signIn, [ nameValidator,
    passValidator, emailValidator, groupValidator ]);

function init() {
  signInSubmitter.registerOnEnter('group');
  signInSubmitter.registerOnEnter('signin_form_submit_button');
  signInSubmitter.register('click', 'signin_form_button');
}

window.addEventListener('load', init, false);
