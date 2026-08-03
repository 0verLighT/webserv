#pragma once

namespace HttpStatus {
  enum Code {
    CONTINUE = 100,
    SWITCHING_PROTOCOLS = 101,
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,
    REDIRECT = 302,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
    BAD_GATEWAY = 502,
  };
}
