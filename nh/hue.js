var fs = require('fs');
var mqtt    = require('mqtt');
var client  = mqtt.connect('mqtt://localhost');
var mqttnamespace='afero/hue/';
var mqttnamespaceset='afero/hue/set/';

// read our config data from disk - right now this includes the hue
// username field that we got when the hue was paired

var gapi;

var hueState= {
    username: ""
};
try
{
  var contents = fs.readFileSync("./huedata.json");
}
catch (err)
{
  console.log("reading json configuration: "+err);
}
// Define to JSON type
if (contents)
{
  hueState= JSON.parse(contents);
}

 
client.on('connect', function () {
  client.subscribe('afero/hue/#');
});
 

var hue = require("node-hue-api");
var lightState = hue.lightState;

require('es6-promise').polyfill();

var hostname="";

var displayBridges = function(bridge) {
    console.log("Hue Bridges Found: " + JSON.stringify(bridge));
};
var displayResult = function(result) {
    console.log(JSON.stringify(result, null, 2));
};
var displayError = function(err) {
    console.error(err);
};
// --------------------------
// Using a promise
hue.nupnpSearch(function(err, result) {
    if (err) throw err;
    displayBridges(result);
    hostname = result[0]["ipaddress"];
    


console.log("Hello World");

var username = "huelibrary",
    api;


console.log("hue.HueApi("+hostname+","+username+")");

api = new hue.HueApi(hostname, hueState.username);
gapi = api;

function toggleLightState(id,message)
{
 var state = lightState.create();
// Set the lamp with id '2' to on
 if (message=="true")
 {
console.log("got message true");
    api.setLightState(id, state.on())
    .then(displayResult)
    .fail(displayError)
    .done();
 }
 else
 {
   api.setLightState(id, state.off())
    .then(displayResult)
    .fail(displayError)
   
 }
}

client.on('message', function (topic, message) {
  // message is Buffer 
  console.log("MQTT: "+topic+" "+message.toString());
  // parse apart the  mosquitto message and turn the lights on/off etc

  // make sure message starts with  afero/hue/set

  if (topic.substring(0,mqttnamespaceset.length)===mqttnamespaceset)
  {
    var command = topic.substring(mqttnamespaceset.length);
    console.log("MQTT:     "+command+" "+message.toString());
    switch (command)
    {
        case "pair":

// do we need to loop and wait 30 seconds for them to press button?
api.createUser(hostname, function(err, user) {
    if (err) throw err;
    console.log("Created user: " + JSON.stringify(user));
    hueState.username=user;
    fs.writeFile( "huedata.json", JSON.stringify( hueState ), "utf8" );
    api = new hue.HueApi(hostname, hueState.username);
});


        break;

        case "Light1":
           toggleLightState(1,message);
        break;
        case "Light2":
           toggleLightState(2,message);
        break;
        case "Light3":
           toggleLightState(3,message);
        break;
    }
  }

  

});

// --------------------------
// Using a callback





});
var lightCallback = function(err, lights) {
    if (err) throw err;
    displayResult(lights);

    // read the lights, and send mosquitto messages
console.log('about to iterate lights:'+lights['lights']);
   for (var lightno in lights['lights'] )
   {
      console.log('in loop:'+lights['lights'][lightno]);
      var light =  lights['lights'][lightno];
      var id = light['id'];
      var name = light['name'];
      var state= light['state']['on'];
      var bri= light['state']['bri'];

      console.log('light '+id+' ['+name+'] is '+state+' at '+bri);
      var mname = mqttnamespace + 'Light'+ id;
      var state_str;
      if (state) state_str='True'; else state_str='False';
      client.publish(mname,state_str);
      mname = mqttnamespace + 'Light'+ id +'Label';
      client.publish(mname,name);

   }



}

function queryHueLightState()
{
  if (gapi)
  {
    gapi.lights(lightCallback);
  }
}
setInterval(queryHueLightState,5000);
