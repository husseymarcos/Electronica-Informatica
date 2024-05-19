const express = require('express');
const bodyParser = require('body-parser');
// const { connectToMongoDB } = require('./mongodb');
const {MongoClient} = require('mongodb');
const Book = require('./bookModels');

// Lógica que tenes acá:
/*Tenes la lógica de mqtt, y el bookController.*/ 

// Accede al archivo .env
require('dotenv').config();

const app = express();

// Middleware para analizar solicitudes JSON
app.use(bodyParser.json());


// Conexión a MongoDB en la nube
const mongoURI = process.env.MONGO_URI; 
const client = new MongoClient(mongoURI, {useNewUrlParser: true, useUnifiedTopology: true });

async function connectToMongoDB(){
    try{
        await client.connect();
        console.log('Conexión a MongoDB establecida correctamente');
    } catch(error){
        console.error('Error al conectar a MongoDB: ', error);
    }   
}

// Conexión a MongoDB

connectToMongoDB();


// MQTT Logic

// Función para conectar al servidor MQTT
function connectToMQTT(mqttURL) {
    mqttClient = mqtt.connect(mqttURL);

    mqttClient.on('connect', () => {
        console.log('Conectado al servidor MQTT');
        // Subscripción a los topics
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
        
        // Lógica para manejar los msj recibidos
        if(topic == '/api/books'){
            try{
                const bookData = JSON.parse(message.toString());
                const database = client.db(process.env.MONGO_DB_NAME);
                const booksCollection = database.collection('books');
                const result = await booksCollection.insertOne(bookData);
                // const book = new Book(bookData);
                // await book.save();
                console.log(`Libro guardado en la base de datos con el _id: ${result.insertedId}`);
            } catch(error){
                console.error('Error al guardar el libro', error);
            }
        }
    });

    // Exporta el cliente MQTT para poder usarlo en otros archivos si es necesario
    return mqttClient;
}

// Conexión a MQTT
const mqttURL = process.env.MQTT_URL;
const mqtt = require('mqtt');
connectToMQTT(mqttURL);



// Rutas de la aplicación

app.post('/api/books/publish', async(req, res) => {
    try {
        const { title, author, genre, year } = req.body;
        const book = new Book({ title, author, genre, year});
        const database = client.db(process.env.MONGO_DB_NAME);
        const booksCollection = database.collection('books');
        const result = await booksCollection.insertOne(book);
        res.status(201).json({ message: `Libro guardado con el _id: ${result.insertedId}` });
      } catch (error) {
          res.status(500).json({ message: `Error al guardar en la base de datos: ${error.message}` });
      }
});


// Iniciar el servidor
const PORT = process.env.PORT || 5500;
app.listen(PORT, () => {
    console.log(`Servidor Express.js en funcionamiento en el puerto ${PORT}`);
});




