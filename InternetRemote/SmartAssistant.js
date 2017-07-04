// make sure you have Node.js Installed!
// Get the IP address of your photon, and put it here:

// CLI command to get your photon's IP address
//
// particle get MY_DEVICE_NAME ipAddress

// Put your IP here!
var settings = {
	ip: "",
	port: 1234,
    deviceId: "SWEARABLE"
};

/**
 * Created by middleca on 7/18/15.
 */

//based on a sample from here
//	http://stackoverflow.com/questions/19548755/nodejs-write-binary-data-into-writablestream-with-buffer

var fs = require("fs");

var samplesLength = 1000;
var sampleRate = 16000;
var bitsPerSample = 8;

var numChannels = 1;

var outStream;

var writeHeader = function() {
    outStream = fs.createWriteStream("test.wav");

	var b = new Buffer(1024);
	b.write('RIFF', 0);
	/* file length */
	b.writeUInt32LE(32 + samplesLength * numChannels, 4);
	//b.writeUint32LE(0, 4);

	b.write('WAVE', 8);

	/* format chunk identifier */
	b.write('fmt ', 12);

	/* format chunk length */
	b.writeUInt32LE(16, 16);

	/* sample format (raw) */
	b.writeUInt16LE(1, 20);

	/* channel count */
	b.writeUInt16LE(1, 22);

	/* sample rate */
	b.writeUInt32LE(sampleRate, 24);

	/* byte rate (sample rate * block align) */
	b.writeUInt32LE(sampleRate * 1, 28);
	//b.writeUInt32LE(sampleRate * 2, 28);

	/* block align (channel count * bytes per sample) */
	b.writeUInt16LE(numChannels * 1, 32);
	//b.writeUInt16LE(2, 32);

	/* bits per sample */
	b.writeUInt16LE(bitsPerSample, 34);

	/* data chunk identifier */
	b.write('data', 36);

	/* data chunk length */
	//b.writeUInt32LE(40, samplesLength * 2);
	b.writeUInt32LE(0, 40);


	outStream.write(b.slice(0, 50));
};

var Particle = require('particle-api-js');
var particle = new Particle();
var token;
var client;

var getAudio = function() {
        console.log("send start listening event...");
        var startEvent = particle.publishEvent({ name: 'Listen', data: "start", auth: token });

        startEvent.then(
            function(data) {
                if (data.body.ok) {
                    console.log("Event published successfully")

                    var net = require('net');
                    console.log("connecting...");
                    client = net.connect(settings.port, settings.ip, function () {
                        client.setNoDelay(true);

                        client.on("data", function (data) {
                            try {
                                //console.log("GOT DATA");
                                outStream.write(data);
                                //outStream.flush();
                                console.log("got chunk of " + data.length + " bytes ");
                                console.log("got chunk of " + data.toString('HEX'));
                            }
                            catch (ex) {
                                console.error("Er!" + ex);
                            }
                        });
                    });
                }
            },
            function(err) {
                console.log("Failed to publish event: " + err)
            });
}

var processRequest = function(durationInMilliSeconds) {
    writeHeader();

    getAudio();

    setTimeout(function() {
        console.log('recorded for ' + durationInMilliSeconds / 1000 + ' seconds');
        client.end();
        outStream.end();

        console.log("send stop listening event...");
        particle.publishEvent({ name: 'Listen', data: "stop", auth: token }).then(
            function() {
                //Perform PiRi function
                
                //process.exit(0);
            });
    }, durationInMilliSeconds);
}

console.log("logging in...");

particle.login({username: 'alexander.yao@outlook.com', password: 'Open1234'}).then(
    function(data) {
        token = data.body.access_token;
        console.log(token);

        particle.getVariable({deviceId: settings.deviceId, name: 'ipAddress', auth: token}).then(
            function(data) {
                console.log('Device variable retrieved successfully:', data);
                settings.ip = data.body.result;

                particle.getEventStream({ deviceId: settings.deviceId, name: 'Listen', auth: token }).then(
                    function(stream) {
                        stream.on('event', function(data) {
                            console.log("Event: " + data.data + "\r\nTime: " + data.published_at);

                            if (data.data == "initiate") {
                                processRequest(5000);
                            }
                        });
                    });
                },
            function(err) {
                console.log('An error occurred while getting attrs:', err);
            });
    },
    function (err) {
        console.log('Could not log in.', err);
    });