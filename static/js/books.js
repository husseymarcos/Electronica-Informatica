// Function to fetch all books
function fetchAllBooks() {
    fetch('/api/books')
        .then(response => {
            if (!response.ok) {
                throw new Error(`Failed to fetch books: ${response.status} ${response.statusText}`);
            }
            return response.json();
        })
        .then(books => {
            const booksList = document.getElementById('books-list');

            // Clear existing list
            booksList.innerHTML = '';

            books.forEach(book => {
                const listItem = document.createElement('li');
                listItem.innerHTML = `
                    <h3>${book.title}</h3>
                    <p>Autor: ${book.author}</p>
                    <p>Género: ${book.genre}</p>
                    <p>Año: ${book.year}</p>
                    <button onclick="requestBook('${book._id}')">Solicitar</button>
                `;
                booksList.appendChild(listItem);
            });
        })
        .catch(error => {
            console.error('Error:', error);
            const messageElement = document.getElementById('message');
            messageElement.innerText = "Error al cargar la lista de libros. Por favor, intenta nuevamente más tarde.";
        });
}

// Function to request a book
function requestBook(bookId) {
    fetch(`/api/books/request/${bookId}`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        }
    })
    .then(response => {
        if (!response.ok) {
            throw new Error(`Failed to request book: ${response.status} ${response.statusText}`);
        }
        const messageElement = document.getElementById('message');
        messageElement.innerText = "Libro solicitado exitosamente.";
        messageElement.style.color = 'green';
    })
    .catch(error => {
        console.error('Error:', error);
        const messageElement = document.getElementById('message');
        messageElement.innerText = "Error al solicitar el libro. Por favor, intenta nuevamente.";
        messageElement.style.color = 'red';
    });
}

// Entry point when the page loads
document.addEventListener("DOMContentLoaded", function() {
    // Fetch all books when the page loads
    fetchAllBooks();
});
