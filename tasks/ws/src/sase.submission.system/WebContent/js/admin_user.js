function getParentTable(start) {
  var parent = start.parentElement;
  while (parent != null) {
    if (parent.tagName.toLowerCase() == "table") {
      return parent;
    } else {
      parent = parent.parentElement;
    }
  }
}

function deleteHandler(event) {
  var img = event.target;
  var td = img.parentElement;
  var row = td.parentElement;
  var table = getParentTable(row);
  var id = row.cells[0].innerHTML;
  var data = new SimpleFormData();
  data.append("id", id);
  var t = createServerUrlFromPath(getContextPath() + '/admin/user_del.do');
  var updater = new Updater(t, data, true);
  updater.update();
  table.deleteRow(row.rowIndex);
  event.stopPropagation();
}

var adminUserNavBar = new NavBar('admin_user_list', '/users.json',
    generateAdminUserList);

function generateUserListNavBar(data) {
  var total = data.NumberOfUsers;
  var pageMax = data.PageMax;
  return adminUserNavBar.generateNavBar(total, pageMax);
}

function generateAdminUserList(data) {
  var adminUserListContainer = document.getElementById("admin_user_list");
  var gen = new TableGenerator('admin_user_list_table',
      'table_head table_head_yellow',
      'table_odd table_td_yellow table_odd_yellow',
      'table_even table_td_yellow table_even_yellow', 'admin_user_list_row');
  gen.begin('table_yellow');
  gen.beginHeaderRow();
  gen.envelopHeadWithClass('id', 'hidden');
  gen.envelopHead('Name');
  gen.envelopHead('Email');
  gen.envelopHead('#Iterations');
  gen.envelopHead('Salt');
  gen.envelopHead('Action');
  gen.endHeaderRow();
  gen.beginBodyWithClass("table_body");
  for ( var i = 0; i < data.ListOfUsers.length; ++i) {
    var user = data.ListOfUsers[i];
    gen.beginRow();
    gen.envelopWithClass(user.id, 'hidden');
    gen.envelop(user.name);
    gen.envelop(user.email);
    gen.envelop(user.iter);
    gen.envelop(user.salt);
    gen.envelop('<img src="/sase.submission.system/icons/trash_full.png"' + 
        'class="cursor_pointer" width="16" height="16" contenteditable="true" />');
    gen.endRow();
  }
  gen.endBody();
  gen.end();
  adminUserListContainer.innerHTML = gen.getHtml();
  adminUserListContainer.appendChild(generateUserListNavBar(data));
  var table = document.getElementById('admin_user_list_table');
  var images = table.getElementsByTagName('img');
  for ( var i = 0; i < images.length; i++) {
    images[i].addEventListener('click', deleteHandler, false);
  }
}

function loadAdminUserListTable(event) {
  if (document.getElementById("admin_user_list") != null) {
    var fetcher = new JsonRetriever(createServerUrlFromPath(getContextPath()
        + '/users.json'), 1, generateAdminUserList);
    fetcher.fetchData();
  }
}

window.addEventListener('load', loadAdminUserListTable, false);