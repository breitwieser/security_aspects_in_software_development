/**
 * Creates a new <code>FormSubmitter</code> object. A
 * <code>FormSubmitter</code> handles form submission and validation. In
 * addition it provides a number of convenience methods to register the form
 * validation and submission to a number of events. The form submitter uses the
 * specified <code>Validators</code> to check the content of the form. If even
 * one invalidates the content, the specified submission function will not be
 * called, but form valiation continues anyway. If all <code>Validators</code>
 * return true, the specified submission function will be called.
 * 
 * @param formId
 *          the id of the form element to validate and submit
 * @param submitFunction
 *          the function to call after form validation. In most cases this
 *          function will submit the form.
 * @param validators
 *          an array of <code>Validator</code> objects. Each validator object
 *          can be used to validate one field of the form. See
 *          <code>Validator</code> for details.
 * @returns a new <code>FormSubmitter</code> object.
 * @author The SASE 2013 Team
 */
function FormSubmitter(formId, submitFunction, validators) {
  this.formId = formId;
  this.submitFunction = submitFunction;
  this.validators = validators;
}

/**
 * Handles form validation and calling of the submission function. This is the
 * function that gets registered with the register function familiy.
 */
FormSubmitter.prototype.submitForm = function() {
  var targetForm = document.getElementById(this.formId);
  var ok = true;
  for ( var i = 0; i < this.validators.length; ++i) {
    if (!this.validators[i].validate(targetForm)) {
      ok = false;
    }
  }
  if (ok) {
    this.submitFunction(targetForm);
  }
};

/**
 * A return key press handler. Useful to submit the form on the user pressing
 * enter in one of the fields.
 * 
 * @param event
 *          the key press event.
 */
FormSubmitter.prototype.onEnter = function(event) {
  var key = event.keyCode || event.which;
  if (key == 0x0D) {
    this.submitForm();
  }
};

/**
 * Register the <code>onEnter</code> handler instead of the specified
 * submission function, for the element with the given id. This function binds
 * the handler to this object, to allow access to the submission function.
 * 
 * @param elementid
 *          the id of the element for which to register the enter handler
 */
FormSubmitter.prototype.registerOnEnter = function(elementid) {
  var element = document.getElementById(elementid);
  if (element == null) {
    return;
  }
  element.addEventListener('keypress', this.onEnter.bind(this), false);
};

/**
 * Register the specified submission function for the element with the given id,
 * on the given event id. This function also binds the internal submission
 * function to this object, to allow access to the user specified submission
 * function.
 * 
 * @param eventId
 *          the event to listen to
 * @param elementId
 *          the id of the element for which to register the event handler
 */
FormSubmitter.prototype.register = function(eventId, elementId) {
  var element = document.getElementById(elementId);
  if (element == null) {
    return;
  }
  element.addEventListener(eventId, this.submitForm.bind(this), false);
};

/**
 * Creates a new <code>Validator</code> object. This validator implementation
 * provides a mechanism to check the content of one form field against a
 * specific regular expression. If the the regular expression does not match the
 * validator sets the innerHTML content of the element with the given error id
 * to the specified errorMsg.
 * 
 * @param inputId
 *          the id of the element to validate
 * @param pattern
 *          the regular expression pattern
 * @param errorId
 *          the id of the element that can take the error message
 * @param errorMsg
 *          the error message
 * @returns a new <code>Validator</code> object
 */
function Validator(inputId, pattern, errorId, errorMsg) {
  this.inputId = inputId;
  this.pattern = pattern;
  this.errorId = errorId;
  this.errorMsg = errorMsg;
}

/**
 * Checks the content of the form field identified by the given id against a
 * given regular expression. If the the regular expression does not match the
 * validator sets the innerHTML content of the element with the given error id
 * to the specified errorMsg.
 * 
 * @param form
 *          the parent form containing the fields to validate
 * @returns {Boolean} true if the validation succeeds; false otherwise
 */
Validator.prototype.validate = function(form) {
  var field = document.getElementById(this.inputId);
  var error = document.getElementById(this.errorId);
  if (!field.value.match(this.pattern)) {
    error.innerHTML = this.errorMsg;
    return false;
  } else {
    error.innerHTML = "";
    return true;
  }
};

/**
 * Register the specified function for the element with the given id, on the
 * given event id.
 * 
 * @param eventId
 *          the event to listen to
 * @param elementId
 *          the id of the element for which to register the event handler
 * @param the event handler function
 */
function registerEventHandler(eventId, elementId, handler) {
  var element = document.getElementById(elementId);
  if (element == null) {
    return;
  }
  element.addEventListener(eventId, handler, false);
};