#include <iomanip>
#include <string>
#include <vector>
#include <limits>
#include <memory>
#include <iomanip> 
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>  
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace sql;

// MySQL connection details
const string DB_HOST = "localhost";
const string DB_USER = "root";
const string DB_PASS = "2004";
const string DB_NAME = "movie_recommendation";

// Movie class
class Movie
{
public:
    string name;
    string genre;

    Movie(string n, string g) : name(n), genre(g) {}

    void setName(string n) { name = n; }
    void setGenre(string g) { genre = g; }
    string getName() const { return name; }
    string getGenre() const { return genre; }
};

// MovieCollection class
class MovieCollection
{
public:
    vector<Movie> m;

    void addMovie(const string& name, const string& genre)
    {
        try {
            auto driver = sql::mysql::get_mysql_driver_instance();
            unique_ptr<Connection> con(driver->connect(DB_HOST, DB_USER, DB_PASS));
            con->setSchema(DB_NAME);
            unique_ptr<PreparedStatement> pstmt(con->prepareStatement("INSERT INTO movies (name, genre) VALUES (?, ?)"));
            pstmt->setString(1, name);
            pstmt->setString(2, genre);
            pstmt->executeUpdate();
        }
        catch (SQLException& e) {
            cerr << "Error adding movie: " << e.what() << endl;
        }
    }

    vector<Movie> getMoviesByName(const string& name)
    {
        vector<Movie> result;
        try {
            auto driver = sql::mysql::get_mysql_driver_instance();
            unique_ptr<Connection> con(driver->connect(DB_HOST, DB_USER, DB_PASS));
            con->setSchema(DB_NAME);
            unique_ptr<PreparedStatement> pstmt(con->prepareStatement("SELECT name, genre FROM movies WHERE name = ?"));
            pstmt->setString(1, name);
            unique_ptr<ResultSet> res(pstmt->executeQuery());
            while (res->next()) {
                result.emplace_back(res->getString("name"), res->getString("genre"));
            }
        }
        catch (SQLException& e) {
            cerr << "Error fetching movies by name: " << e.what() << endl;
        }
        return result;
    }

    vector<Movie> getMoviesByGenre(const string& genre)
    {
        vector<Movie> result;
        try {
            auto driver = sql::mysql::get_mysql_driver_instance();
            unique_ptr<Connection> con(driver->connect(DB_HOST, DB_USER, DB_PASS));
            con->setSchema(DB_NAME);
            unique_ptr<PreparedStatement> pstmt(con->prepareStatement("SELECT name, genre FROM movies WHERE genre = ?"));
            pstmt->setString(1, genre);
            unique_ptr<ResultSet> res(pstmt->executeQuery());
            while (res->next()) {
                result.emplace_back(res->getString("name"), res->getString("genre"));
            }
        }
        catch (SQLException& e) {
            cerr << "Error fetching movies by genre: " << e.what() << endl;
        }
        return result;
    }

    vector<Movie> getAllMovies()
    {
        vector<Movie> result;
        try {
            auto driver = sql::mysql::get_mysql_driver_instance();
            unique_ptr<Connection> con(driver->connect(DB_HOST, DB_USER, DB_PASS));
            con->setSchema(DB_NAME);
            unique_ptr<Statement> stmt(con->createStatement());
            unique_ptr<ResultSet> res(stmt->executeQuery("SELECT name, genre FROM movies"));
            while (res->next()) {
                result.emplace_back(res->getString("name"), res->getString("genre"));
            }
        }
        catch (SQLException& e) {
            cerr << "Error fetching all movies: " << e.what() << endl;
        }
        return result;
    }


    // Utility function to get movies by ID
    vector<Movie> getMoviesById(int id) {
        vector<Movie> result;
        try {
            auto driver = sql::mysql::get_mysql_driver_instance();
            unique_ptr<Connection> con(driver->connect(DB_HOST, DB_USER, DB_PASS));
            con->setSchema(DB_NAME);
            unique_ptr<PreparedStatement> pstmt(con->prepareStatement("SELECT name, genre FROM movies WHERE id = ?"));
            pstmt->setInt(1, id);
            unique_ptr<ResultSet> res(pstmt->executeQuery());
            while (res->next()) {
                result.emplace_back(res->getString("name"), res->getString("genre"));
            }
        }
        catch (SQLException& e) {
            cerr << "Error fetching movies by id: " << e.what() << endl;
        }
        return result;
    }

    bool isGenreAvailable(const string& genre) {
        try {
            auto driver = sql::mysql::get_mysql_driver_instance();
            unique_ptr<Connection> con(driver->connect(DB_HOST, DB_USER, DB_PASS));
            con->setSchema(DB_NAME);
            unique_ptr<PreparedStatement> pstmt(con->prepareStatement("SELECT COUNT(*) FROM movies WHERE genre = ?"));
            pstmt->setString(1, genre);
            unique_ptr<ResultSet> res(pstmt->executeQuery());
            if (res->next() && res->getInt(1) > 0) {
                return true;
            }
        }
        catch (SQLException& e) {
            cerr << "Error checking genre availability: " << e.what() << endl;
        }
        return false;
    }

};

// Ratings class
class Ratings : public MovieCollection
{
public:
    int rating;

    Ratings() : rating(0) {}
    Ratings(int a) : rating(a) {}

    void Rating(int movie_id)
    {
        int R1;
        cout << "How much would you rate this movie out of 10: ";
        cin >> R1;

        while (cin.fail() || R1 > 10 || R1 < 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid rating. Try again: ";
            cin >> R1;
        }

        addRating(movie_id, R1);
        cout << "Rating Updated" << endl;
    }

    void addRating(int movie_id, const int& val)
    {
        try {
            auto driver = sql::mysql::get_mysql_driver_instance();
            unique_ptr<Connection> con(driver->connect(DB_HOST, DB_USER, DB_PASS));
            con->setSchema(DB_NAME);
            unique_ptr<PreparedStatement> pstmt(con->prepareStatement("INSERT INTO ratings (movie_id, rating) VALUES (?, ?)"));
            pstmt->setInt(1, movie_id);
            pstmt->setInt(2, val);
            pstmt->executeUpdate();
        }
        catch (SQLException& e) {
            cerr << "Error adding rating: " << e.what() << endl;
        }
    }

    vector<int> getAllRatings(int movie_id)
    {
        vector<int> result;
        try {
            auto driver = sql::mysql::get_mysql_driver_instance();
            unique_ptr<Connection> con(driver->connect(DB_HOST, DB_USER, DB_PASS));
            con->setSchema(DB_NAME);
            unique_ptr<PreparedStatement> pstmt(con->prepareStatement("SELECT rating FROM ratings WHERE movie_id = ?"));
            pstmt->setInt(1, movie_id);
            unique_ptr<ResultSet> res(pstmt->executeQuery());
            while (res->next()) {
                result.push_back(res->getInt("rating"));
            }
        }
        catch (SQLException& e) {
            cerr << "Error fetching ratings: " << e.what() << endl;
        }
        return result;
    }
};

// Main class
class Main : public MovieCollection
{
public:
    MovieCollection c;
    Ratings a;


    void initialCreatorSetup()
    {
        c.addMovie("Inception", "Sci-Fi");
        c.addMovie("The Dark Knight", "Action");
        c.addMovie("Pulp Fiction", "Crime");
        c.addMovie("Forrest Gump", "Drama");
        c.addMovie("The Shawshank Redemption", "Drama");
        c.addMovie("Fight Club", "Drama");
        c.addMovie("The Godfather", "Crime");
        c.addMovie("Schindler's List", "Biography");
        c.addMovie("Goodfellas", "Crime");
        c.addMovie("The Matrix", "Sci-Fi");
        c.addMovie("Saving Private Ryan", "War");
        c.addMovie("Gladiator", "Action");
        c.addMovie("The Silence of the Lambs", "Thriller");
        c.addMovie("Braveheart", "Biography");
        c.addMovie("The Departed", "Crime");
        c.addMovie("Titanic", "Romance");
        c.addMovie("The Lord of the Rings", "Adventure");
        c.addMovie("The Lion King", "Animation");
        c.addMovie("Jurassic Park", "Adventure");
        c.addMovie("The Shining", "Horror");
        c.addMovie("Toy Story", "Animation");
        c.addMovie("The Terminator", "Sci-Fi");
        c.addMovie("E.T. the Extra-Terrestrial", "Family");
        c.addMovie("The Green Mile", "Crime");
        c.addMovie("The Sixth Sense", "Mystery");
        c.addMovie("Avatar", "Adventure");
        c.addMovie("The Avengers", "Action");
        c.addMovie("Inglourious Basterds", "War");
        c.addMovie("The Social Network", "Biography");
        c.addMovie("Interstellar", "Sci-Fi");
        c.addMovie("Charli and the Choclate Factory", "Action");

        // Retrieve movie IDs from the database
        vector<int> movie_ids = getMovieIds();

        // Add ratings for the movies (dummy ratings for demonstration)
        a.addRating(movie_ids[0], 8);
        a.addRating(movie_ids[1], 8);
        a.addRating(movie_ids[2], 9);
        a.addRating(movie_ids[3], 7);
        a.addRating(movie_ids[4], 9);
        a.addRating(movie_ids[5], 10);
        a.addRating(movie_ids[6], 8);
        a.addRating(movie_ids[7], 9);
        a.addRating(movie_ids[8], 7);
        a.addRating(movie_ids[9], 8);
        a.addRating(movie_ids[10], 9);
        a.addRating(movie_ids[11], 8);
        a.addRating(movie_ids[12], 7);
        a.addRating(movie_ids[13], 8);
        a.addRating(movie_ids[14], 9);
        a.addRating(movie_ids[15], 7);
        a.addRating(movie_ids[16], 8);
        a.addRating(movie_ids[17], 9);
        a.addRating(movie_ids[18], 8);
        a.addRating(movie_ids[19], 7);
        a.addRating(movie_ids[20], 8);
        a.addRating(movie_ids[21], 9);
        a.addRating(movie_ids[22], 7);
        a.addRating(movie_ids[23], 8);
        a.addRating(movie_ids[24], 9);
        a.addRating(movie_ids[25], 8);
        a.addRating(movie_ids[26], 7);
        a.addRating(movie_ids[27], 8);
        a.addRating(movie_ids[28], 9);
        a.addRating(movie_ids[29], 7);
        a.addRating(movie_ids[30], 6);
    }



    vector<int> getMovieIds()
    {
        vector<int> ids;
        try {
            auto driver = sql::mysql::get_mysql_driver_instance();
            unique_ptr<Connection> con(driver->connect(DB_HOST, DB_USER, DB_PASS));
            con->setSchema(DB_NAME);
            unique_ptr<Statement> stmt(con->createStatement());
            unique_ptr<ResultSet> res(stmt->executeQuery("SELECT id FROM movies"));
            while (res->next()) {
                ids.push_back(res->getInt("id"));
            }
        }
        catch (SQLException& e) {
            cerr << "Error fetching movie IDs: " << e.what() << endl;
        }
        return ids;
    }

    int getMovieIdByName(const string& name)
    {
        int movie_id = -1; // Default to -1 if movie not found or error occurs

        try {
            auto driver = sql::mysql::get_mysql_driver_instance();
            unique_ptr<Connection> con(driver->connect(DB_HOST, DB_USER, DB_PASS));
            con->setSchema(DB_NAME);
            unique_ptr<PreparedStatement> pstmt(con->prepareStatement("SELECT id FROM movies WHERE name = ?"));
            pstmt->setString(1, name);
            unique_ptr<ResultSet> res(pstmt->executeQuery());

            if (res->next()) {
                movie_id = res->getInt("id");
            }
        }
        catch (SQLException& e) {
            cerr << "Error fetching movie ID by name: " << e.what() << endl;
        }

        return movie_id;
    }






    void displayMovies()
    {
        auto allMovies = c.getAllMovies();

        // Print table header
        cout << setw(5) << left << "ID";
        cout << setw(35) << left << "Name";
        cout << setw(15) << left << "Genre";
        cout << "Ratings" << endl;
        cout << setfill('-') << setw(75) << "-" << endl;
        cout << setfill(' '); // Reset fill character

        // Print each movie and its ratings
        for (const auto& movie : allMovies) {
            int movieId = getMovieIdByName(movie.getName());

            cout << setw(5) << left << movieId;
            cout << setw(35) << left << movie.getName();
            cout << setw(15) << left << movie.getGenre();

            // Get ratings for the current movie
            vector<int> ratings = a.getAllRatings(movieId);

            // Print ratings
            if (ratings.empty()) {
                cout << "No ratings yet." << endl;
            }
            else {
                for (size_t i = 0; i < ratings.size(); ++i) {
                    cout << ratings[i];
                    if (i != ratings.size() - 1) {
                        cout << ", ";
                    }
                }
                cout << endl;
            }
        }
    }



    void displayRatings()
    {
        auto movie_ids = getMovieIds();
        for (const auto& id : movie_ids) {
            auto ratings = a.getAllRatings(id);
            if (!ratings.empty()) {
                cout << "Ratings for movie ID " << id << ": ";
                for (const auto& rating : ratings) {
                    cout << rating << " ";
                }
                cout << endl;
            }
        }
    }

    void displayMenu()
    {
        cout << "************************************************************************" << endl;
        cout << "-----------------------MOVIE RECOMENDATION SYSTEM-----------------------" << endl;
        cout << "************************************************************************" << endl;
        cout << "1) Display All Movies." << endl;
        cout << "2) Recieve Recommendations" << endl;
        cout << "3) Rate a Movies" << endl;
        cout << "4) Search for movies by genre" << endl;
        cout << "5) Search for movies by name" << endl;
        cout << "6) Exit" << endl;
    }


    void recommendMovies() {
        // Step 1: Ask user for their top 3 movies by ID and their ratings
        int topMovieIds[3];
        int topMovieRatings[3];

        for (int i = 0; i < 3; ++i) {
            cout << "Enter the ID of your top movie #" << (i + 1) << ": ";
            cin >> topMovieIds[i];
            cout << "Enter your rating for this movie (out of 10): ";
            cin >> topMovieRatings[i];
        }

        // Step 2: Retrieve the genres of the top 3 movies
        vector<string> topGenres;
        for (int i = 0; i < 3; ++i) {
            vector<Movie> movies = c.getMoviesById(topMovieIds[i]);
            if (!movies.empty()) {
                topGenres.push_back(movies[0].getGenre());
            }
        }

        // Step 3: Determine the highest-rated genre
        map<string, int> genreRatings;
        for (int i = 0; i < 3; ++i) {
            genreRatings[topGenres[i]] += topMovieRatings[i];
        }

        string highestRatedGenre;
        int maxRating = 0;
        for (const auto& genreRating : genreRatings) {
            if (genreRating.second > maxRating) {
                maxRating = genreRating.second;
                highestRatedGenre = genreRating.first;
            }
        }

        // Step 4: Recommend 10 movies from the highest-rated genre
        cout << "Recommended movies in the genre: " << highestRatedGenre << endl;
        vector<Movie> recommendedMovies = c.getMoviesByGenre(highestRatedGenre);
        int count = 0;

        for (const auto& movie : recommendedMovies) {
            cout << setw(5) << left << getMovieIdByName(movie.getName());
            cout << setw(35) << left << movie.getName();
            cout << setw(15) << left << movie.getGenre();

            // Get ratings for the current movie
            vector<int> ratings = a.getAllRatings(getMovieIdByName(movie.getName()));

            // Print ratings
            if (ratings.empty()) {
                cout << "No ratings yet." << endl;
            }
            else {
                for (size_t i = 0; i < ratings.size(); ++i) {
                    cout << ratings[i];
                    if (i != ratings.size() - 1) {
                        cout << ", ";
                    }
                }
                cout << endl;
            }

            if (++count >= 10) break;
        }
    }

    void displayMoviesByGenre(const string& genre) {
        if (isGenreAvailable(genre)) {
            auto movies = c.getMoviesByGenre(genre);

            cout << left << setw(5) << "ID"
                << left << setw(35) << "Name"
                << left << setw(15) << "Genre"
                << "Ratings" << endl;

            cout << string(60, '-') << endl;

            for (const auto& movie : movies) {
                int movie_id = getMovieIdByName(movie.getName());
                cout << left << setw(5) << movie_id
                    << left << setw(35) << movie.getName()
                    << left << setw(15) << movie.getGenre();

                auto ratings = a.getAllRatings(movie_id);
                if (ratings.empty()) {
                    cout << "No ratings yet.";
                }
                else {
                    for (size_t i = 0; i < ratings.size(); ++i) {
                        cout << ratings[i];
                        if (i != ratings.size() - 1) {
                            cout << ", ";
                        }
                    }
                }
                cout << endl;
            }
        }
        else {
            cout << "Error: Genre '" << genre << "' not found in the database." << endl;
        }
    }
    void displayMovieByName()
    {
        string name;
        cin.ignore(); // Clear the input buffer
        cout << "Enter movie name: ";
        getline(cin, name);

        vector<Movie> movies = c.getMoviesByName(name);

        if (movies.empty()) {
            cout << "No movies found with the name \"" << name << "\".\n";
            return;
        }

        cout << left << setw(5) << "ID"
            << left << setw(35) << "Name"
            << left << setw(15) << "Genre"
            << "Ratings" << endl;

        cout << string(60, '-') << endl;

        for (const auto& movie : movies) {
            int movie_id = getMovieIdByName(movie.getName());
            cout << left << setw(5) << movie_id
                << left << setw(35) << movie.getName()
                << left << setw(15) << movie.getGenre();

            vector<int> ratings = a.getAllRatings(movie_id);
            if (ratings.empty()) {
                cout << "No ratings yet";
            }
            else {
                for (const int& rating : ratings) {
                    cout << rating << " ";
                }
            }
            cout << endl;
        }
    }




};

int main()
{
    try {
        auto driver = sql::mysql::get_mysql_driver_instance();
        unique_ptr<Connection> con(driver->connect(DB_HOST, DB_USER, DB_PASS));
        unique_ptr<Statement> stmt(con->createStatement());

        // Create database if it doesn't exist
        stmt->execute("CREATE DATABASE IF NOT EXISTS movie_recommendation");
        cout << "Database 'movie_recommendation' created successfully." << endl;

        // Use the 'movie_recommendation' database
        stmt->execute("USE movie_recommendation");

        // Create 'movies' table if it doesn't exist
        stmt->execute("CREATE TABLE IF NOT EXISTS movies ("
            "id INT AUTO_INCREMENT PRIMARY KEY,"
            "name VARCHAR(255) NOT NULL,"
            "genre VARCHAR(255) NOT NULL"
            ")");
        cout << "Table 'movies' created successfully." << endl;

        // Create 'ratings' table if it doesn't exist
        stmt->execute("CREATE TABLE IF NOT EXISTS ratings ("
            "id INT AUTO_INCREMENT PRIMARY KEY,"
            "movie_id INT NOT NULL,"
            "rating INT NOT NULL,"
            "FOREIGN KEY (movie_id) REFERENCES movies(id)"
            ")");
        cout << "Table 'ratings' created successfully." << endl;
       
        Main m;
        //m.initialCreatorSetup();

        int option;

        while (true) {
            m.displayMenu();
            cout << "Enter your choice: ";
            cin >> option;

            while (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Try again: ";
                cin >> option;
            }

            string genre;
            switch (option) {

            case 1:
                cout << "***************************************************************************" << endl;
                cout << "-------------------------------WELCOME USER--------------------------------" << endl;
                cout << "***************************************************************************" << endl;
                cout << "-------------------------------LIST OF MOVIES------------------------------" << endl;
                cout << "***************************************************************************" << endl;

                m.displayMovies();
                break;
            case 2:
                m.recommendMovies();
                break;
            case 3:
            {
                int movie_id;
                cout << "Enter movie ID to rate: ";
                cin >> movie_id;

                while (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Try again: ";
                    cin >> movie_id;
                }

                m.a.Rating(movie_id);
                break;
            }
            case 4:

                cout << "Enter genre to display movies: ";
                cin.ignore(); // Ignore the newline character left in the buffer
                getline(cin, genre);
                m.displayMoviesByGenre(genre);
                break;

            case 5:
                m.displayMovieByName();
                
                break;

            case 6:
                cout << "Thank you for using the program. Exiting..." << endl;
                return 0;
            default:
                cout << "Invalid choice. Please try again." << endl;
            }
        }
        con->close();
    }
    catch (SQLException& e) {
        cerr << "SQL Error: " << e.what() << endl;
    }

    return 0;
}