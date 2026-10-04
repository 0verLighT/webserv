# File too large

Run the following command:

```sh
head -c 2048 /dev/zero | tr '\0' 'A' > /tmp/too-large.txt
curl -i -X POST --data-binary @/tmp/too-large.txt http://localhost:8080/
```

You should see this in the terminal:

```sh
HTTP/1.1 413 Payload Too Large
Content-Length: 318
Content-Type: text/html

<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Payload Too Large</title>
	<style>
	    h1, p {
			text-align: center;
		}
	</style>
</head>
<body>
    <h1>Payload Too Large</h1>
    <hr>
    <p>webserv</p>
</body>
</html>%
```
