var client = new Messaging.Client("test.mosca.io", 80, "arduino_" + parseInt(Math.random() * 100, 10));
var onoff = 3;

//"arduino_" + parseInt(Math.random() * 100, 10)
//Gets  called if the websocket/mqtt connection gets disconnected for any reason
client.onConnectionLost = function (responseObject) {
    //Depending on your scenario you could implement a reconnect logic here
	window.location.replace("http://renatobylyk.com/projects/AC_project/index.php");
	
};
//Gets called whenever you receive a message for your subscriptions
function connectRoom(site_publish, callback) {
    client.onMessageArrived = function (message) {
        if(!isNaN(message.payloadString)){
			if (message.destinationName == site_publish + "/response/dht_hum") {
				document.getElementById("dht_hum").innerHTML = message.payloadString + " %";
			}
			if (message.destinationName == site_publish + "/response/dht_temp") {
				document.getElementById("dht_temp").innerHTML = message.payloadString + "°C";
			}
        }
        if (message.destinationName == site_publish + "/response/watt") {
            if (parseInt(message.payloadString) > 1000) {
                document.getElementById("watt").innerHTML = "680.0";
                if (parseInt(message.payloadString) > 110) {
                    onoff = 1;
                    openclose = 1;
                }
            } else {
                document.getElementById("watt").innerHTML = message.payloadString;
            }
        }
        if (message.destinationName == site_publish + '/response') {
            if (message.payloadString == "connected") {
                isConnected = 1;
                callback(message.destinationName);
            } else {
                callback(message.payloadString);
            }
        }
    };
}

//Connect Options
var options = {
    timeout: 3,

    onSuccess: function () {
        door = true;
    }
    ,
    //Gets Called if the connection could not be established
    onFailure: function (message) {
        window.location.replace("http://renatobylyk.com/projects/AC_project/index.php");
    }
};
//Creates a new Messaging.Message Object and sends it to the HiveMQ MQTT Broker
var publish = function (payload, topic, qos) {
    //Send your message (also possible to serialize it as JSON or protobuf or just use a string, no limitations)
    var message = new Messaging.Message(payload);
    message.isRetained = false;
    message.destinationName = topic;
    message.qos = qos;
    client.send(message);
}

