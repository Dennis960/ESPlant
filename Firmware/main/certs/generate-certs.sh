openssl req -newkey rsa:2048 -nodes -keyout prvtkey.pem -x509 -days 3650 -out servercert.pem -subj "/CN=Blumy HTTPS Server"