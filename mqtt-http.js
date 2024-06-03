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


// Ruta para realizar la verificación del Login


// Chequealo, trata de hacerlo parecido a lo que tenías antes.
app.post('/api/rfid/verification', async (req, res) => { // TODO
  const { uuid } = req.body;

  if(!uuid){
    return res.status(400).json({status: 'failure', message: 'UUID is required'});
  }
  try {
    const isAuthorized = await verifyCard(uuid);
    if (isAuthorized) {
      res.json({ status: 'success', message: 'Card is authorized' });
    } else {
      res.json({ status: 'failure', message: 'Card is not authorized' });
    }
  } catch (error) {
    res.status(500).json({ status: 'error', message: error.message });
  }
});


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



