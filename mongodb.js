const mongoose = require('mongoose');

// Función para conectar a MongoDB
async function connectToMongoDB(mongoURI) {
    try {
        await mongoose.connect(mongoURI, { useNewUrlParser: true, useUnifiedTopology: true });
        console.log('Conexión a MongoDB establecida correctamente');
    } catch (error) {
        console.error('Error al conectar a MongoDB:', error);
    }
}

// Define tus modelos y esquemas de Mongoose aquí, por ejemplo:
const bookSchema = new mongoose.Schema({
    title: String,
    author: String,
    pages: Number,
});

const Book = mongoose.model('Book', bookSchema);

// Funciones para realizar operaciones CRUD en la base de datos
async function createBook(bookData) {
    try {
        const newBook = new Book(bookData);
        await newBook.save();
        console.log('Libro creado exitosamente:', newBook);
        return newBook;
    } catch (error) {
        console.error('Error al crear el libro:', error);
    }
}

// Exporta las funciones que quieras utilizar en otros archivos
module.exports = {
    connectToMongoDB,
    createBook,
    // Agrega otras funciones CRUD aquí según sea necesario
};
