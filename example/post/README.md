# POST

The only purpose of this file is to serve as directions for demonstrating the POST method.

Run the following command:

```sh
curl -i -X POST \
  -d 'Hello, this is a test.' \
  'http://localhost:8080/example/post/getenv.py?debug=1'
```

You should see this in the terminal:

```sh
HTTP/1.1 200 OK
Content-Length: 101
Content-Type: text/plain

method=POST
query=debug=1
content_type=application/x-www-form-urlencoded
body=Hello, this is a test.
```
