const express = require('express');
const bodyParser = require('body-parser');
const { connectToMongoDB } = require('./mongodb');
const {connectToMQTT, publishToMQTT} = require('./mqttLogic');
// const bookController = require('./controllers/bookController');
// const bookService = require('./service/bookService');
const Book = require('./models/bookModels');

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
// app.use("/api/books",bookController);




// Iniciar el servidor
const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`Servidor Express.js en funcionamiento en el puerto ${PORT}`);
});


// Publicar un mensaje en el tema 'topic/ejemplo'
// publishToMQTT('topic/ejemplo', 'Hola desde node.js');

// Lógica para realizar operaciones en la base de datos de mongodb

// Add Book
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



// -----------------------------------------

// Get Books
app.get('/api/books/get-books', async(req, res) => {
 
});


// -----------------------------------------

// Update Book
app.put('/api/books/modify/:id', async (req, res) => {
 
});

// -----------------------------------------

// Delete Book

app.delete('/api/books/delete/:id', async (req, res) => {

});
