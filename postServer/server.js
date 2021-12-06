#!/usr/bin/env node

const express = require('express');
const path = require('path');
const app = express(),
      bodyParser = require("body-parser");
      port = 3080;
var net = require('net');

var lastSentTime = new Date();

app.use(bodyParser.json());


app.post('/sendCommand', (req, res) => {
  console.log("COMMAND: " + req.body.test);

  // Only send commands every 250ms
  let currTime = new Date();
  if(currTime - lastSentTime > 250) {
    lastSentTime = currTime;
  }

  res.json({connection: "Sent"})
});

app.listen(port, () => {
  console.log(`Server listening on the port::${port}`);
});
