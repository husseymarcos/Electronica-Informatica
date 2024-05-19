const Book = require('../bookModels');

exports.createBook = async (title, author, genre, year) => {
    const newBook = new Book({
        title,
        author,
        genre,
        year
    });

    try {
        const savedBook = await newBook.save();
        return savedBook;
    } catch (error) {
        throw new Error(error.message);
    }
};

exports.getAllBooks = async () => {
    try {
        const books = await Book.find();
        return books;
    } catch (error) {
        throw new Error(error.message);
    }
};

exports.updateBook = async (id, updatedBookData) => {
    try {
        const book = await Book.findById(id);
        if (!book) {
            throw new Error('Libro no encontrado');
        }
        Object.assign(book, updatedBookData);
        const updatedBook = await book.save();
        return updatedBook;
    } catch (error) {
        throw new Error(error.message);
    }
};

exports.deleteBook = async (id) => {
    try {
        const book = await Book.findById(id);
        if (!book) {
            throw new Error('Libro no encontrado');
        }
        await book.remove();
    } catch (error) {
        throw new Error(error.message);
    }
};
