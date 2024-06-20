const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');
const config = require('./config');
const { verifyCard } = require('./server'); // Importar la función verifyCard
const WebSocket = require('ws');
const { MongoClient } = require('mongodb');
const { addBookToDB } = require('./server');

const {mongoUri} = require('./server');

const {mqttClient} = require('./server');



// Crear una aplicación Express
const app = express();
app.use(bodyParser.json());
app.use(express.static(path.join(__dirname, 'static')));

// Crear servidor WebSocket
const wss = new WebSocket.Server({ noServer: true });

// Mapa para almacenar las promesas pendientes de verificación
const pendingVerifications = new Map();

// Mapa para almacenar las promesas de solicitud de libros
// const pendingRequests = new Map();

/*mqttClient.on('connect', () => {
  mqttClient.subscribe('library/usersVerification/#', (err) =>{
    if(err){
      console.error("Error al suscribirse a los tópicos de verificación:", err);
    } else {
      console.log("Suscrito a library/usersVerification/#");
    } 
  })
});*/


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

// Fijate de donde podría salir el uuid. 
// Ruta para verificación del RFID
app.post('/api/rfid/verification', async (req, res) => { // FIXME: Anda mal! :(. Fijate si falla ahora, de hacerlo con post.  
  // TODO: Que chequee en la base de datos (debe chequear en usersVerification). Manda al topic confirmVerification. Volver a realizar esta lógica. 
  const uuid = req.body.uuid; // last chance --> req.body

  console.log("UUID: ", uuid);
  console.log();
  // console.log(req.query);
  // console.log();

  const isAuthorized = await verifyCard(uuid);
  const responseTopic = `library/usersVerification/${uuid}`; // TODO: Evaluá el sentido de responseTopic, se lo usa más abajo con tema de websocket, por eso consideré dejarlo acá.
  
  // mqttClient.publish(responseTopic, isAuthorized ? "authorized": "unathorized");

  const verificationPromise = new Promise((resolve, reject) =>{
    pendingVerifications.set(responseTopic, {resolve, reject});
  })

  console.log("Verification Promise: ", verificationPromise);



  try{ 
    const status = await verificationPromise;

    console.log("Status: ", status);

    if(isAuthorized && status === 'authorized'){ // verifyCard, es el que chequea en usersVerification.
      res.json({status: 'success', message: 'authorized'});
      mqttClient.publish('library/confirmVerification', `Tarjeta con UUID ${uuid} ingresó correctamente a LibrosExpress`);
    } else {
      res.status(403).json({status: 'error', message: 'unathorized'});
    }
  } catch (error){
    res.status(500).send("Error al verificar la tarjeta.");
  } finally {
    pendingVerifications.delete(responseTopic);
  }
});





// Ruta para SOLICITUD de LIBROS

/*app.post('/api/books/request/:id', async (req, res) => {
  const bookId = req.params.id;
  const responseTopic = `library/bookRequests/${bookId}`;

  if (pendingRequests.has(responseTopic)) {
    return res.status(429).send("Verification already in progress.");
  }

  const requestPromise = new Promise((resolve, reject) => {
    pendingRequests.set(responseTopic, { resolve, reject });
  });

  mqttClient.publish('library/bookRequests', bookId, (err) => {
    console.log("Estoy ejecutando la publicación en el topic library/bookRequests");
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
});*/



// Manejar los mensajes recibidos en los tópicos de respuesta - Verificación de RFID
mqttClient.on('message', (topic, message) => {
  console.log(`Mensaje recibido en el tópico ${topic}: ${message}`);
  if (pendingVerifications.has(topic)) {
    const { resolve } = pendingVerifications.get(topic);
    resolve(message.toString());
    console.log("RESOLVE: ", resolve);
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



