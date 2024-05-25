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
                    <p>Author: ${book.author}</p>
                    <p>Genre: ${book.genre}</p>
                    <p>Year: ${book.year}</p>
                    <button onclick="deleteBook('${book._id}')">Delete</button>
                `;
                booksList.appendChild(listItem);
            });
        })
        .catch(error => {
            console.error('Error:', error);
            // Handle error, show message to user
        });
}

// Function to delete a book
function deleteBook(bookId) {
    const confirmed = confirm("Are you sure you want to delete this book?");
    if (confirmed) {
        fetch(`/api/books/delete/${bookId}`, {
            method: 'DELETE',
            headers: {
                'Content-Type': 'application/json'
            }
        })
            .then(response => {
                if (!response.ok) {
                    throw new Error(`Failed to delete book: ${response.status} ${response.statusText}`);
                }
                // Reload the books list after deletion
                fetchAllBooks();
            })
            .catch(error => {
                console.error('Error:', error);
                alert('Failed to delete book. Please try again.');
                // Handle error, show message to user
            });
    }
}

// Function to redirect to edit page for a book
/*function editBook(bookId) {
    // Redirect to edit page with book ID in query parameter
    window.location.href = `editBook.html?id=${bookId}`;
}*/


// Entry point when the page loads
document.addEventListener("DOMContentLoaded", function() {
    // Fetch all books when the page loads
    fetchAllBooks();
});
