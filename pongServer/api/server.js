#!/usr/bin/env node

const express = require('express');
const path = require('path');
const axios = require("axios");
var cors = require('cors');
var http = require('http');
const app = express(),
      bodyParser = require("body-parser");
      port = 3080;

var ip = "192.168.0.220";

var player1Press = 0 // 0 = none, 1 = up, 2 = down
var player2Press = 0
var player1Previous = 0
var player2Previous = 0

app.use(bodyParser.json());
app.use(cors());

app.use(express.static(path.join(__dirname, '../frontend/build')));

app.get('/', (req,res) => {
  res.sendFile(path.join(__dirname, '../frontend/build/index.html'));
});

app.get('/getPlayerMoves', function (req, res) {
    res.send(player1Press + ":" + player1Previous + ":" + player2Press + ":" + player2Previous);
});

app.post('/sendPress', (req, res) => {
    if(req.body.player == 1) {
        if(req.body.direction > -1 && req.body.direction < 3) {
            player1Previous = player1Press;
            player1Press = req.body.direction;
        }
    } else if(req.body.player == 2) {
        if(req.body.direction > -1 && req.body.direction < 3) {
            player2Previous = player2Press;
            player2Press = req.body.direction;
        }
    }

    console.log("p1: " + player1Press + ", p2: " + player2Press);

    // Send data to FPGA whenever update occurs
    // if(player1Previous != player1Press || player2Previous != player2Press) {
    //     var strOut = "/?DATA=" + player1Press + ":" + player1Previous + ":" + player2Press + ":" + player2Previous;
    //     var options = {
    //         host: ip,
    //         path: strOut
    //     }
    //     var request = http.request(options, function (res) {
    //         var data = '';
    //         res.on('data', function (chunk) {
    //             data += chunk;
    //         });
    //         res.on('end', function () {
    //             console.log(data);
    
    //         });
    //     });
    //     request.on('error', function (e) {
    //         console.log(e.message);
    //     });
    //     request.end();
    // }

    res.json({response: "received"})
});

app.listen(port, () => {
  console.log(`Server listening on the port::${port}`);
});