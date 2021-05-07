MQTT is a light weight messaging system between machines. I commonly use it to communicate between my IoT on my local network such as camera pi, server pi, speaker pi, ESP8266.

There are 2 components to MQTT pubsub system. It's much simplier than I thought when I tried debugging why my setup wasn't working, I knew it couldn't be this hard for a lightweight 
system. If it's not straight forward and simple, your intuition is correct, something is wrong outside of the MQTT setup.

Component:
1. Broker
2. Client

The broker is installed as a service through `sudo apt-get install mosquitto` which serves as the server to post bulletin board where clients can subscribe to or publish to.
The mosquitto service (broker) should automatically start after installing, otherwise you can check the status with `sudo systemctl status mosquitto.service` 
interchanging with start, stop, enable, disable.

The clients are installed through `sudo apt-get install mosquitto-clients` 1. publish messages to topics and 2. subscribes to topics and listens for messages.

Example of subscribing command:
`mosquitto_sub -d -h myhostnamewithbroker.local -t mytopic`

Example of publishing command:
`mosquitto_pub -d -h myhostnamewithbroker.local -t mytopic -m "mymessage"`

The two commands is all you need.

If you want to build programs with MQTT, I recommend paho-mqtt python package. There are 3 main concepts on_connect(), on_message(), loop_forever(), 
http://www.steves-internet-guide.com/mqtt-python-callbacks/ this explains it pretty well. Weird concept to wrap my head around, 
but from python paho-mqtt perspective this is only building the client that subscribes and publishes, nothing to do with the broker in python code. You do not build the 
broker or anything. The mosquitto.service is the broker and is working in the background.

Story on why I was frustrated with installing MQTT and hope you learn from my mistake. Coming back to install another system, I knew it should be simple from previous experience. 
But for some reason I kept getting connection refused or timed out with no error code. I couldn't figure out with google. I didn't understand why my clients on the same local network 
could not publish or subscribe other than the broker's local machine, it would simply timeout without any useful messages. The broker's local machine could publish and subscribe with 
another terminal no problem at all. Immediately I figure it must be a firewall issue, but I had none that I remember installing on the broker raspberry pi, I even ran the executable 
'ufw' and 'ufw cannot be found'. I couldn't figure out whether my setup was wrong, config file was wrong, some kind of bad MQTT version, weird edge cases where I needed a 
username/pw (NO DO NOT NEED, it is as simple as the example commands). I debugged it by using a third machine to serve a second external client to the host and it too failed with 
the same behavior of hanging without error. It had to be the broker's fault as a firewall or something. It turns out 'ufw' firewall executable only reveals itself when 
using 'sudo ufw'. It is deceiving that a 'ufw status' command would yield 'ufw not found' and cost me a couple hours :(. 