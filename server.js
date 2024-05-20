const express = require('express');
const bodyParser = require('body-parser');
const mongoose = require('mongoose');
const path = require('path');
require('dotenv').config();
const config = require('./config');
const app = express();

app.use(bodyParser.json());

// Definimos la estructura del libro usando Mongoose
const bookSchema = new mongoose.Schema({
  title: { type: String, required: true },
  author: { type: String, required: true },
  genre: String,
  year: Number
});

const Book = mongoose.model('Book', bookSchema);

// Middleware para CORS y Content Security Policy
app.use((req, res, next) => {
    res.header('Access-Control-Allow-Origin', '*');
    res.header('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE');
    res.header('Access-Control-Allow-Headers', 'Content-Type');
    res.header('Content-Security-Policy', "default-src 'none'; img-src 'self' http://54.147.184.97:27017");
    next();
});

app.use(express.static(path.join(__dirname, 'public')));

// Conexión a MongoDB usando Mongoose
const mongoURI = `mongodb://${config.mongodb.hostname}:${config.mongodb.port}/${config.mongodb.database}`;

mongoose.connect(mongoURI, { useNewUrlParser: true, useUnifiedTopology: true })
  .then(() => console.log('Conexión a MongoDB establecida correctamente'))
  .catch(error => console.error('Error al conectar a MongoDB:', error));

// Lógica de MQTT
const mqttURL = process.env.MQTT_URL;
const mqtt = require('mqtt');

function connectToMQTT(mqttURL) {
    const mqttClient = mqtt.connect(mqttURL);

    mqttClient.on('connect', () => {
        console.log('Conectado al servidor MQTT');
        mqttClient.subscribe('/api/books', (err) => {
            if (err) {
                console.error('Error al suscribirse:', err);
            } else {
                console.log('Suscripción exitosa a /api/books');
            }
        });
    });

    mqttClient.on('message', async (topic, message) => {
        console.log('Mensaje recibido:', message.toString(), 'en el topic', topic);
        
        if (topic === 'api/books') {
            try {
                const bookData = JSON.parse(message.toString());
                const book = new Book(bookData);
                const result = await book.save();
                console.log(`Libro guardado en la base de datos con el _id: ${result._id}`);
            } catch (error) {
                console.error('Error al guardar el libro', error);
            }
        }
    });

    return mqttClient;
}

connectToMQTT(mqttURL);

// Ruta para agregar libros
app.post('/api/books/publish', async (req, res) => {
    try {

        const database = client.db(config.mongodb.database);
        const bookCollection = database.collection("books");

        const { title, author, genre, year } = req.body;
        const bookData = {
            title: title,
            author: author,
            genre: genre,
            year: year
        }

        const book = new Book(bookData);
        const result = await book.save();
        res.status(201).json({ message: `Libro guardado con el _id: ${result._id}` });

    } catch (error) {
        res.status(500).json({ message: `Error al guardar en la base de datos: ${error.message}` });
    }
});

// Iniciar el servidor
const PORT = process.env.PORT || 27017;
app.listen(PORT, () => {
    console.log(`Servidor Express.js en funcionamiento en el puerto ${PORT}`);
});






