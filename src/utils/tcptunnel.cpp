
#include <string.h>
#include <sstream>
#include <iostream>
#include <thread>
#include <list>
#include <chrono>
#include <future>
#include <memory>
#include <utility>

#include "config.h"
#include "socket.h"


void threadManager(std::list<std::pair<std::shared_ptr<std::atomic<bool>>, std::unique_ptr<std::thread>>> & threads)
{
  std::cout << "iniciei gerenciador de threads" << std::endl;
  while(true)
    {
      std::this_thread::sleep_for(std::chrono::seconds(30));
      std::cout << "realizando limpeza das threads (size = " << threads.size() << ")" << std::endl;
      int i = 0;
      std::list<std::pair<std::shared_ptr<std::atomic<bool>>, std::unique_ptr<std::thread>>>::iterator it;
      for(it = threads.begin(); it != threads.end(); ++i)
        {
          if ((*(it->first)))
            {
              (it->first).reset();
              (it->second)->join();
              (it->second).reset();
              it = threads.erase(it);
              std::cout << "removi thread inutilizada " << (i+1) << std::endl;
            }
          else
            {
              ++it;
            }
        }
    }

}

void tunnelingTask(std::unique_ptr<lemoce::ClientSocket> server_ptr,
                   std::unique_ptr<lemoce::ClientSocket> client_ptr,
                   std::shared_ptr<std::atomic<bool>> done)
{

  lemoce::ClientSocket server = *server_ptr;
  lemoce::ClientSocket client = *client_ptr;
  
  try
    {
      std::cout << "iniciei thread de tunneling" << std::endl;
      while(!server.isClosed())
        {
      
          std::unique_ptr<std::string> incomingMessage = client.read();
          if ((*incomingMessage).size() == 0)
            {
              server.close();
              client.close();
              break;
            }
          std::cout << " ==> " << (*incomingMessage) << std::endl;

          std::string * tmpMessage = incomingMessage.get();
                  
          server.write((*tmpMessage));
          std::cout << (*tmpMessage) << " ==>" << std::endl;
          std::unique_ptr<std::string> responseMessage = server.read();
          if ((*responseMessage).size() == 0)
            {
              server.close();
              client.close();
              break;
            }
          std::cout << (*responseMessage) << " <==" << std::endl;

          tmpMessage = responseMessage.get();
          client.write((*tmpMessage));
          std::cout << " <== " << (*tmpMessage) <<  std::endl;
        }
    }
  catch (lemoce::SocketException & ex)
    {
      server.close();
      client.close();
    }
  std::cout << "terminei a thread" << std::endl;
  (*done) = true;
}


int main(int argc, char *argv[])
{

  if (argc != 4)
    {
      std::cout << "uso errado" << std::endl;
      return -1;
    }

  std::istringstream listenPortStream{argv[1]};
  std::istringstream connectPortStream{argv[3]};
  
  int MAX_BUFFER = 8192;
  int listeningPort, remotePort;

  listenPortStream >> listeningPort;
  connectPortStream >> remotePort;
  
  std::string remoteHost{argv[2]};

  try
    {
      lemoce::ServerSocket server{listeningPort};
      server.listen();

      std::list<
        std::pair<std::shared_ptr<std::atomic<bool>>,
                  std::unique_ptr<std::thread>>> threads;
      std::thread manager{threadManager, std::ref(threads)};
      
      for (;;)
        {

          std::unique_ptr<lemoce::ClientSocket> client_ptr{new lemoce::ClientSocket{}};
          std::unique_ptr<lemoce::ClientSocket> remoteClient_ptr;

          try
            {
              server.accept((*client_ptr));
              remoteClient_ptr.reset(new lemoce::ClientSocket{remoteHost, remotePort});
              (*remoteClient_ptr).connect();
              std::cout << "conectei com o servidor remoto" << std::endl;
            }
          catch (lemoce::SocketException & ex)
            {
              if (ex.getErrno() == EINTR)
                {
                  (*remoteClient_ptr).close();
                  std::cout << "fechei conexao pela excecao de interrupcao" << std::endl;
                  continue;
                }
              else
                {
                  throw ex;
                }
            }

          std::shared_ptr<std::atomic<bool>> done{new std::atomic<bool>(false)};
          std::unique_ptr<std::thread> thread{new std::thread
            {
              tunnelingTask, std::move(remoteClient_ptr),
              std::move(client_ptr),
              done
            }};
          
          threads.push_back(std::make_pair(done, std::move(thread)));
        }
    }
  catch (lemoce::SocketException & ex)
    {
      std::cout << ex.getMessage() << std::endl;
    }

  
  return 0;
}
