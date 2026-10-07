# Upload

Run the following command:

```sh
curl -i -X POST \
  --data-binary "These are the file's contents." \
  http://0.0.0.0:8080/upload/file.txt
```

You should see this in the terminal:

```sh
HTTP/1.1 201 Created
Content-Length: 17
Content-Type: text/plain

Uploaded file.txt% 
```
