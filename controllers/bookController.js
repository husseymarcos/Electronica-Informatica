const express = require('express');
const router = express.Router();
const bookService = require('../service/bookService');
const Book = require('../bookModels');

router.post('/api/books/publish', async (req, res) => {
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

/*
router.get('/get-books', async (req, res) => {
    try {
        const books = await bookService.getAllBooks();
        res.json(books);
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
});

router.put('/modify/:id', async (req, res) => {
    const { id } = req.params;
    const updatedBookData = req.body;
    try {
        const updatedBook = await bookService.updateBook(id, updatedBookData);
        res.json(updatedBook);
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
});

router.delete('/delete/:id', async (req, res) => {
    const { id } = req.params;
    try {
        await bookService.deleteBook(id);
        res.json({ message: 'Libro eliminado exitosamente' });
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
});
*/

module.exports = router;
