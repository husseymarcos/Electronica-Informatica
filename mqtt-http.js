const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');
const config = require('./config');
const { verifyCard } = require('./server'); // Importar la función verifyCard
const WebSocket = require('ws');
const { MongoClient } = require('mongodb');

const { addBookToDB } = require('./server');

const { requestBook } = require('./server');

const {mongoUri} = require('./server');

const {mqttClient} = require('./server');

const {confirmVerification} = require('./server');


// Crear una aplicación Express
const app = express();
app.use(bodyParser.json());
app.use(express.static(path.join(__dirname, 'static')));

// Crear servidor WebSocket
const wss = new WebSocket.Server({ noServer: true });

// Mapa para almacenar las promesas pendientes de verificación
const pendingVerifications = new Map();


mqttClient.on('connect', () => { // Conexión a MQTT
  mqttClient.subscribe('library/usersVerification/#', (err) =>{
    if(err){
      console.error("Error al suscribirse a los tópicos de verificación:", err);
    } else {
      console.log("Suscrito a library/usersVerification/#");
    } 
  })
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

// TODO: Hacer algo similar para obtener todos los libros, pero asociado a cada tarjeta!
// TODO: Hacer la lógica para devolver los libros ASOCIADOS A CADA TARJETA. Es decir, los libros que solicito cada usuario.

/*Posible forma de implementarlo:
Es posible informar en la database de myBooks, el id del usuario que solicitó. Y comparar mediante eso, es decir:

Tener un parámetro denominado uuid, donde si es determinado uuid, devuelva determinados libros y lo mismo con el otro uuid.

TODO: Ver como podría implementarse. Notemos que no sé cada uuid, de cada tarjeta, debería hacerse de forma "automática".
*/ 

// TODO: Ver acá donde entra la lógica para implementar el returnBook, implementado en server.js

app.get('/api/mybooks', async (req, res) => {
  const client = new MongoClient(mongoUri);

  try {
    await client.connect();
    const database = client.db(config.mongodb.database);
    const myBooksCollection = database.collection(config.mongodb.myBooksCollection);
    const books = await myBooksCollection.find({}).toArray(); // TODO: Lo malo acá, es que va a devolver los libros que fueron solicitados por todos los usuarios. 

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
  const uuid = req.body.uuid; 

  console.log("UUID: ", uuid);
  console.log();
  
  const isAuthorized = await verifyCard(uuid);
  const responseTopic = `library/usersVerification/${uuid}`; // library/usersVerification/uuid
  
  
  const verificationPromise = new Promise((resolve, reject) =>{
    pendingVerifications.set(responseTopic, {resolve, reject});
  })


  mqttClient.publish(responseTopic, isAuthorized ? "authorized": "unathorized");

  
  try{ 
    const status = await verificationPromise;

    console.log("Status: ", status);

    if(isAuthorized && status === 'authorized'){ // verifyCard, es el que chequea en usersVerification.
      res.json({status: 'success', message: 'authorized'});
      
      confirmVerification(`Tarjeta con UUID ${uuid} ingresó correctamente a LibrosExpress`).catch(console.dir);

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

app.post('/api/books/request', async (req, res) => { // TODO: 
  const bookId = req.body.id; 
  const titleOfBook = req.body.title; // TODO: ver por qué acá se pasa como undefined. 
  console.log("BookId desde /api/books/request: ",bookId);
  console.log("titleOfBook desde /api/books/request: ", titleOfBook);

  try{
    const status = await requestBook(bookId);
    console.log("Status desde /api/books/request: ", status);

    if(status){
      res.status(200).send("Libro solicitado exitosamente.");
      const responseTopic = `library/bookRequests/${bookId}`; // Aca realizo un publish a library/bookRequests/#

      // Esto es lo que va a informarse por Serial Monitor del Arduino
      mqttClient.publish(responseTopic, `El libro con el ID: ${bookId}, es decir ${titleOfBook} fue solicitado correctamente`);

    } else{
      res.status(409).send("Libro solicitado anteriormente.");
    }
  } catch (error){
    console.error("Error al solicitar el libro: ", error);
    res.status(500).send("Error al solicitar el libro");
  }

  /*requestBook(bookId).then(() => {
    if(requestBook(bookId)){ // El status dio true, por lo que salió todo bien
      res.status(200).send("Libro solicitado exitosamente.");
    } else{ // El status dio false, porque el libro ya fue pedido. No podes pedirlo 2 veces 
      res.status(500).send("Libro solicitado anteriormente.");
    }
  }).catch((error) => {
    console.error("Error al solicitar el libro: ", error);
    res.status(500).send("Error al solicitar el libro.");
  });

  const responseTopic = `library/bookRequests/${bookId}`;

  // Esto es lo que va a informarse por Serial Monitor del Arduino
  mqttClient.publish(responseTopic, `El libro con el ID: ${bookId}, es decir ${titleOfBook} fue solicitado correctamente`); 
  */
  
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


