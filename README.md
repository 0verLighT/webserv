This project has been created as part of the 42 curriculum by [amartel](https://intra.42.fr/users/amartel), [dnantet](https://intra.42.fr/users/dnantet)

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

```
Load configuration
	|
	v
Start server
	|
	v
Wait for request <---
	|				|
	v				|
Handle request		|
	|				|
	v				|
Send response -------
```
Here, handling a request can be either displaying a premade `.html` page or executing a script and displaying its output.

## Instructions

After cloning the repository, run `make` at the project's root then execute with
```sh
./webserv config/<config_file.toml>
```
A configuration file can contain the following parameters:
- **port** = *int* -> Which port is the server accessible on.
- **file** = *string* -> The filepath to access (if it's a html page) or execute (if it's a script).
- **executor** = *string* -> Filepath to the executor to use with a script. Optional if the script has a valid shebang.
- **cgi_enabled** = *boolean* -> CGI activation status. Optional, true by default.
- **timeout** = *int* -> How long the server waits (in milliseconds) before timeout. Optional, 5000 by default.

Any configuration file must be in `.toml` format. See official documentation [here](https://toml.io/en/). Note that for simplicity's sake, only key-value pairs are used in this project (no tables).

The server can then be accessed at **localhost:port**, or from any machine connected to the same local network with **host_ip:port**.

## Resources

- HTTP framing: [RFC 9112](https://www.rfc-editor.org/rfc/rfc9112.html), for message format, body length, chunked encoding, connection handling.
- HTTP semantics: [RFC 9110](https://www.rfc-editor.org/rfc/rfc9110.html), for methods, status codes, headers, redirects, `Content-Length`.
- CGI: [RFC 3875](https://www.rfc-editor.org/rfc/rfc3875.html), for meta-variables, script execution, stdin/stdout behavior.
- HTTP reference: [MDN HTTP messages](https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Messages) and [MDN status codes](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status).

### About AI
AI was used for debugging and stress-testing the CGI.
