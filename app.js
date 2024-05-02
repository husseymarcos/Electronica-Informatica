const express = require('express');
const bodyParser = require('body-parser');
const { connectToMongoDB } = require('./mongodb');
const connectToMQTT = require('./mqttLogic');
const bookRoutes = require('./routes/bookRoutes');

const app = express();


require('dotenv').config();

// Middleware para analizar solicitudes JSON
app.use(bodyParser.json());




// Conexión a MongoDB en la nube
const mongoURI = process.env.MONGO_URI; // Atenco acá cambia la ip. 
connectToMongoDB(mongoURI);

// Conexión a MQTT
const mqttURL = process.env.MQTT_URL; // O la URL de tu servidor MQTT en la nube. Atento acá a la ip que cambia constantemente.
const mqttClient = connectToMQTT(mqttURL);

// Rutas de la aplicación
app.use("/books",bookRoutes);



// Iniciar el servidor
const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`Servidor Express.js en funcionamiento en el puerto ${PORT}`);
});
