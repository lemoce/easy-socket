
#include <string>
#include <memory>

#include <errno.h>
#include <string.h>

#include <fcntl.h>

#include "socket.h"
#include "config.h"

#include <iostream>

void lemoce::initializeSocketAPI() { }

bool lemoce::isClosed(int fd)
{
  return (fcntl(fd, F_GETFL) < 0 && errno == EBADF);
}

int lemoce::closeSocket(int fd)
{
  return ::close(fd);
}

void lemoce::treatSocketError(bool cleanup)
{
  throw SocketException{errno};
}

std::string lemoce::getMessageError(int error)
{
  std::string tmp{::strerror(error)};
  return tmp;
}
