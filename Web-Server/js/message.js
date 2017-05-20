var client = new Messaging.Client("MQTT BROKER", 80, "NAME_" + parseInt(Math.random() * 100, 10));

//"arduino_" + parseInt(Math.random() * 100, 10)
//Gets  called if the websocket/mqtt connection gets disconnected for any reason
client.onConnectionLost = function (responseObject) {
    //Depending on your scenario you could implement a reconnect logic here

};
//Gets called whenever you receive a message for your subscriptions
function connectRoom(site_publish, callback) {
    client.onMessageArrived = function (message) {
       // Your CALL BACK MESSAGE NEW
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
