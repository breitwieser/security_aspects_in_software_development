function showDisclaimer() {
  var src = document.getElementById('disclaimer_src');
  var overlayDiv = document.getElementById('MessageOverlay');
  var transparent = document.getElementById('lbody');
  var overlay = new Overlay('SASE Submission System Disclaimer', 'The SASE Submission System (SASE SUB SYS)is the basis for the web security assignment of the Security Aspects in Software Development (Sicherheitsaspekte in der Softwareentwicklung (SASE)) practical exercises in the winter semester 2013/2014 (Course number: 705.025, Graz University of Technology).', src.innerHTML, overlayDiv, transparent);
  overlay.show();
}

function showLicense() {
  var src = document.getElementById('license_src');
  var overlayDiv = document.getElementById('MessageOverlay');
  var transparent = document.getElementById('lbody');
  var overlay = new Overlay('SASE Submission System License', 'The SASE Submission System is licensed under the MIT License shown below:', src.innerHTML, overlayDiv, transparent);
  overlay.show();
}

function initFooter() {
  var license = document.getElementById('license');
  var disclaimer = document.getElementById('disclaimer');
  license.addEventListener('click', showLicense, false);
  disclaimer.addEventListener('click', showDisclaimer, false);
}

window.addEventListener('load', initFooter, false);