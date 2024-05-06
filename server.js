const express = require('express');
const bodyParser = require('body-parser');
const { connectToMongoDB } = require('./mongodb');


const app = express();


// Accede al archivo .env
require('dotenv').config();


// Middleware para analizar solicitudes JSON
app.use(bodyParser.json());


// Conexión a MongoDB en la nube
const mongoURI = process.env.MONGO_URI; // Atenco acá cambia la ip. 
connectToMongoDB(mongoURI);


// ------------------------------------------------------------------------------------

// Rutas de la aplicación
app.get('/', (req, res) => {
    res.send('¡Hola desde Express!');
});

app.post('/api/books/publish', async(req, res) => {
    try {
        const { title, author, genre, year } = req.body;
        const book = new Book({
          title,
          author,
          genre,
          year
        });
    
        const savedBook = await book.save();
        res.status(201).json(savedBook);
      } catch (error) {
          res.status(500).json({ message: `Error al guardar en la base de datos: ${error.message}` });
      }
});


// Iniciar el servidor
const PORT = process.env.PORT || 5500;
app.listen(PORT, () => {
    console.log(`Servidor Express.js en funcionamiento en el puerto ${PORT}`);
});


// ----------------------------

// MQTT Logic

let mqttClient;

// Función para conectar al servidor MQTT
function connectToMQTT(mqttURL) {
    mqttClient = mqtt.connect(mqttURL);

    mqttClient.on('connect', () => {
        console.log('Conectado al servidor MQTT');
        // Suscribirse a los temas necesarios aquí
        mqttClient.subscribe('/api/books', (err) => {
            if (err) {
              console.error('Error al suscribirse:', err);
            } else {
              console.log('Suscripción exitosa a /api/books');
            }
        });
    
    });

    mqttClient.on('message', (topic, message) => {
        console.log('Mensaje recibido:', message.toString(), 'en el topic', topic);
        // Agregar lógica para manejar los mensajes recibidos aquí
    });

    // Exporta el cliente MQTT para poder usarlo en otros archivos si es necesario
    return mqttClient;
}

// Conexión a MQTT
const mqttURL = process.env.MQTT_URL;
const mqtt = require('mqtt');
connectToMQTT(mqttURL);


