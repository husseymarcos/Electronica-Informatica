/*Si llega a fallar, muy posiblemente sea por el manejo que tenemos por este lado
Porque por la parte de backend, considero que está todo bien, pero veamos que ocurre!*/

function addBook() {

    event.preventDefault();
    
    const title = document.getElementById('title').value;
    const author = document.getElementById('author').value;
    const genre = document.getElementById('genre').value;
    const year = document.getElementById('year').value;

    // Check if any field is empty
    if (!title.trim() || !author.trim()) {
        document.getElementById('error-message').innerText = "Title and author are required.";
        document.getElementById('error-message').style.display = 'block';
        return;
    }

    const newBook = {
        title: title,
        author: author,
        genre: genre,
        year: parseInt(year)
    };

    fetch('/api/books/publish', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(newBook)
    })
    .then(response => {
        if (response.ok) {
            // Book successfully added
            document.getElementById('success-message').innerText = "Book added successfully!";
            document.getElementById('success-message').style.color = 'green';
            document.getElementById('success-message').style.display = 'block';
            document.getElementById('error-message').style.display = 'none';
            document.getElementById('add-book-form-fields').reset();
        } else {
            // Error adding book
            document.getElementById('error-message').innerText = "Failed to add book. Please try again later.";
            document.getElementById('error-message').style.color = 'red';
            document.getElementById('error-message').style.display = 'block';
            document.getElementById('success-message').style.display = 'none';
        }
    })
    .catch(error => {
        console.error('Error:', error);
        // Handle error, show message to user
        document.getElementById('error-message').innerText = "Failed to add book. Please try again later.";
        document.getElementById('error-message').style.color = 'red';
        document.getElementById('error-message').style.display = 'block';
        document.getElementById('success-message').style.display = 'none';
    });
}

document.getElementById('add-book-form-fields').addEventListener('submit', addBook);