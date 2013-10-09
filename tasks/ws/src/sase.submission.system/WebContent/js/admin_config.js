function setCfgValue(id, value) {
  var data = new SimpleFormData();
  data.append("id", id);
  data.append("value", value);
  var t = createServerUrlFromPath(getContextPath() + '/admin/config_edit.do');
  var updater = new Updater(t, data, true);
  updater.update();
}

function adminConfigEditDone(event) {
  var td = event.target;
  var row = td.parentElement;
  var id = row.cells[0].innerHTML;
  var value = row.cells[2].innerHTML;
  setCfgValue(id, value);
  event.stopPropagation();
}

function adminConfigEditBegin(event) {
}

var adminConfigNavBar = new NavBar('admin_config_list', '/config.json',
    generateAdminConfigList);

function generateConfigNavBar(data) {
  var total = data.approxEntries;
  var pageMax = data.PageMax;
  return adminConfigNavBar.generateNavBar(total, pageMax);
}

function generateAdminConfigList(data) {
  var adminConfigListContainer = document.getElementById("admin_config_list");
  var gen = new TableGenerator('admin_config_list_table',
      'table_head table_head_yellow',
      'table_odd table_td_yellow table_odd_yellow',
      'table_even table_td_yellow table_even_yellow', 'admin_config_list_row');
  gen.begin('table_yellow');
  gen.beginHeaderRow();
  gen.envelopHeadWithClass('id', 'hidden');
  gen.envelopHead('Property');
  gen.envelopHead('Value');
  gen.endHeaderRow();
  gen.beginBodyWithClass("table_body");
  for ( var i = 0; i < data.ConfigData.length; i++) {
    var prop = data.ConfigData[i];
    gen.beginRow();
    gen.envelopWithClass(prop.id, 'hidden');
    gen.envelop(prop.name);
    gen.envelopEditable(prop.value);
    gen.endRow();
  }
  gen.endBody();
  gen.end();
  adminConfigListContainer.innerHTML = gen.getHtml();
  adminConfigListContainer.appendChild(generateConfigNavBar(data));
  gen.registerTableCellHandlers(adminConfigEditBegin, adminConfigEditDone,
      gen.blurOnEnter);
}

function loadAdminConfigList() {
  if (document.getElementById("admin_config_list") != null) {
    var t = createServerUrlFromPath(getContextPath() + '/config.json');
    var fetcher = new JsonRetriever(t, 1, generateAdminConfigList);
    fetcher.fetchData();
  }
}

window.addEventListener('load', loadAdminConfigList, false);