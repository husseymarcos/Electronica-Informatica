const express = require('express');
const router = express.Router();
const bookService = require('../service/bookService');


router.post('/publish', async (req, res) => {
    const { title, author, genre, year } = req.body;
    try {
        const savedBook = await bookService.createBook(title, author, genre, year);
        res.status(201).json(savedBook);
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
});

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

module.exports = router;
