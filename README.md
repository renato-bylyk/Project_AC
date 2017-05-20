# Project_AC // controlling the AC's wirelessly while keeping track if they are on or off and how much power they consume.

#1 mqtt - esp8266:

With the mqtt protocol i subscribe in the fields i wait for the messages to arrive - if message arrives we sent it to arduino then wait until the arduino is ready to respond.

#2 arduino - dht11 - sct013-000 :

If a message arrives , i check what the message was and then respond with the answer + send the command it was asked to the AC and judging by the power consumption we can estimate if the command was received or not.
