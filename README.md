An Arduino program targeting the SONOFF 4CH relay box.

This software is designed to run a domestic boiler. At the moment, it is in the early stages of development and only provides basic functionality. This said, the code does fetch the time with NTP and has a regular triggered event, there is just no scheduling functionality to take advantage of this yet.

The software was written in a rush as our boiler controller wasn't working last winter and we needed heat.

It allows you to control the boiler and heating circuit pump, with two more relays available for use, over wifi.

I previously used crontab from my NAS to schedule on and off times for the boiler.

