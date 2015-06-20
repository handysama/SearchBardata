SearchBarData
-----------------
This application purpose is to give user interface to perform search Bar data from database.
To insert data into database this application read input text files in CSV format from sub-folder 'Data' 
and insert them into SQLite database (e.g. @CL_Daily.db).

How to use it:
1. At the top window, there are two drop down lists which represent Symbol and threshold.
The threshold value is use for support/resistance calculation.

2. There are search condition box that allow user to add more condition to refine search result.
The maximum number of search condition rows are 50 rows and the minimum is 1 row.
Supported value data types are Date, Time, and Numeric.
Supported search operator are:
- equal (=)
- greater than or equal (>=)
- less than equal (<=)

3. After search condition has been defined, user should click 'Search' to begin search operation.
The program will build SQL query string based on conditions and execute it into database.
The query execution result will be shown in table below search condition box. 

4. To terminate program, user may click 'Quit' button or click 'X' at corner of window.
