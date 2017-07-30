var settings = {
    ip: "",
    port: 1234,
    deviceId: "SWEARABLE",
    username: "alexander.yao@outlook.com",
    password: "Open1234",
    event: "Listen"
};

var fs = require("fs");
var samplesLength = 1000;
var sampleRate = 16000;
var bitsPerSample = 8;
var numChannels = 1;

//var ffmpeg = "ffmpeg-win";
var ffmpeg = "./ffmpeg";
var inputFile = "resources/input.wav";
var outputFile = "resources/output.wav";

var outStream;

const { exec } = require('child_process');

var writeHeader = function () {
    outStream = fs.createWriteStream(inputFile);

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
    b.writeUInt16LE(numChannels, 22);

    /* sample rate */
    b.writeUInt32LE(sampleRate, 24);

    /* byte rate (sample rate * block align) */
    //b.writeUInt32LE(sampleRate * 1, 28);
    b.writeUInt32LE(sampleRate * numChannels * bitsPerSample / 8, 28);

    /* block align (channel count * bytes per sample) */
    //b.writeUInt16LE(numChannels * 1, 32);
    b.writeUInt16LE(numChannels * bitsPerSample / 8, 32);

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

var getAudio = function () {
    console.log("send start listening event...");
    var startEvent = particle.publishEvent({ name: settings.event, data: "start", auth: token });

    startEvent.then(
        function (data) {
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
        function (err) {
            console.log("Failed to publish event: " + err)
        });
}

function syncRecognize(filename, encoding, sampleRateHertz, languageCode) {
    const Speech = require('@google-cloud/speech');
    const speech = Speech();

    const request = {
        encoding: encoding,
        sampleRateHertz: sampleRateHertz,
        languageCode: languageCode
    };

    return speech.recognize(filename, request);
}

var processRequest = function (durationInMilliSeconds) {
    writeHeader();

    getAudio();

    setTimeout(function () {
        console.log('recorded for ' + durationInMilliSeconds / 1000 + ' seconds');
        client.end();
        outStream.end();

        console.log("send stop listening event...");
        particle.publishEvent({ name: settings.event, data: "stop", auth: token }).then(
            function () {
                fs.unlink(outputFile);
                // Convert to 16-bit mono
                var convertCommand = ffmpeg + ' -i ' + inputFile + ' -acodec pcm_s16le -ac 1 ' + outputFile;
                console.log('converting to 16 bit output: ' + convertCommand);
                exec(convertCommand, (error, stdout, stderr) => {
                    if (error) {
                        console.error(`exec error: ${error}`);
                        return;
                    }
                    console.log(`stdout: ${stdout}`);
                    console.log(`stderr: ${stderr}`);

                    // Call google speech to text api
                    console.log('calling google speech to text api');
                    syncRecognize(outputFile, 'LINEAR16', 16000, 'en-US')
                        .then((results) => {
                            const transcription = results[0];
                    
                            console.log(`Transcription: ${transcription}`);

                            // Publish result
                            particle.publishEvent({ name: "Notification", data: transcription, auth: token });
                        })
                        .catch((err) => {
                            console.error('ERROR:', err);
                        });
                });

                //process.exit(0);
            });
    }, durationInMilliSeconds);
}


console.log("logging in...");

particle.login({ username: settings.username, password: settings.password }).then(
    function (data) {
        token = data.body.access_token;
        console.log(token);

        particle.getVariable({ deviceId: settings.deviceId, name: 'ipAddress', auth: token }).then(
            function (data) {
                console.log('Device variable retrieved successfully:', data);
                settings.ip = data.body.result;

                particle.getEventStream({ deviceId: settings.deviceId, name: settings.event, auth: token }).then(
                    function (stream) {
                        stream.on('event', function (data) {
                            console.log("Event: " + data.data + "\r\nTime: " + data.published_at);

                            if (data.data == "initiate") {
                                processRequest(5000);
                            }
                        });
                    });
            },
            function (err) {
                console.log('An error occurred while getting attrs:', err);
            });
    },
    function (err) {
        console.log('Could not log in.', err);
    });