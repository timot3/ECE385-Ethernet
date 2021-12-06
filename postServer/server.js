#!/usr/bin/env node

const express = require('express');
const path = require('path');
const app = express(),
      bodyParser = require("body-parser");
      port = 3080;
var net = require('net');

app.use(bodyParser.json());

app.post('/sendCommand', (req, res) => {
  console.log("COMMAND: " + req.body.test);
});

app.listen(port, () => {
  console.log(`Server listening on the port::${port}`);
});
