#!/usr/bin/python

from subprocess import call
from pubnub import Pubnub

pubnub = Pubnub(publish_key="pub-c-00a7863c-157e-41cd-9949-cf7c4f7b0485", subscribe_key="sub-c-dd38910c-4037-11e7-b6a4-02ee2ddab7fe", ssl_on=False)
channel = "Internet Remote"

def callback(message, channel):
    command = message['command']

    if command == "query":
        call(["node", "SmartAssistant.js"])

def error(message):
    print("ERROR : " + str(message))


def connect(message):
    print("CONNECTED")


def reconnect(message):
    print("RECONNECTED")


def disconnect(message):
    print("DISCONNECTED")


pubnub.subscribe(channels=channel, callback=callback, error=error,
                 connect=connect, reconnect=reconnect, disconnect=disconnect)

#import os

#cmd = """
#osascript -e 'tell application "System Events"
#	--keystroke "ls"
#	--keystroke " -al" & return
#	key code 126 using {command down}
#	end tell' 
#"""

#os.system(cmd)

#osascript -e 'tell application "System Events" to keystroke "m" using {command down}' 