const express = require('express');
const router = express.Router();
const Book = require('../models/bookModels');

// Ruta para agregar un nuevo libro
router.post('/publish', async (req, res) => {
  
  // Crear una nueva instancia del libro utilizando el modelo de datos
  const newBook = new Book({
      title: req.body.title,
      author: req.body.author,
      genre: req.body.genre,
      year: req.body.year
  });

  try {
    // Guardar el nuevo libro en la base de datos
    const savedBook = await newBook.save();
    // Enviar una respuesta con el libro recién agregado
    res.status(201).json(savedBook);
  } catch (error) {
    // Manejar cualquier error que ocurra durante el proceso
    res.status(500).json({ message: error.message });
  }
});

module.exports = router;

// ...................................

// Route para obtener todos los libros

router.get('/get-books', async (req, res) => {
  try {
    // Consultar todos los libros en la base de datos
    const books = await Book.find();
    // Enviar una respuesta con los libros encontrados
    res.json(books);
  } catch (error) {
    // Manejar errores
    res.status(500).json({ message: error.message });
  }
});

// ...................................

// Route para actualizar un libro existente 

router.put('/modify/:id', async (req, res) => {
  try {
    const book = await Book.findById(req.params.id);
    if (!book) {
      return res.status(404).json({ message: 'Libro no encontrado' });
    }
    // Actualizar los campos del libro con los datos proporcionados en la solicitud
    book.title = req.body.title || book.title;
    book.author = req.body.author || book.author;
    book.genre = req.body.genre || book.genre;
    book.year = req.body.year || book.year;

    // Guardar los cambios en la base de datos
    const updatedBook = await book.save();
    // Enviar una respuesta con el libro actualizado
    res.json(updatedBook);
  } catch (error) {
    // Manejar errores
    res.status(500).json({ message: error.message });
  }
});


// ...................................

// Route para eliminar un libro existente 

router.delete('/books/:id', async (req, res) => {
  try {
    const book = await Book.findById(req.params.id);
    if (!book) {
      return res.status(404).json({ message: 'Libro no encontrado' });
    }
    // Eliminar el libro de la base de datos
    await book.remove();
    // Enviar una respuesta con un mensaje de éxito
    res.json({ message: 'Libro eliminado exitosamente' });
  } catch (error) {
    // Manejar errores
    res.status(500).json({ message: error.message });
  }
});
