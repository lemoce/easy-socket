#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <string>
#include <memory>
#include <exception>

namespace lemoce {

  ///
  /// Brief Classe base que contem apenas o File Descriptor do socket.
  ///
  class Socket
  {
  public:
    Socket();

    int getFd() const;
    void setFd(int fd);

    bool isClosed();
    void close();

  protected:

    int fd; //!< Brief File Descriptor do socket
  };

  ///
  /// Brief Classe que abstrai as operações de recv e send do TCP/IP.
  ///  
  class ClientSocket : public Socket {
  public:
    ClientSocket();
    ClientSocket(std::string remote, int port);
    
    void setRemoteHost(std::string);
    std::string getRemoteHost() const;
    void setRemotePort(int port);
    int getRemotePort() const;
    void setLocalHost(std::string);
    std::string getLocalHost() const;
    void setLocalPort(int);
    int getLocalPort() const;
    void setBufferSize(int);
    int getBufferSize() const;

    void connect();
    std::unique_ptr<std::string> read();
    int read(char msg[], const size_t n);
    void write(const std::string& message);
    void write(const char * msg, const size_t n);
    
  private:

    int bufferSize;
    void fillLocalHostPort();
    
    std::string remoteHost; //!< Brief host do servidor
    int remotePort;         //!< Brief porta do servidor

    std::string localHost;  //!< Brief host do client
    int localPort;          //!< Brief porta do client

    
    
  };

  ///
  /// Brief Classe que abstrai as operações de accept
  ///  
  class ServerSocket : public Socket
  {
  public:
    ServerSocket(int port);
    
    void listen();
    void accept(ClientSocket & client);

  private:
    static int BACKLOG;     //!< Brief numero de backlog possíveis

    int localPort;          //!< Brief porta local do servidor
  };


  ///
  /// Brief Class embarca o código de erro
  ///
  class SocketException : public std::exception
  {
  public:
    SocketException(int err_number);
    std::string getMessage() const;
    virtual const char * what() const throw();
    int getErrno() const;
  private:
    std::string message;
    int error;
  };
  
}

#endif /* __SOCKET_H__ */
