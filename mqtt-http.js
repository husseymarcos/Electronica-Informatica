const express = require('express');
const bodyParser = require('body-parser');
const mqtt = require("mqtt");
const path = require('path');
const config = require('./config');

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

// Ruta para eliminar un libro
app.delete('/api/books/delete/:id', (req, res) => {
  const book = req.body;
  
  // Publicar la solicitud de eliminación en el tópico MQTT
  mqttClient.publish('library/books/delete', JSON.stringify(book), (err) => {
    if (err) {
      console.error("Error al publicar en MQTT:", err);
      res.status(500).send("Error al eliminar el libro.");
    } else {
      res.status(200).send("Solicitud de eliminación enviada exitosamente.");
    }
  });
});

// Iniciar el servidor en el puerto 5500
const PORT = 5500;
app.listen(PORT, () => {
  console.log(`Servidor HTTP escuchando en el puerto ${PORT}`);
});



