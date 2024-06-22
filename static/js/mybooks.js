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
                const content = book.content; // Access the content object directly

                const listItem = document.createElement('li');
                listItem.innerHTML = `
                    <h3>${content.title}</h3>
                    <p>Autor: ${content.author}</p>
                    <p>Género: ${content.genre}</p>
                    <p>Año: ${content.year}</p>
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
/*function returnBook(bookId) { // TODO: Este debe devolver el libro, a la base de datos. Para que pueda volver a ser solicitado.
    console.log("Id del libro devuelto: ", bookId);
    
    fetch('/api/mybooks', {  // TODO: Ver si esto tiene sentido
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
}*/
