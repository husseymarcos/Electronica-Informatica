const express = require('express');
const bodyParser = require('body-parser');
const mqtt = require("mqtt");
const path = require('path');
const config = require('./config');
const { verifyCard } = require('./server'); // Importar la función verifyCard
const WebSocket = require('ws');
const { MongoClient } = require('mongodb');

// Configuración de mongoDB
var mongoUri = 'mongodb://' + config.mongodb.hostname + ':' + config.mongodb.port + '/' + config.mongodb.database;

// Configuración de MQTT
var mqttUri  = 'mqtt://' + config.mqtt.hostname + ':' + config.mqtt.port; // Broker de MQTT
const mqttClient = mqtt.connect(mqttUri);

// Crear una aplicación Express
const app = express();
app.use(bodyParser.json());
app.use(express.static(path.join(__dirname, 'static')));

// Crear servidor WebSocket
const wss = new WebSocket.Server({ noServer: true });

// Mapa para almacenar las promesas pendientes de verificación
const pendingVerifications = new Map();

// Mapa para almacenar las promesas de solicitud de libros
const pendingRequests = new Map();

// Suscribirse al tópico de respuesta cuando se conecta al servidor MQTT
mqttClient.on('connect', () => {
  mqttClient.subscribe('library/usersVerification/#', (err) => {
    if (!err) {
      console.log('Suscrito a library/usersVerification/#');
    } else {
      console.error('Error al suscribirse al tópico:', err);
    }
  });

  mqttClient.subscribe('library/bookRequests/#', (err) => {
    if (!err) {
      console.log('Suscrito a library/bookRequests/#');
    } else {
      console.error('Error al suscribirse al tópico:', err);
    }
  });
});


// Ruta para obtener todos los libros
app.get('/api/books', async (req, res) => {
  const client = new MongoClient(mongoUri);

  try {
    await client.connect();
    const database = client.db(config.mongodb.database);
    const collection = database.collection(config.mongodb.bookCollection);
    const books = await collection.find({}).toArray();

    res.json(books);
  } catch (error) {
    console.error('Error al obtener la lista de libros:', error);
    res.status(500).send('Error al obtener la lista de libros.');
  } finally {
    await client.close();
  }
});



// Ruta para agregar un libro
app.post('/api/books/publish', (req, res) => {
  const book = req.body;
  
 // Llama a la función para agregar libros desde server.js
 addBookToDB(book).then(() => {
  res.status(200).send("Libro agregado exitosamente.");
}).catch((error) => {
  console.error("Error al agregar el libro:", error);
  res.status(500).send("Error al agregar el libro.");
});

});

// Ruta para verificación del RFID
app.post('/api/rfid/verification', async (req, res) => {
  console.log("llegué a hacer algo en /api/rfid/verification");
  const { uuid } = req.body;
  const responseTopic = `library/usersVerification/${uuid}`;

  // Verificar si ya hay una promesa pendiente para esta UUID
  if (pendingVerifications.has(responseTopic)) {
    return res.status(429).send("Verification already in progress.");
  }

  const verificationPromise = new Promise((resolve, reject) => {
    pendingVerifications.set(responseTopic, { resolve, reject });
  });

  mqttClient.publish('library/usersVerification', uuid, (err) => {
    console.log("Estoy ejecutando la publicación en el topic library/usersVerification");
    if (err) {
      console.error("Error al publicar en MQTT:", err);
      res.status(500).send("Error al verificar la tarjeta.");
      pendingVerifications.delete(responseTopic);
    }
  });


  try {
    const status = await verificationPromise;
    if (status === 'authorized') {
      // Send response indicating success
      res.json({ status: 'success', message: 'authorized' });
      console.log("publique la res");
 
      // Publicar en el topic de confirmación - Comunicación con ESP32 
      mqttClient.publish('library/confirmVerification', 'Ingreso a LibrosExpress realizado');
    } else {
      res.status(403).json({ status: 'error', message: 'unauthorized' });
    }
  } catch (error) {
    res.status(500).send("Error al verificar la tarjeta.");
  } finally {
    pendingVerifications.delete(responseTopic);
  }
});



// Ruta para SOLICITUD de LIBROS

app.post('/api/books/request/:id', async (req, res) => {
  const bookId = req.params.id;
  const responseTopic = `library/bookRequests/${bookId}`;

  if (pendingRequests.has(responseTopic)) {
    return res.status(429).send("Verification already in progress.");
  }

  const requestPromise = new Promise((resolve, reject) => {
    pendingRequests.set(responseTopic, { resolve, reject });
  });

  mqttClient.publish('library/bookRequests', bookId, (err) => {
    if (err) {
      console.error("Error al publicar en MQTT:", err);
      res.status(500).send("Error al solicitar el libro.");
      pendingRequests.delete(responseTopic);
    }
  });

  try {
    const status = await requestPromise;
    if (status === 'available') {
      res.json({ status: 'success', message: 'Libro disponible' });
    } else {
      res.status(403).json({ status: 'error', message: 'Libro no disponible' });
    }
  } catch (error) {
    res.status(500).send("Error al solicitar el libro.");
  } finally {
    pendingRequests.delete(responseTopic);
  }
});



// Manejar los mensajes recibidos en los tópicos de respuesta - Verificación de RFID
mqttClient.on('message', (topic, message) => {
  console.log(`Mensaje recibido en el tópico ${topic}: ${message}`);
  if (pendingVerifications.has(topic)) {
    const { resolve } = pendingVerifications.get(topic);
    resolve(message.toString());
    pendingVerifications.delete(topic);
    console.log(`Promesa resuelta para el tópico ${topic}`);
  } else {
    console.log(`No hay promesas pendientes para el tópico ${topic}`);
  }

  // Enviar el mensaje recibido a todos los clientes WebSocket
  wss.clients.forEach(client => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(JSON.stringify({ topic, message: message.toString() }));
    }
  });
});

// Manejar las conexiones WebSocket
wss.on('connection', ws => {
  console.log('Nuevo cliente WebSocket conectado');
  ws.on('message', message => {
    console.log(`Mensaje recibido del cliente WebSocket: ${message}`);
  });
});

// Iniciar el servidor en el puerto 80 y manejar las conexiones de WebSocket
const server = app.listen(80, () => {
  console.log(`Servidor HTTP escuchando en el puerto 80`);
});

server.on('upgrade', (request, socket, head) => {
  wss.handleUpgrade(request, socket, head, ws => {
    wss.emit('connection', ws, request);
  });
});



