function printResponse(sendData) {
  var data = eval('(' + sendData.responseText + ')');
  var reportArea = document.getElementById('program_report');
  var gen = new TableGenerator('program_report_table',
      'table_head table_head_yellow',
      'table_odd table_td_yellow table_odd_yellow',
      'table_even table_td_yellow table_even_yellow', 'program_report_row');
  gen.begin('table_yellow');
  gen.beginHeaderRow();
  gen.envelopHead('Message');
  gen.endHeaderRow();
  gen.beginBodyWithClass("table_body");
  for ( var i = 0; i < data.Log.length; i++) {
    var msg = data.Log[i];
    gen.beginRow();
    gen.envelopWithAdditionalClass(msg, 'submission');
    gen.endRow();
  }
  gen.endBody();
  gen.end();
  reportArea.innerHTML = gen.getHtml();
}

function submitFile() {
  var form = document.getElementById('program_submission');
  var file = document.getElementById('file_select_hidden').files[0];
  if (file == null) {
    return;
  }
  var data = new FormData();
  data.append("submission", file);
  var t = form.action;
  var updater = new Updater(t, data, false, printResponse);
  updater.update();
}

function initSubmission() {
  var fileSubmitter = new FormSubmitter('program_submission', submitFile, []);
  fileSubmitter.registerOnEnter('file_form_submit_button');
  fileSubmitter.register('click', 'file_form_button');
  registerEventHandler('click', 'file_choice_button', function() {
    var button = document.getElementById('file_select_hidden');
    button.click();
  });
  registerEventHandler('change', 'file_select_hidden', function(event) {
    var file = this.files[0];
    var desc = document.getElementById('file_choice_description');
    desc.innerHTML = file.name;
    event.preventDefault();
  });
}

window.addEventListener('load', initSubmission, false);