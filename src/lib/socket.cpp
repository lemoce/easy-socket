#include <string.h>
#include <errno.h>

#include <iostream>

#include <string>
#include <sstream>
#include <memory>

#include "config.h"
#include "socket.h"

// TODO Remover Message Buffer
int lemoce::ServerSocket::BACKLOG = 512;

lemoce::Socket::Socket()
{
  
  initializeSocketAPI();
  
  this->fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (this->fd < 0)
    {
      treatSocketError(true);
    }
}

int lemoce::Socket::getFd() const
{
  return fd;
}

void lemoce::Socket::setFd(int fdToSet)
{
  fd = fdToSet;
}

bool lemoce::Socket::isClosed()
{
  return lemoce::isClosed(fd);
}

void lemoce::Socket::close()
{
  if(closeSocket(fd) < 0)
    {
      throw SocketException{errno};
    }
}


lemoce::ClientSocket::ClientSocket()
{
  bufferSize = 8192;
}


lemoce::ClientSocket::ClientSocket(std::string remote, int port): ClientSocket{}
{
  remotePort = port;
  remoteHost = remote;
}

void lemoce::ClientSocket::connect()
{

  struct sockaddr_in serveraddr;
  
  ::memset(&serveraddr, 0, sizeof(struct sockaddr_in));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = ::htons(remotePort);
  if (::inet_pton(AF_INET, remoteHost.c_str(), &serveraddr.sin_addr) < 0)
    {
      treatSocketError(false);
    }

  if (::connect(getFd(), reinterpret_cast<struct sockaddr *>(&serveraddr), sizeof(serveraddr)) < 0)
    {
      treatSocketError(false);
    }

  struct sockaddr_in clientaddr;
  socklen_t clientlen = sizeof(clientaddr);
  if (::getsockname(getFd(), reinterpret_cast<struct sockaddr *>(&clientaddr), &clientlen) < 0)
    {
      treatSocketError(false);
    }

  std::string lHost{::inet_ntoa(clientaddr.sin_addr)};
  int lPort = ::ntohs(clientaddr.sin_port);
  localHost = lHost;
  localPort = lPort;
}

std::unique_ptr<std::string> lemoce::ClientSocket::read()
{
  char chararray[bufferSize];
  std::ostringstream buffer;
  int nread;

  read(chararray, bufferSize);
    
  buffer << chararray;
  
  std::unique_ptr<std::string> result{new std::string{buffer.str()}};

  return result;
}

int lemoce::ClientSocket::read(char msg[], const size_t n)
{
  size_t nleft;
  ssize_t nread;
  char *ptr;

  ptr = msg;
  nleft = n;

  while (nleft > 0)
    {
      if ((nread = ::recv(getFd(), ptr, n, 0)) < 0)
        {
          if (errno == EINTR)
            {
              nread = 0;
              continue;
            }
          else
            treatSocketError(false);
        }

      nleft -= nread;
      ptr = ptr + nread;
      break;
    }

  if (nleft > 0)
    {
      msg[n - nleft] = '\0';
    }

  return (n - nleft);
}

// TODO Modificar para array de bytes
void lemoce::ClientSocket::write(const std::string& message)
{
  write(message.c_str(), message.size());
}

void lemoce::ClientSocket::write(const char * msg, size_t n)
{
  size_t nleft = n;
  ssize_t nwritten = nleft;
  const char * ptr = msg;

  while (nleft > 0)
    {
      if ((nwritten = ::send(getFd(), ptr, nleft, 0)) <= 0)
        {
          if (nwritten < 0 && errno == EINTR)
            nwritten = 0;
          else
            treatSocketError(false);
        }
      nleft -= nwritten;
      ptr += nwritten;
    }  
}

void lemoce::ClientSocket::setRemoteHost(std::string rHost)
{
  remoteHost = rHost;
}

std::string lemoce::ClientSocket::getRemoteHost() const
{
  return remoteHost;
}

void lemoce::ClientSocket::setRemotePort(int rPort)
{
  remotePort = rPort;
}

int lemoce::ClientSocket::getRemotePort() const
{
  return remotePort;
}

void lemoce::ClientSocket::setLocalHost(std::string lHost)
{
  localHost = lHost;
}

std::string lemoce::ClientSocket::getLocalHost() const
{
  return localHost;
}

void lemoce::ClientSocket::setLocalPort(int lPort)
{
  localPort = lPort;
}

int lemoce::ClientSocket::getLocalPort() const
{
  return localPort;
}

void lemoce::ClientSocket::setBufferSize(int size)
{
  bufferSize = size;
}

int lemoce::ClientSocket::getBufferSize() const
{
  return bufferSize;
}


lemoce::ServerSocket::ServerSocket(int lPort):
  lemoce::Socket{}, localPort(lPort) { }

void lemoce::ServerSocket::listen()
{
  struct sockaddr_in serveraddr;
  
  ::memset(&serveraddr, 0, sizeof(struct sockaddr_in));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
  serveraddr.sin_port = ::htons(localPort);

  if (::bind(getFd(), reinterpret_cast<struct sockaddr *>(&serveraddr), sizeof(serveraddr)) < 0)
    {
      throw SocketException(errno);
    }

  if (::listen(getFd(), 5) < 0)
    {
      throw SocketException(errno);
    }

}

void lemoce::ServerSocket::accept(lemoce::ClientSocket & client)
{

  struct sockaddr_in clientaddr;
  socklen_t clientaddr_len = sizeof(clientaddr);

  int client_fd = ::accept(getFd(), reinterpret_cast<struct sockaddr *>(&clientaddr), &clientaddr_len);
  if (client_fd < 0)
    {
      throw SocketException{errno};
    }

  client.setFd(client_fd);

  std::string localHost{::inet_ntoa(clientaddr.sin_addr)};
  int localPort = ::ntohs(clientaddr.sin_port);
  client.setLocalHost(localHost);
  client.setLocalPort(localPort);

  struct sockaddr_in serveraddr;
  socklen_t serveraddr_len;
  if (::getsockname(getFd(), reinterpret_cast<struct sockaddr *>(&serveraddr), &serveraddr_len) >= 0)
    {
      std::string serverHost{::inet_ntoa(serveraddr.sin_addr)};
      client.setRemoteHost(serverHost);

      int serverPort = ::ntohs(serveraddr.sin_port);
      client.setRemotePort(serverPort);
    }
  else
    {
      client.setRemotePort(localPort);
    }
}



lemoce::SocketException::SocketException(int no): error{no}
{
  message = getMessageError(error);
}

std::string lemoce::SocketException::getMessage() const
{
  return message;
}

const char * lemoce::SocketException::what() const throw()
{
  return message.c_str();
}

int lemoce::SocketException::getErrno() const
{
  return error;
}
