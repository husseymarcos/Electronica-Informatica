const express = require('express');
const bodyParser = require('body-parser');
const { MongoClient } = require('mongodb');
const Book = require('./bookModels');
const path = require('path');
require('dotenv').config();

const app = express();

app.use(bodyParser.json());

app.use((req, res, next) => {
    res.header('Access-Control-Allow-Origin', '*');
    res.header('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE');
    res.header('Access-Control-Allow-Headers', 'Content-Type');
    // Actualiza la directiva CSP para permitir la carga de imágenes desde la URL específica
    res.header('Content-Security-Policy', "default-src 'none'; img-src 'self' http://54.147.184.97:27017");
    next();
});

app.use(express.static(path.join(__dirname, 'public')));

const mongoURI = process.env.MONGO_URI; 
const client = new MongoClient(mongoURI, { useNewUrlParser: true, useUnifiedTopology: true });

async function connectToMongoDB() {
    try {
        await client.connect();
        console.log('Conexión a MongoDB establecida correctamente');
    } catch (error) {
        console.error('Error al conectar a MongoDB: ', error);
    }   
}

connectToMongoDB();

const mqttURL = process.env.MQTT_URL;
const mqtt = require('mqtt');
connectToMQTT(mqttURL);

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
                const database = client.db(process.env.MONGO_DB_NAME);
                const booksCollection = database.collection('books');
                const result = await booksCollection.insertOne(bookData);
                console.log(`Libro guardado en la base de datos con el _id: ${result.insertedId}`);
            } catch (error) {
                console.error('Error al guardar el libro', error);
            }
        }
    });

    return mqttClient;
}

app.post('/api/books/publish', async (req, res) => {
    try {
        const { title, author, genre, year } = req.body;
        const book = new Book({ title, author, genre, year });

        const database = client.db(process.env.MONGO_DB_NAME);
        const booksCollection = database.collection('books');
        const result = await booksCollection.insertOne(book);
        res.status(201).json({ message: `Libro guardado con el _id: ${result.insertedId}` });
    } catch (error) {
        res.status(500).json({ message: `Error al guardar en la base de datos: ${error.message}` });
    }
});

const PORT = process.env.PORT || 27017;
app.listen(PORT, () => {
    console.log(`Servidor Express.js en funcionamiento en el puerto ${PORT}`);
});






