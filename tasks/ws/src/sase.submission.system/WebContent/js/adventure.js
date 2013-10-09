var feed_counter = 0;
var fuzzy_width = 200;
var fuzzy_height = 155;

function growFuzzy(factor) {
  var fuzzy = document.getElementById('fuzzys_adventure');
  fuzzy_width = fuzzy_width * factor;
  fuzzy_height = fuzzy_height * factor;
  fuzzy.style.width = fuzzy_width + 'px';
  fuzzy.style.height = fuzzy_height + 'px';
}

function resetFuzzy() {
  var fuzzy = document.getElementById('fuzzys_adventure');
  fuzzy_width = 200;
  fuzzy_height = 155;
  fuzzy.style.width = fuzzy_width + 'px';
  fuzzy.style.height = fuzzy_height + 'px';
  feed_counter = 0;
}

function setResponse(response) {
  var target = document.getElementById('adventure_text_field');
  if (target) {
    target.innerHTML = response;
  } else {
    alert(response);
  }
}

function setResponseToOverlay(response) {
  var target = document.getElementById('MessageOverlay');
  var lbody = document.getElementById('lbody');
  var overlay = new Overlay('Exception Stack Trace', 'Here is a detailed stack trace of the exception leading you to the error page. Click to hide!', response, target, lbody);
  overlay.show();
}

function decodeSt() {
  var commData = document.getElementById('ComAreaDataStore');
  var msg = commData.innerHTML;
  if (!msg || msg.trim().length < 1) {
    return 'No further detail ...';
  }
  msg = eval('(' + msg + ')');
  var text = "";
  for ( var i = 0; i < msg.Throwables.length; i++) {
    var t = msg.Throwables[i];
    text += "<p>";
    text += atob(t).replace(/\n/g, '<br>');
    text += "</p>";
  }
  return text;
}

function parseText(text) {
  var exitMatcher = /(exit|get.*out|go.*out|leave)/i;
  var fleeMatcher = /(flee|run|scamper|depart)/i;
  var goMatcher = /(move|walk|jog|sprint|travel|wander|drift|go)/i;
  var fightMatcher = /(attack|fight|kill|destroy|annihilate|eridicate|decimate|eliminate|massacre|slaughter|erease|brain|neutrali[sz]e|murder|execute)/i;
  var feedMatcher = /(feed|cater|give|provide|supply)/i;
  var traceMatcher = /trace/i;
  var toMatcher = /(to|towards)/i;
  var fromMatcher = /from/i;
  var fuzzyMatcher = /(fuzzy|bug|insect)/i;
  var egressMatcher = /(door|gateway|egress|doorway)/;
  var helpMatcher = /help/i;

  var action;
  var actionName;
  if (actionName = text.match(helpMatcher)) {
    action = 'help';
  } else if (actionName = text.match(fleeMatcher)) {
    action = 'flee';
  } else if (actionName = text.match(exitMatcher)) {
    action = 'exit';
  } else if (actionName = text.match(goMatcher)) {
    action = 'go';
  } else if (actionName = text.match(traceMatcher)) {
    action = 'trace';
  } else if (actionName = text.match(fightMatcher)) {
    action = 'fight';
  } else if (actionName = text.match(feedMatcher)) {
    action = 'feed';
  } else {
    /* No action */
    action = null;
  }

  var direction;
  if (text.match(toMatcher)) {
    direction = 'to';
  } else if (text.match(fromMatcher)) {
    direction = 'from';
  } else if (action == 'flee') {
    direction = 'from';
  } else if (action == 'go') {
    direction = 'to';
  } else {
    direction = null;
  }

  var object;
  // var objectName;
  if (text.match(fuzzyMatcher)) {
    object = 'fuzzy';
  } else if (text.match(egressMatcher)) {
    object = 'door';
  } else {
    object = null;
  }

  if (!action) {
    setResponse("I cannot distill any wisdom from your command, oh hapless one!");
    return;
  }

  if (!object) {
    switch (action) {
    case 'flee':
      setResponse("Where should I " + actionName[0] + " to?");
      return;
    case 'go':
      setResponse("Where should I " + actionName[0] + " to?");
      return;
    case 'trace':
    case 'fight':
    case 'feed':
      setResponse("Who do you want to " + actionName[0] + "?");
      return;
    }
  }

  var fleeFromFuzzy = 'You valiantly and quite smartly try to evade <b class="fuzzy_the_bug">Fuzzy the Bug\'s</b> murderously gluttonous advances, without doubt to tell the world about the danger of this monstrous abomination. Sadly, there is no abyss filled with molten lava, separating you from the king of all killer lemon bugs. <b class="fuzzy_the_bug"> Fuzzy the Bug</b> reaches you with a speed that belies his humongous body and unceremoniously eats you.';
  var goToFuzzy = 'In an ingenious attempt to confuse <b class="fuzzy_the_bug">Fuzzy the Bug</b> you move towards him. Sadly <b class="fuzzy_the_bug">Fuzzy</b> is neither confused, nor impressed. In one fell swoop he catches you in his terrifying maw and turns you into second breakfast.';
  var helpText = 'Your encounter with <b class="fuzzy_the_bug">Fuzzy the Bug</b> is styled as a text adventure. This means you can tell me what you want to do by issuing simple verb subject commands like <code>attack gateway</code>, or <code>run from fuzzy</code>. I will try to interpret your command to the best of my abilities. Sadly my programmers did not equip me with a large vocabulary. So please forgive me and try to rephrase your command, if I don\'t do what you want. If this does not work for you, you can always rewrite me! See <code>function parseText(text)</code> in <code>adventure.js</code> for details.';
  switch (action) {
  case 'help':
    setResponse(helpText);
    resetFuzzy();
    return;
  case 'exit':
    setResponse(fleeFromFuzzy);
    resetFuzzy();
    return;
  case 'flee':
    if (object == 'fuzzy') {
      if (direction == 'to') {
        setResponse(goToFuzzy);
      } else {
        setResponse(fleeFromFuzzy);
      }
    } else if (object == 'door') {
      if (direction == 'to') {
        setResponse(fleeFromFuzzy);
      } else {
        setResponse(goToFuzzy);
      }
    } else {
      setResponse(fleeFromFuzzy);
    }
    resetFuzzy();
    return;
  case 'go':
    if (object == 'fuzzy') {
      if (direction == 'from') {
        setResponse(fleeFromFuzzy);
      } else {
        setResponse(goToFuzzy);
      }
    } else if (object == 'door') {
      if (direction == 'from') {
        setResponse(goToFuzzy);
      } else {
        setResponse(fleeFromFuzzy);
      }
    } else {
      setResponse(goToFuzzy);
    }
    resetFuzzy();
    return;
  case "fight":
    if (object == 'fuzzy') {
      setResponse('Without any weapons, nor any protection you valiantly storm towards <b class="fuzzy_the_bug">Fuzzy the Bug</b> in a brave attempt to rip off his feelers and feed them to him. Sadly, <b class="fuzzy_the_bug">Fuzzy</b> is not particularly happy about your, in the end, completely futile endeavor. In the blink of an eye <b class="fuzzy_the_bug">Fuzzy</b> lifts his left front paw and simply squishes you like, well a bug.');
    } else {
      setResponse('You valiantly attack the gateway to the Internet. <b class="fuzzy_the_bug">Fuzzy the Bug</b> looks at you for some time and then simply devours you as a snack.');
    }
    resetFuzzy();
    return;
  case "trace":
    if (object == 'fuzzy') {
      setResponseToOverlay(decodeSt());
    }
    resetFuzzy();
    return;
  case "feed":
    if (object == 'fuzzy') {
      feed_counter++;
      switch (feed_counter) {
      case 1:
        setResponse('We told you not to feed him!');
        growFuzzy(2);
        break;
      case 2:
        setResponse('The elders of the Internet appear and beseech you to stop this madness!');
        growFuzzy(2);
        break;
      default:
        growFuzzy(2);
        feed_counter = 0;
        fuzzy_width = 200;
        fuzzy_height = 155;
        setResponse('Now you have done it! <b class="fuzzy_the_bug">Fuzzy</b> swollen with the food you so reckless fed him drags his massive form toward the Gateway to the Internet. With abject horror you realize the desctruction <b class="fuzzy_the_bug">Fuzzy the Bug</b> will bring to the Internet and thus to the world. There is nothing you can do to stop him. <br /> <br />Congratulations, you have survived your encounter with <b class="fuzzy_the_bug">Fuzzy the Bug</b>. But at what price?');
        break;
      }
    }
    return;
  }
}

function adventureParser(event) {
  var key = event.keyCode || event.which;
  if (key == 0x0D) {
    var input = event.target;
    var text = input.value;
    parseText(text);
    input.value = '';
  }
  return false;
}

function block() {
  return false;
}

function initAdventure() {
  var parser = document.getElementById('adventure_parser');
  if (parser) {
    parser.addEventListener('keypress', adventureParser, false);
  }
  var form = document.getElementById('adventure_form');
  if (form) {
    form.addEventListener('submit', block, false);
  }
}

window.addEventListener('load', initAdventure, false);