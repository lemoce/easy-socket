
#include "socket.h"
#include "config.h"
#include <sstream>
#include <iostream>

void lemoce::initializeSocketAPI() {
  // so precisa ser chamado 1 vez em toda execuçao do programa
  // mas pode ser chamado varias vezes sem problemas
  // desde que sejam chamado no mesmo numero de vezes WSACleanup
  WSADATA wsaData;

  int result = ::WSAStartup(MAKEWORD(2,2), &wsaData);
  if (result < 0) {
    {
      treatSocketError(true);
    }
  }
}

bool lemoce::isClosed(int fd)
{
  struct timeval tv = {0, 0};
  fd_set fds;

  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  
  int rc = ::select(0, &fds, nullptr, nullptr, &tv);
  if (!FD_ISSET(fd, &fds))
    {
      return false;
    }
  u_long n = 0;
  
  ioctlsocket(fd, FIONREAD, &n);
  return n == 0;
}

int lemoce::closeSocket(int fd)
{

  // nao ha necessidade de se chamar o shutdown pois usamos blocking read
  int result = ::closesocket(fd);
  ::WSACleanup();

  return result;
}

void lemoce::treatSocketError(bool cleanup)
{
  int error;
  error = ::WSAGetLastError();

  if (cleanup)
    {
      ::WSACleanup();
    }
  throw SocketException{error};
}

std::string lemoce::getMessageError(int error)
{

  char *buffer;
  ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
		  nullptr,
		  error,
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR)&buffer,
		  1024,
		  nullptr);

  std::ostringstream oss;

  oss << buffer;

  ::LocalFree(buffer);
  
  return oss.str();
}
