#include "socket.h"
#include "config.h"

#include <memory>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <string>

void str_echo(std::istream& in, lemoce::ClientSocket & client)
{

  std::string payload;

  while(std::getline(in, payload))
    {
      try
        {
          std::cout << "li pelo teclado " << payload << std::endl;
          client.write(payload);
          std::unique_ptr<std::string> ptrRead = client.read();
          std::cout << "recebi do servidor " << *ptrRead << std::endl;
        }
      catch(lemoce::SocketException& ex)
        {
          if (ex.getErrno() != EINTR)
            throw ex;
        }
    }
}

int main(int argc, char *argv[])
{
  int MAX_BUFFER = 8192;
  std::string host = "localhost";
  int port = 7;

  //std::cin.exceptions(std::ios_base::failbit | std::ios_base::eofbit);
  
  if (argc == 3)
    {
      std::istringstream issHost{argv[1]};
      issHost >> host;
      
      std::istringstream issPort{argv[2]};
      issPort >> port;
    }

  try
    {
      lemoce::ClientSocket client(host, port);
      client.connect();

      str_echo(std::cin, client);
      
      client.close();
    }
  catch (lemoce::SocketException& ex)
    {
      std::cout << ex.getMessage() << std::endl;
    }
  return 0;
}
