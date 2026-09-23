*This project has been created as part of the 42 curriculum by [amartel](https://intra.42.fr/users/amartel), [dnantet](https://intra.42.fr/users/dnantet)*

# webserv 🌐

503 Service Unavailable

## Description

The goal of this project is to make a basic HTTP server in C++98.

A web server is a system that listens for requests from browsers and sends back the resources needed to display a website.

### How it works

When a user types: `https://example.com`

1. **The browser sends a request**\
It asks the server: *“Give me the website at example.com.”*
2. **The request reaches the web server**\
The server receives the request and figures out what the user is asking for.
3. **The server finds or creates the response**\
It might retrieve an HTML file, image, CSS file, or data from a database.
4. **The server sends the response back**\
The browser receives the files/data.
5. **The browser displays the website**\
It turns the HTML, CSS, and JavaScript into the webpage the user sees.

### Overview

The basic lifecycle of this web server is as follow:

```md
Load configuration
    |
    v
Start server
    |
    v
Wait for request <---
    |                |
    v                |
Handle request       |
    |                |
    v                |
Send response -------
```

Here, handling a request can be either displaying a premade `.html` page or executing a script and displaying its output.

## Instructions

After cloning the repository, run `make` at the project's root then execute with

```sh
./webserv config/<config_file.toml>
```

A configuration file is a TOML document. The server reads a `[server]` table and can also accept nested route settings under `[server.route]`-style sections.

```toml
[server]
port = 8080
host = "0.0.0.0"
server_name = "localhost"
file = "example/time/time.py"
executor = "/usr/bin/python"
cgi_enabled = true
autoindex = true
timeout = 20000
root = "html"
default_error_page = "error.html"
max_body_size = 1048576

[server.route]
root = "html"
methods = ["GET", "POST", "DELETE"]
autoindex = true
default_file = "index.html"

```

Supported server-level parameters:

- **port** = *int* -> the port the server listens on.
- **host** = *string* -> the bind address.
- **server_name** = *string* -> the default server name.
- **file** = *string* -> the default file used for startup/default route handling.
- **executor** = *string* -> the program used to execute CGI scripts.
- **cgi_enabled** = *boolean* -> enable or disable CGI execution.
- **autoindex** = *boolean* -> enable or disable directory listing.
- **timeout** = *int* -> server poll timeout in milliseconds.
- **root** = *string* -> base document root.
- **default_error_page** = *string* -> fallback custom error page.
- **max_body_size** = *int* -> maximum client request body size in bytes.

Supported route-level parameters:

- **root** = *string* -> directory to serve for the route.
- **methods** = *array of strings* -> accepted HTTP methods.
- **autoindex** = *boolean* -> enable or disable directory listing for that route.
- **default_file** = *string* -> file to serve when the route points to a directory.
- **upload_path** = *string* -> directory where uploaded files are stored.
- **redirect** = *string* -> redirect target for the route.
- **cgi_enabled** = *boolean* -> CGI enable/disable for that route.
- **cgi_extension** = *string* -> file extension to treat as CGI.
- **default_error_page** = *string* -> route-specific error page.
- **max_body_size** = *int* -> maximum request size for the route.

Any configuration file must be in `.toml` format. See [official documentation](https://toml.io/en/).

The server can then be accessed at **localhost:port**, or from any machine connected to the same local network with **host_ip:port**.

### Direct HTTP methods test

- GET

```sh
curl -i 'http://localhost:8080/example/get/GETME.md'
```

- POST

```sh
curl -i -X POST \
  -d 'Hello, this is a test' \
  'http://127.0.0.1:8080/example/post/getenv.py?debug=1'
```

- DELETE

```sh
curl -i -X DELETE http://localhost:8080/assets/DELETEME.md
```

## Resources

- HTTP framing: [RFC 9112](https://www.rfc-editor.org/rfc/rfc9112.html), for message format, body length, chunked encoding, connection handling.
- HTTP semantics: [RFC 9110](https://www.rfc-editor.org/rfc/rfc9110.html), for methods, status codes, headers, redirects, `Content-Length`.
- CGI: [RFC 3875](https://www.rfc-editor.org/rfc/rfc3875.html), for meta-variables, script execution, stdin/stdout behavior.
- HTTP reference: [MDN HTTP messages](https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Messages) and [MDN status codes](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status).

### About AI

AI was used for :

- setting up the boring stuff
- debugging and stress-testing the CGI.
