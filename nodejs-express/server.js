'use strict';

const express = require('express');

// Constants
const PORT = 9000;
const HOST = '0.0.0.0';

// Web function invocation
const app = express();
app.get('/*', (req, res) => {
  res.send('Hello Serverless Cloud Function , Web Function\n');
});

// Event function invocation
app.post('/event-invoke', (req, res) => {
  res.send('Hello Serverless Cloud Function , Event Function\n');
});

var server = app.listen(PORT, HOST);
console.log(`SCF Running on http://${HOST}:${PORT}`);

server.timeout = 0; // never timeout
server.keepAliveTimeout = 0; // keepalive, never timeout

