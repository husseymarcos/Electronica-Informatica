const express = require('express');
const bodyParser = require('body-parser');
const mqtt = require("mqtt");
const path = require('path');
const config = require('./config');
const { verifyCard } = require('./server'); // Importar la función verifyCard

/*
Este código básicamente permite publicar al mqtt que está escuchando.
La idea es que de esta forma, el libro que agreguemos, desde la página web, como se maneja por http
se publique en el mqtt que está escuchando en server.js, como sabemos, el código que está allí, maneja
el agregado de datos al mongo db, por tanto, el msj que vamos a agregar ahora, alude a lo que mandemos
desde nuestro http, por tanto el formato json que agreguemos desde nuestra página, directamente debería agregarse
en nuestra db. Aqui está presente la relación entre suscribe y publish de mqtt. Básicamente, la clave está
en identificar quien suscribe, y quien publica. 
*/ 

// Configuración de MQTT
var mqttUri  = 'mqtt://' + config.mqtt.hostname + ':' + config.mqtt.port; // Broker de MQTT
const mqttClient = mqtt.connect(mqttUri);

// Crear una aplicación Express
const app = express();
app.use(bodyParser.json());
app.use(express.static(path.join(__dirname, 'static')));


// Mapa para almacenar las promesas pendientes de verificación
// const pendingVerifications = new Map();

// Ruta para agregar un libro
app.post('/api/books/publish', (req, res) => {
  const book = req.body;
  
  // Publicar el libro en el tópico MQTT
  mqttClient.publish('library/books', JSON.stringify(book), (err) => {
    if (err) {
      console.error("Error al publicar en MQTT:", err);
      res.status(500).send("Error al agregar el libro.");
    } else {
      res.status(200).send("Libro agregado exitosamente.");
    }
  });
});

// Ruta para verificación del RFID
app.post('/api/rfid/verification', async (req, res) => {
  const { uuid } = req.body;
  const responseTopic = `library/registerUsers/${uuid}`;

  /*const promise = new Promise((resolve, reject) =>{
    pendingVerifications.set(responseTopic, {resolve, reject});
  })*/

  mqttClient.publish('library/registerUsers', uuid, (err) => {
    if (err) {
      console.error("Error al publicar en MQTT:", err);
      res.status(500).send("Error al verificar la tarjeta.");
    } else {
      mqttClient.once('message', (topic, message) => {
        if (topic === responseTopic) {
          const status = message.toString();
          res.json({ status: 'success', message: status });
        }
      });
    }
  });
});

// Manejar los mensajes recibidos en los tópicos de respuesta

/*mqttClient.on('message', (topic, message) =>{
  if (pendingVerifications.has(topic)) {
    const { resolve } = pendingVerifications.get(topic);
    resolve(message.toString());
    pendingVerifications.delete(topic);
  }
});*/

// Hacer que el http escuche a una variable que cambia. Eso es cuando se agrega una tarjeta.
// Si cambia el estado de esa variable, el http permite llevar al login.html

// Tengo esp32, publicando en el topic. library/usersVerification
// El mqtt tiene que escuchar en ese topic para ver si se apoyó una tarjeta.
// Una vez que ese estado cambia, lleva a cambiar el estado del http, y me lleva al login.html. 

function checkAuth(req, res, next) {
  if (req.session.isAuthenticated) {
    next();
  } else {
    res.redirect('/login.html');
  }
}


app.get('/index.html', checkAuth, (req, res) => {
  res.sendFile(__dirname + '/static/index.html');
});


// Iniciar el servidor en el puerto 80
const PORT = 80;
app.listen(PORT, () => {
  console.log(`Servidor HTTP escuchando en el puerto ${PORT}`);
});



