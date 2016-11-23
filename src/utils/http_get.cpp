#include <string.h>

#include "socket.h"

#include <iostream>
#include <sstream>
#include <string>
#include <memory>

int main(int argc, char *argv[])
{

  int MAX_BUFFER = 8192;
  
  if (argc != 3)
    {
      std::cerr << "usage: http_get <ip> <porta>" << std::endl;
      return -1;
    }

  int porta;
  std::istringstream iss{std::string{argv[2]}};
  iss >> porta;

  lemoce::ClientSocket client{std::string{argv[1]}, porta};

  std::cout << client.getRemoteHost() << std::endl;
  std::cout << client.getRemotePort() << std::endl;
    
  try
    {
      client.connect();
      client.write("GET / HTTP/1.0\r\n\r\n");

      std::unique_ptr<std::string> response = client.read();
      std::cout << *response << std::endl;
      client.close();
    }
  catch(lemoce::SocketException& e)
    {
      std::cout << e.getErrno() << std::endl;
      std::cout << e.getMessage() << std::endl;
      try
        {
          client.close();
        }
      catch(std::exception& ex)
        { }
    }

  return 0;
}
