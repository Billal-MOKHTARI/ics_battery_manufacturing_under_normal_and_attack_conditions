ALTER USER 'root'@'localhost' IDENTIFIED BY 'z6LAiebNyg2P5YxOyhOT';

-- Create a new user to access mysql service remotely
CREATE USER 'root'@'192.168.50.7' IDENTIFIED BY 'ggdzc345befv43TEFt';

-- Grant privileges (all databases in this example)
GRANT ALL PRIVILEGES ON *.* TO 'root'@'192.168.50.7' WITH GRANT OPTION;

-- Apply changes
FLUSH PRIVILEGES;