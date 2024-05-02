const express = require('express');
const bodyParser = require('body-parser');
const { connectToMongoDB } = require('./mongodb');
const connectToMQTT = require('./mqttLogic');
const bookRoutes = require('./routes/bookRoutes');

const app = express();

// Middleware para analizar solicitudes JSON
app.use(bodyParser.json());

// Conexión a MongoDB en la nube - TODO Debo ver esto bien!
const mongoURI = 'mongodb+srv://<username>:<password>@<cluster>.mongodb.net/<database>'; // Esto hay que ver si tiene sentido!
connectToMongoDB(mongoURI);

// Conexión a MQTT
const mqttURL = 'mqtt://52.23.254.208'; // O la URL de tu servidor MQTT en la nube. Atento acá a la ip que cambia constantemente.
const mqttClient = connectToMQTT(mqttURL);

// Rutas de la aplicación
app.use(bookRoutes);

// Manejo de solicitudes POST
app.post('/books', (req, res) => {
  // Aquí puedes acceder a las funciones definidas en mongodb.js y mqttLogic.js
  // Por ejemplo, puedes guardar los datos recibidos en MongoDB
  // y publicar un mensaje MQTT si es necesario
  res.send('Solicitud POST recibida');
});

// Iniciar el servidor
const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`Servidor Express.js en funcionamiento en el puerto ${PORT}`);
});
