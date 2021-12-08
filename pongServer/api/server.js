#!/usr/bin/env node

const express = require('express');
const app = express(),
      bodyParser = require("body-parser");
      port = 3080;
var cors = require('cors');

var player1Press = 0 // 0 = none, 1 = up, 2 = down
var player2Press = 0


app.use(bodyParser.json());
app.use(cors());

// app.use(express.static(path.join(__dirname, '../frontend-controller/build')));

// app.get('/', (req,res) => {
//   res.sendFile(path.join(__dirname, '../frontend-controller/build/index.html'));
// });

app.get('/getPlayerMoves', function (req, res) {
    res.send(player1Press + ":" + player2Press);
});

app.post('/sendPress', (req, res) => {
    if(req.body.player == 1) {
        if(req.body.direction > -1 && req.body.direction < 3)
            player1Press = req.body.direction;
    } else if(req.body.player == 2) {
        if(req.body.direction > -1 && req.body.direction < 3)
            player2Press = req.body.direction;
    }

    console.log("p1: " + player1Press + ", p2: " + player2Press);
  
    res.json({response: "received"})
});

app.listen(port, () => {
  console.log(`Server listening on the port::${port}`);
});