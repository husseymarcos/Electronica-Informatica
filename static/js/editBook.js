// Function to fetch book details by ID
function fetchBookDetails(bookId) {
    fetch(`/api/books/${bookId}`)
        .then(response => {
            if (!response.ok) {
                throw new Error(`Failed to fetch book details: ${response.status} ${response.statusText}`);
            }
            return response.json();
        })
        .then(book => {
            // Populate form fields with book details
            document.getElementById('book-id').value = book._id; // Utilizamos el campo _id en lugar de sportId
            document.getElementById('book-title').value = book.title;
            document.getElementById('book-author').value = book.author;
            document.getElementById('book-genre').value = book.genre;
            document.getElementById('book-year').value = book.year;
        })
        .catch(error => {
            console.error('Error:', error);
            // Handle error, show message to user
        });
}

// Function to handle form submission
function updateBook() {
    const bookId = document.getElementById('book-id').value;
    const title = document.getElementById('book-title').value;
    const author = document.getElementById('book-author').value;
    const genre = document.getElementById('book-genre').value;
    const year = document.getElementById('book-year').value;

    const updatedBook = {
        title: title,
        author: author,
        genre: genre,
        year: year
    };

    fetch(`/api/books/modify/${bookId}`, {
        method: 'PUT',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(updatedBook)
    })
        .then(response => {
            if (!response.ok) {
                throw new Error(`Failed to update book: ${response.status} ${response.statusText}`);
            }
            // Redirect back to books list after successful update
            window.location.href = 'books.html';
        })
        .catch(error => {
            console.error('Error:', error);
            // Handle error, show message to user
        });
}

// Function to get book ID from URL query parameter
function getBookIdFromUrl() {
    const queryString = window.location.search;
    const urlParams = new URLSearchParams(queryString);
    return urlParams.get('id');
}

// Entry point when the page loads
document.addEventListener("DOMContentLoaded", function() {
    const bookId = getBookIdFromUrl();
    if (bookId) {
        // Fetch and populate book details
        fetchBookDetails(bookId);
    } else {
        console.error('Book ID not found in URL');
    }
});
