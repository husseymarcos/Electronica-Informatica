const mongoose = require('mongoose');

// Definimos la estructura de como queremos tener el dato

const bookSchema = new mongoose.Schema({
  title: { type: String, required: true },
  author: { type: String, required: true },
  genre: String,
  year: Number
});

const Book = mongoose.model('Book', bookSchema);

module.exports = Book;