#include <string.h>
#include <time.h>

#include "socket.h"

#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char *argv[])
{

  const int MAX_BUFFER = 8192;
  int porta = 13;

  try
    {
      lemoce::ServerSocket s{porta};
      s.listen();
      for(;;)
        {
          lemoce::ClientSocket client;
          s.accept(client);

          time_t ticks = time(nullptr);
          char buff[MAX_BUFFER];
          snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
          client.write(std::string{buff});

          std::cout << "mensagem enviada para " << client.getLocalHost() << ":" << client.getLocalPort() << std::endl;

          client.close();
        }
    }
  catch (lemoce::SocketException& ex)
    {
      std::cout << ex.getMessage() << std::endl;
    }

  
  return 0;
}
