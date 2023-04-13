const String PAGE = R"DATA(
<HTML>
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-eOJMYsd53ii+scO/bJGFsiCZc+5NDVN2yr8+0RDqr0Ql0h+rP48ckxlpbzKgwra6" crossorigin="anonymous">
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta3/dist/js/bootstrap.bundle.min.js" integrity="sha384-JEW9xMcG8R+pH31jmWH6WWP0WintQrMb4s7ZOdauHnUtxwoG2vI5DkLtS3qm9Ekf" crossorigin="anonymous"></script>
  <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js'></script>"
  <meta name='viewport' content='width=device-width, initial-scale=1'>



  <HEAD>
      <TITLE>LED</TITLE>
    </HEAD>
  <BODY style='background-color:black;'>
      <a class='btn btn-primary btn-large' onclick="setSlider(255, 0, 0)">Rot</a>
      <a class='btn btn-primary btn-large' onclick="setSlider(0, 255, 0)">Gruen</a>
      <a class='btn btn-primary btn-large' onclick="setSlider(0, 0, 255)">Blau</a>
      <a class='btn btn-primary btn-large' onclick="setSlider(255,70,25)">Weiss</a>
      <a class='btn btn-primary btn-large' href='/rainbow'>Regenbogen</a>
      <a class='btn btn-primary btn-large' href='/fadered'>FadeRed</a>
      <a class='btn btn-primary btn-large' href='/fadepurple'>FadePurple</a>
      <a class='btn btn-primary btn-large' href='/rgbloop'>RGBLoop</a>
      <br/>
      <a class='btn btn-primary btn-large' onclick="setCode(0)">Farbe 1</a>
      <a class='btn btn-primary btn-large' onclick="setCode(1)">Farbe 2</a>
      <a class='btn btn-primary btn-large' onclick="setCode(2)">Farbe 3</a>
      <a class='btn btn-primary btn-large' onclick="setCode(3)">Farbe 4</a>
      <a class='btn btn-primary btn-large' onclick="setCode(4)">Farbe 5</a>
      <a class='btn btn-primary btn-large' onclick="setNextAnimation()">Naechste Animation</a>
      <div class="slidecontainer">
        <input type="range" min="0" max="255" value="50" class="slider" id="redSlider" onchange="sendData()">
      </div>
      <div class="slidecontainer">
        <input type="range" min="0" max="255" value="50" class="slider" id="greenSlider" onchange="sendData()">
      </div>
      <div class="slidecontainer">
        <input type="range" min="0" max="255" value="50" class="slider" id="blueSlider" onchange="sendData()">
      </div>
      <div class="slidecontainer">
        <label for="brightnessSlider" style="color: white;">Helligkeit</label>
        <input type="range" min="0" max="100" value="100" class="slider" id="brightnessSlider" onchange="sendData()">
      </div>
      <div class="custom-control custom-switch">
        <input type="checkbox" class="custom-control-input" id="fadeSwitch" onclick="sendData()">
        <label class="custom-control-label" for="fadeSwitch" style="color:white;">Fade</label>
      </div>
      <div>
        <label for="inputDelay" style="color:white;">Fade Delay in ms</label>
        <input type="number" id="inputDelay" min=0 onchange="sendData()" >
      </div>

      </BODY>
</HTML>   
  <script>
  
  
//
//  function setFadeTime(){
//    fadeTime = document.getElementById("inputDelay").value;
//    alert(fadeTime);
//  }

  function setCode(i){
      
      var brightness = document.getElementById("brightnessSlider").value;
      var red = document.getElementById("redSlider").value * brightness/100;
      var green = document.getElementById("greenSlider").value * brightness/100;
      var blue = document.getElementById("blueSlider").value * brightness/100;
      var eff = i;
      var daten = '{ "red":' + red + ' , "green":' + green + ', "blue":' + blue + ', "effect":' + eff + ' }';
      var xhttp = new XMLHttpRequest(); 
      xhttp.open("POST", "/setCode", true);
  
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
        // Response
        var response = this.responseText;
        }
      };
      xhttp.send(daten);
  }

  function setNextAnimation(){
    var xhttp = new XMLHttpRequest();
    xhttp.open("POST", "/setNextAnimation", true);
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
      // Response
      var response = this.responseText;
      }
    };
    xhttp.send();
  }
  
  function sendData() {
    var brightness = document.getElementById("brightnessSlider").value;
    var red = document.getElementById("redSlider").value * brightness/100;
    var green = document.getElementById("greenSlider").value * brightness/100;
    var blue = document.getElementById("blueSlider").value * brightness/100;
    var fadeTime = document.getElementById("inputDelay").value;
    var fade = document.getElementById("fadeSwitch").checked;
    var data = '{ "red":' + red + ' , "green":' + green + ', "blue":' + blue + ', "fadeTime:":' + fadeTime + ' }';
    var xhttp = new XMLHttpRequest();
    if(fade == true){
      xhttp.open("POST", "/fade", true);
    }
    else {
      xhttp.open("POST", "/color", true);
    }
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
      // Response
      var response = this.responseText;
      }
    };
    xhttp.send(data);
  }
   function FadePurple() {
    
    var xhttp = new XMLHttpRequest();
    
      xhttp.open("POST", "/fadepurple", true);
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
        // Response
        var response = this.responseText;
        }
      };
      xhttp.send();
      setTimeout("FadePurple()", 9000);
    
  }

  function setSlider(red, green, blue){
    document.getElementById("redSlider").value = red;
    document.getElementById("greenSlider").value = green;
    document.getElementById("blueSlider").value = blue;
    sendData();
  }

  </script>
<style>
  .slidecontainer {
    width: 75%; /* Width of the outside container */
    margin: auto;
    padding: 10px;
  }
  
  /* The slider itself */
  .slider {
    -webkit-appearance: none;  /* Override default CSS styles */
    appearance: none;
    width: 100%; /* Full-width */
    height: 25px; /* Specified height */
    outline: none; /* Remove outline */
    opacity: 0.7; /* Set transparency (for mouse-over effects on hover) */
    -webkit-transition: .2s; /* 0.2 seconds transition on hover */
    transition: opacity .2s;
  }
  
  /* Mouse-over effects */
  .slider:hover {
    opacity: 1; /* Fully shown on mouse-over */
  }
  
  /* The slider handle (use -webkit- (Chrome, Opera, Safari, Edge) and -moz- (Firefox) to override default look) */
  .slider::-webkit-slider-thumb {
    -webkit-appearance: none; /* Override default look */
    appearance: none;
    width: 25px; /* Set a specific slider handle width */
    height: 25px; /* Slider handle height */
    background: #000000;
    cursor: pointer; /* Cursor on hover */
  }
  
  .slider::-moz-range-thumb {
    width: 25px; /* Set a specific slider handle width */
    height: 25px; /* Slider handle height */
    background: #000000;
    cursor: pointer; /* Cursor on hover */
  }
  #redSlider {
    background: red;
  }
  #greenSlider {
    background: green;
  }
  #blueSlider {
    background: blue;
  }
  #brightnessSlider {
    background: white;
  }
</style>  
)DATA";
