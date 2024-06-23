// Function to fetch my books
function fetchMyBooks() {
    fetch('/api/mybooks')
        .then(response => {
            if (!response.ok) {
                throw new Error(`Failed to fetch books: ${response.status} ${response.statusText}`);
            }
            return response.json();
        })
        .then(books => {
            const booksList = document.getElementById('my-books-list');

            // Clear existing list
            booksList.innerHTML = '';

            books.forEach(book => {
                console.log(book);
                const listItem = document.createElement('li');
                listItem.innerHTML = `
                    <h3>${book.title}</h3>
                    <p>Autor: ${book.author}</p>
                    <p>Género: ${book.genre}</p>
                    <p>Año: ${book.year}</p>
                    <button onclick="returnBook('${book._id}')">Devolver</button>
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


// TODO: La parte de devolución, lo podemos ver en un branch aparte de última, porque debo reevaluar la lógica para borrar de la db, y todas esas cuestiones.
function returnBook(bookId) { // TODO: Este debe devolver el libro, a la base de datos. Para que pueda volver a ser solicitado.
    console.log("Id del libro devuelto: ", bookId);
    
    fetch('/api/mybooks/return', {  // TODO: Ver si esto tiene sentido
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        }, 
        body: JSON.stringify({ id: bookId })
    })
    .then(response => {
        if (!response.ok) {
            throw new Error(`Failed to request book: ${response.status} ${response.statusText}`);
        }
        const messageElement = document.getElementById('message');
        messageElement.innerText = "Libro devuelto exitosamente.";
        messageElement.style.color = 'green';
    })
    .catch(error => {
        console.error('Error:', error);
        const messageElement = document.getElementById('message');
        messageElement.innerText = "Error al devolver el libro. Por favor, intenta nuevamente.";
        messageElement.style.color = 'red';
    });
}

// Entry point when the page loads
document.addEventListener("DOMContentLoaded", function() {
    // Fetch all books when the page loads
    fetchMyBooks();
});
