cc2538_coap_sensor
==================

WSN temperature sensor based on cc2538 + Contiki + CoAP

## Getting, building, and installing via Git ##
  
    $ git clone https://github.com/retfie/cc2538_coap_sensor.git
    $ cd cc2538_coap_sensor
    $ git submodule init
    $ git submodule update
    $ make

This should generate coap_sensor bin, elf and hex target files:

Update contiki submodules. Wee need cc2538-bsl script to upload image via serial port
to bootloader into CC2538:

    $ cd contiki
    $ git submodule update --init

Back to sensor project dir:

    $ cd ../
    
Put device in bootloader mode via reset and predefined external pin:

    $ make coap-post.upload

If flashing was successfull, sensor will print its IPv6 address on console
and will start posting messages with some metrics to predefined IP address bbbb::1:

Example message in JSON format:

    $ { "eui": "00124b0003d0a6ac", "vdd": "2671 mV", "temp": "35238 mC", "count": "5242", "tmp102": "25312 mC" }

You can change some of parameters with CoAP capable client:

* Copper (Cu) plugin for Firefox
* SMCP C stack from command line ( https://github.com/darconeous/smcp.git )

Get current value of some parameter:

    $ smcpctl get coap://[IPv6 addr of sensor]/config?param=interval

Change parameter on device:

    $ smcpctl post coap://[IPv6 addr of sensor]/config?param=interval 60

Available parameters currently are:

 * interval - time between metric post
 * path	 - path to which to post on server
 * ip	 - IP of the CoAP server to post


