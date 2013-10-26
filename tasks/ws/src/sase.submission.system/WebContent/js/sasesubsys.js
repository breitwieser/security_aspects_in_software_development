/**
 * Parses the value of a request parameter for the current page from the
 * location property of the window object.
 * 
 * @param name
 *          the name of the request parameter to get
 * @returns the value of the request parameter if it exists; null otherwise
 */
function getRequestParameter(name) {
  var encName = encodeURIComponent(name);
  var matcher = new RegExp('[?&]' + encName + '=([^&]*)');
  var matches = matcher.exec(window.location);
  if (matches) {
    return decodeURIComponent(matches[1]);
  } else {
    return null;
  }
}

/**
 * Determines the application context path, which in this case is the first path
 * element of the locations pathname.
 * 
 * @returns the application context path
 */
function getContextPath() {
  return window.location.pathname.match(/^\/([^\/]*)/)[1];
}

/**
 * Build the fully qualified URL to a specific resource on the server.
 * 
 * @param path
 *          the path of the resource on the server. The values for the protocol,
 *          host name and port are taken from window.location.
 * @returns {String} the fully qualified URL pointing to this resource
 */
function createServerUrlFromPath(path) {
  return createServerUrl(window.location.protocol, window.location.hostname,
      window.location.port, path);
}

/**
 * Build the fully qualified URL to a specific resource on the server using the
 * given protocol, hostname, port and resource path.
 * 
 * @param protocol
 *          the protocol of the URL, such as http
 * @param hostname
 *          the hostname or IP address of the server
 * @param port
 *          the port of the service on the server
 * @param path
 *          the path to the resource
 * @returns {String} the fully qualified URL pointing to this resource
 */
function createServerUrl(protocol, hostname, port, path) {
  return protocol.match(/^([^:]*):$/)[1] + '://' + hostname + ':' + port + '/'
      + path;
}

/**
 * Compute a non-invertible representation of the user's password. For this we
 * use a simple MD5 hash, applied <code>iterations</code> times on the
 * <code>salt</code> concatenated with the <code>pwd</code>. We let the
 * server specify the <code>salt</code> and <code>iterations</code>, so
 * that we can upgrade our security requirements over time. By using a secured
 * password, we are not liable if our password database is stolen, because we do
 * not store the user passwords in plain.
 * 
 * @param pwd
 *          the password to secure
 * @param salt
 *          the salt to use
 * @param iterations
 *          the number of times the MD5 hash function is applied
 * @returns the secured password.
 */
function digestPassword(pwd, salt, iterations) {
  var res = pwd + salt;

  //FIX 1f ensure that password is hashed at least once
  iterations = iterations <= 0 ? 1 : iterations;
  
  for ( var i = 0; i < iterations; i++) {
    res = CryptoJS.MD5(res);
  }
  return res;
}

function Condiments(uri, username) {
  this.uri = uri;
  this.username = username;
  this.salt = null;
  this.iter = null;
}

Condiments.prototype.fetchCondiments = function() {
  var waiter = new XMLHttpRequest();
  if (waiter) {
    // waiter.onreadystatechange = this.responseHandler.bind(this, waiter);
    waiter.open('GET', this.uri + '/condiments.json?name=' + this.username,
        false);
    waiter.setRequestHeader('X-Requested-With', 'XMLHttpRequest');
    waiter.send();
    var condimentTray = eval('(' + waiter.responseText + ')');
    this.salt = condimentTray.salt;
    this.iter = condimentTray.iter;
  }
};

Condiments.prototype.toString = function() {
  return "Salt: " + this.salt + "\n#Iterations: " + this.iter;
};

/**
 * Creates a new JsonRetriever. This class fetches JSON information from the
 * server using AJAX. The index specifies the row offset of the data to
 * retrieve.
 * 
 * @param uri
 *          the uri of the JSON provider
 * @param index
 *          the row offset of the data
 * @param dataCallback
 *          a data handler, which is called when the data has been successfully
 *          fetched from the server
 * @param preCallback
 *          (optional) parameter defining a second callback, that is called
 *          before the dataCallback
 * 
 * @returns {JsonRetriever}
 */
function JsonRetriever(uri, index, dataCallback, preCallback) {
  this.uri = uri;
  this.index = index;
  this.dataCallback = dataCallback;
  this.preCallback = preCallback;
}

JsonRetriever.prototype.defaultHandler = function(fetchRequest) {
  if (fetchRequest.readyState === 4) {
    var data = eval('(' + fetchRequest.responseText + ')');
    if (this.preCallback) {
      this.preCallback();
    }
    this.dataCallback(data);
  }
};

JsonRetriever.prototype.fetchData = function() {
  var fetchRequest = new XMLHttpRequest();
  if (fetchRequest) {
    fetchRequest.onreadystatechange = this.defaultHandler.bind(this,
        fetchRequest);
    fetchRequest.open('GET', this.uri + '?index=' + this.index, true);
    fetchRequest.setRequestHeader('X-Requested-With', 'XMLHttpRequest');
    fetchRequest.send();
  }
};

/**
 * Creates a new <code>SimpleFormData</code> object.
 * <code>SimpleFormData</code> is a convenient method to create the
 * &name=parameter format used by web forms. It works for simple string or
 * string convertible data. For byte arrays or files see JavaScripts own
 * <code>FormData</code>.
 * 
 * @returns a new <code>SimpleFormData</code> object
 */
function SimpleFormData() {
  this.names = new Array();
  this.values = new Array();
}

/**
 * Appends a name/value pair to the list of form parameters.
 * 
 * @param name
 *          the name of the parameter
 * @param value
 *          the value of the parameter
 */
SimpleFormData.prototype.append = function(name, value) {
  this.names.push(name);
  this.values.push(value);
};

/**
 * Converts all parameter name/value pairs into a form data string of the format
 * name1=value1&name2=value2...
 */
SimpleFormData.prototype.toString = function() {
  var data = "";
  for ( var i = 0; i < this.names.length; i++) {
    data += this.names[i] + '=' + this.values[i] + '&';
  }
  if (data.length > 0) {
    data = data.slice(0, -1);
  }
  return data;
};

function Updater(target, data, simple, responseHandler) {
  this.target = target;
  this.data = data;
  this.simple = simple;
  this.responseHandler = responseHandler;
}

Updater.prototype.defaultHandler = function(sendData) {
  if (sendData.readyState === 4) {
    if (sendData.status != 200) {
      document.open();
      document.write(sendData.responseText);
      document.close();
      return;
    }
    if (this.responseHandler == null) {
      this.defaultErrorHandler(sendData);
    } else {
      this.responseHandler(sendData);
    }
  }
};

Updater.prototype.update = function() {
  var sendData = new XMLHttpRequest();
  if (sendData) {
    sendData.onreadystatechange = this.defaultHandler.bind(this, sendData);
    sendData.open('POST', this.target, true);
    sendData.setRequestHeader('X-Requested-With', 'XMLHttpRequest');
    if (this.simple) {
      sendData.setRequestHeader("Content-type",
          'application/x-www-form-urlencoded');
      sendData.send(this.data.toString());
    } else {
      sendData.send(this.data);
    }
  }
};

Updater.prototype.defaultErrorHandler = function(sendData) {
  var response = sendData.responseText;
  if (!response || response.trim().length < 1) {
    return;
  }
  var comData = document.getElementById('ComAreaDataStore');
  comData.innerHTML = response;
  commHandler();
};

/**
 * Creates a new <code>TableGenerator</code> object. The
 * <code>TableGenerator</code> is helpful when building HTML tables using
 * JavaScript. It provides a number of generator methods which add HTML tags to
 * the internal representation of the table. Be careful, there are no sanity
 * checks done on the order of the method calls. Thus calling the methods out of
 * their logical order might lead to corrupt HTML. To convert the internal table
 * representation to HTML call the <code>toHtml</code> method.
 * 
 * @param tableId
 *          the element id of the table tag
 * @param hclass
 *          the class applied to the table headers (th)
 * @param tclass_odd
 *          the class applied to the odd row table datas (td)
 * @param tclass_even
 *          the class applied to the even row table datas (td)
 * @param rowId
 *          an id string that is used to give each row in the table a unique
 *          identifier
 * @returns a new <code>TableGenerator</code> object
 */
function TableGenerator(tableId, hclass, tclass_odd, tclass_even, rowId) {
  this.tableId = tableId;
  this.hclass = hclass;
  this.tclass_odd = tclass_odd;
  this.tclass_even = tclass_even;
  this.tclass = tclass_even;
  this.html = "";
  this.rowCount = 0;
  this.rowId = rowId;
}

/**
 * Begins building a table by adding a table tag to the internal representation.
 * 
 * @param clazz
 *          the class for the table
 */
TableGenerator.prototype.begin = function(clazz) {
  this.html += '<table id="' + this.tableId + '" class="' + clazz + '">';
};

/**
 * Starts a header row by adding the table head and table row tags.
 */
TableGenerator.prototype.beginHeaderRow = function() {
  this.html += '<thead><tr>';
};

/**
 * Envelops the given data string in table header (th) tags.
 * 
 * @param data
 *          the table header data
 */
TableGenerator.prototype.envelopHead = function(data) {
  this.html += '<th class="' + this.hclass + '">' + data + "</th>";
};

/**
 * Envelops the given data string in table header (th) tags using the given
 * class.
 * 
 * @param data
 *          the table header data
 * @param clazz
 *          the class to use to display the table data
 */
TableGenerator.prototype.envelopHeadWithClass = function(data, clazz) {
  this.html += '<th class="' + clazz + '">' + data + "</th>";
};

/**
 * Closes the table header adding a closing row tag and a closing table head tag
 * (/thead).
 */
TableGenerator.prototype.endHeaderRow = function() {
  this.html += '</tr></thead>';
};

/**
 * Starts the body of the table by adding a table body tag (body).
 */
TableGenerator.prototype.beginBody = function() {
  this.html += '<tbody>';
};

/**
 * Starts the body of the table by adding a table body tag (body) with the given
 * class.
 * 
 * @param clazz
 *          the class of the table body tag
 */
TableGenerator.prototype.beginBodyWithClass = function(clazz) {
  this.html += '<tbody class="' + clazz + '">';
};

/**
 * Starts a table row by adding a table row tag (tr). Each table row gets its
 * own unique id which is created by appending '_{nbr}' to the row id. Here
 * {nbr} is the current number of the row starting with zero for the first row.
 */
TableGenerator.prototype.beginRow = function() {
  this.tclass = ((this.rowCount % 2) == 0) ? this.tclass_even : this.tclass_odd;
  this.html += '<tr id="' + this.rowId + '_' + this.rowCount + '">';
};
/**
 * Envelops the given data string in table data tags (td)
 * 
 * @param data
 *          the data to envelop
 */
TableGenerator.prototype.envelop = function(data) {
  this.html += '<td class="' + this.tclass + '">' + data + "</td>";
};

/**
 * Envelops the given editable data string in table data tags (td)
 * 
 * @param data
 *          the data to envelop
 */
TableGenerator.prototype.envelopEditable = function(data) {
  this.html += '<td class="' + this.tclass + '" contenteditable="true">' + data
      + "</td>";
};

/**
 * Envelops the given data string in table data tags (td) using the given class.
 * 
 * @param data
 *          the data to envelop
 * @param clazz
 *          the class to use
 */
TableGenerator.prototype.envelopWithClass = function(data, clazz) {
  this.html += '<td class="' + clazz + '">' + data + "</td>";
};

/**
 * Envelops the given data string in table data tags (td) using the given in
 * addition to the standard class.
 * 
 * @param data
 *          the data to envelop
 * @param clazz
 *          the class to use in addition to the standard class
 */
TableGenerator.prototype.envelopWithAdditionalClass = function(data, clazz) {
  this.html += '<td class="' + clazz + ' ' + this.tclass + '">' + data
      + "</td>";
};

/**
 * Closes the current row by adding a closing row tag (/tr).
 */
TableGenerator.prototype.endRow = function() {
  this.html += '</tr>';
  this.rowCount++;
};

/**
 * Closes the table body by adding a closing table body tag (/tbody).
 */
TableGenerator.prototype.endBody = function() {
  this.html += '</tbody>';
};

/**
 * Closes the table by adding a closing table tag (/table).
 */
TableGenerator.prototype.end = function() {
  this.html += '</table>';
};

/**
 * Converts the internal table representation to HTML.
 * 
 * @returns {String} the HTML string representation of the table
 */
TableGenerator.prototype.getHtml = function() {
  return this.html;
};

/**
 * A default table data key handler that blurs the focus on its target element,
 * when either enter or escape are pressed. This can be used in conjunction with
 * the <code>registerTableCellHandlers</code> method.
 * 
 * @param event
 *          the keypress event
 */
TableGenerator.prototype.blurOnEnter = function(event) {
  var key = event.keyCode || event.which;
  // stop editing on return and escape
  if (key == 0x0D || key == 0x1B) {
    event.target.blur();
  }
};

/**
 * Registers the given handlers for focus, blur and keypress events for every
 * content edible table cell in the table. This only works, once the table has
 * been generated and added to the document. Otherwise this function will do
 * nothing.
 * 
 * @param handler
 *          the on click handler
 */
TableGenerator.prototype.registerTableCellHandlers = function(focusHandler,
    blurHandler, keyHandler) {
  var table = document.getElementById(this.tableId);
  if (!table) {
    return;
  }
  for ( var i = 0, row; row = table.rows[i]; i++) {
    for ( var j = 0, cell; cell = row.cells[j]; j++) {
      if (cell.contentEditable) {
        cell.addEventListener('focus', focusHandler, false);
        cell.addEventListener('blur', blurHandler, false);
        cell.addEventListener('keypress', keyHandler, false);
      }
    }
  }
};

/**
 * This function checks if the server left a message in the communication area
 * of the page and makes it visible.
 */
function commHandler() {
  var commData = document.getElementById('ComAreaDataStore');
  var msg = commData.innerHTML;
  if (!msg || msg.trim().length < 1) {
    return;
  }
  msg = eval('(' + msg + ')');
  var comArea = document.getElementById('CommArea');
  var comMsgArea = document.getElementById('CommMessage');
  comMsgArea.innerHTML = msg.Message;
  comArea.style.marginTop = '0px';
}

function commClose() {
  var commData = document.getElementById('ComAreaDataStore');
  commData.innerHTML = "";
  var comArea = document.getElementById('CommArea');
  var comMsgArea = document.getElementById('CommMessage');
  comMsgArea.innerHtml = "";
  comArea.style.marginTop = '-34px';
}

function initCommCloseButton() {
  var msgCloseButton = document.getElementById("CommClose");
  msgCloseButton.addEventListener('click', commClose, false);
}

window.addEventListener('load', commHandler, false);
window.addEventListener('load', initCommCloseButton, false);

/**
 * Creates a new overlay object. An overlay is an element that is layed over the
 * rest of the page, while greying out the rest of the page.
 * 
 * @param titleText
 *          the title of the overlay
 * @param descriptionText
 *          the description that specifies what the content of the overlay
 *          signifies
 * @param bodyText
 *          the body of the overlay
 * @param outerContainer
 *          the element that contains the overlay. Must be a div
 * @param transparent
 *          the element that is made transparent, while the overlay is shown
 * @returns a new <code>Overlay</code> object
 */
function Overlay(titleText, descriptionText, bodyText, outerContainer,
    transparent) {
  this.outerContainer = outerContainer;
  var clazz = this.outerContainer.getAttribute('class');
  this.outerContainerClass = (clazz == null) ? "" : clazz;
  this.transparent = transparent;
  var clazz = this.transparent.getAttribute('class');
  this.transparentClass = (clazz == null) ? "" : clazz;
  this.head = document.createElement('div');
  this.head.setAttribute('id', 'overlay_head');
  this.head.setAttribute('class', 'overlay_head');
  this.button = document.createElement('div');
  this.button.setAttribute('id', 'overlay_close');
  this.button.setAttribute('class',
      'overlay_close round_button mod_button_grey');
  this.button.innerHTML = 'x';
  this.title = document.createElement('h1');
  this.title.setAttribute('class', 'heading');
  this.title.innerHTML = titleText;
  this.description = document.createElement('p');
  this.description.setAttribute('class',
      'description description_yellow overlay_description');
  this.description.innerHTML = descriptionText;
  this.head.appendChild(this.button);
  this.head.appendChild(this.title);
  this.head.appendChild(this.description);
  this.body = document.createElement('div');
  this.body.setAttribute('id', 'overlay_body');
  this.body.setAttribute('class', 'overlay_body');
  this.body.innerHTML = bodyText;
  this.container = document.createElement('div');
  this.container.setAttribute('id', 'overlay_container');
  this.container.setAttribute('class', 'message_overlay');
  this.container.appendChild(this.head);
  this.container.appendChild(this.body);
}

/**
 * Shows the overlay, by making adding the overlay itself and making the
 * specified document main element transparent.
 */
Overlay.prototype.show = function() {
  this.transparent.setAttribute('class', this.transparentClass + ' trans_ovl');
  this.outerContainer.setAttribute('class', '');
  this.outerContainer.appendChild(this.container);
  this.button.addEventListener('click', this.closeHandler.bind(this), false);
  window.addEventListener('resize', this.resizeHandler.bind(this), false);
  this.resizeHandler();
};

/**
 * Hides the overlay
 */
Overlay.prototype.hide = function() {
  this.transparent.setAttribute('class', this.transparentClass);
  this.outerContainer.setAttribute('class', this.outerContainerClass);
  this.outerContainer.removeChild(this.container);
  this.button.removeEventListener('click', this.closeHandler.bind(this), false);
  window.removeEventListener('resize', this.resizeHandler.bind(this), false);
};

/**
 * A default event handler that will hide the overlay.
 */
Overlay.prototype.closeHandler = function() {
  this.hide();
};

/**
 * An event handler that handles computing the correct size of the body of the
 * overlay.
 */
Overlay.prototype.resizeHandler = function() {
  var containerHeight = this.container.clientHeight;
  var headerHeight = this.head.offsetHeight;
  /*
   * 42, the answer to life, the universe and how much space is taken up by
   * margins, which offsetHeight does not account for ...
   */
  this.body.style.height = (containerHeight - headerHeight - 42) + 'px';
};

/**
 * Creates a new NavBar object with the given table container, data provider
 * path, and table generator function. A navigation bar helps browsing large
 * tables, by splitting the tables into smaller fragments and only displaying
 * only a single section of the table.
 * 
 * @param tableContainer
 *          the element where the table this navigation bar is for should be
 *          created
 * @param providerPath
 *          the path to the servlet that generates the data required by the
 *          table generator function to create the table fragment
 * @param tableGenerator
 *          the function that actually builds the table from the data provided
 *          by the data provider servlet
 * @returns a new NavBar object
 */
function NavBar(tableContainer, providerPath, tableGenerator) {
  this.index = 0;
  this.tableContainer = tableContainer;
  this.providerPath = providerPath;
  this.tableGenerator = tableGenerator;
}

/**
 * Updates the index of the navigation bar to a new value. Because the
 * navigation bar uses ajax to load a fragment of a table from the server, it
 * needs to store the current position somewhere. This is the index value.
 * 
 * @param index
 *          the current index of the table fragment to display
 */
NavBar.prototype.updateIndex = function(index) {
  this.index = index;
};

/**
 * Loads a new table fragment from the server, freshly generates the table using
 * the table generator function and the new data and then updates the index of
 * the navigation bar.
 * 
 * @param newIndex
 *          the new table fragment index
 */
NavBar.prototype.defaultHandler = function(newIndex) {
  if (document.getElementById(this.tableContainer) != null) {
    var t = createServerUrlFromPath(getContextPath() + this.providerPath);
    var fetcher = new JsonRetriever(t, newIndex + 1, this.tableGenerator,
        this.updateIndex.bind(this, newIndex));
    fetcher.fetchData();
  }
};

/**
 * Generates the HTML for the table navigation bar.
 * 
 * @param total
 *          the total number of entries in the table
 * @param pageMax
 *          the number of rows per table fragment
 * @returns a new div HTML element containing the navigation bar for the table
 */
NavBar.prototype.generateNavBar = function(total, pageMax) {
  var container = document.createElement('div');
  if (total < pageMax) {
    return container;
  }
  var page = Math.floor(this.index / pageMax);
  var lastPage = Math.floor((total - 1) / pageMax);
  var prevIndex = ((page - 1) < 0 ? 0 : page - 1) * pageMax;
  var lastIndex = (lastPage * pageMax);
  var nextIndex = (page + 1) > lastPage ? lastIndex : (page + 1) * pageMax;
  container.setAttribute('class', 'nav_bar_container');
  var inner = [ '&lt;&lt;   ', '&lt;', '&gt;', '   &gt;&gt;' ];
  var handlers = [ this.defaultHandler.bind(this, 0),
      this.defaultHandler.bind(this, prevIndex),
      this.defaultHandler.bind(this, nextIndex),
      this.defaultHandler.bind(this, lastIndex) ];
  var classes = [ 'nav_bar_left', 'nav_bar_center', 'nav_bar_center',
      'nav_bar_right' ];
  for ( var i = 0; i < 4; i++) {
    var navBar = document.createElement('div');
    navBar.setAttribute('class', 'nav_bar ' + classes[i]);
    var span = document.createElement('span');
    span.innerHTML = inner[i];
    span.addEventListener('click', handlers[i], false);
    span.setAttribute('class', 'nav_bar');
    navBar.appendChild(span);
    container.appendChild(navBar);
  }
  var spacer = document.createElement('div');
  spacer.setAttribute('class', 'kill_float');
  container.appendChild(spacer);
  return container;
};
