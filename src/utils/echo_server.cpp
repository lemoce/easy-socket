
#include <chrono>
#include <future>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <string.h>
#include <thread>
#include <utility>

#include "socket.h"
#include "config.h"


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


void echoServerTask(std::unique_ptr<lemoce::ClientSocket> client_ptr,
                    std::shared_ptr<std::atomic<bool>> done)
{
  lemoce::ClientSocket client = *client_ptr;
  std::thread::id threadId = std::this_thread::get_id();
  
  try
    {
      while(!client.isClosed())
        {
          std::unique_ptr<std::string> message_ptr = client.read();
          if ((*message_ptr).size() == 0)
            {
              client.close();
              break;
            }
          std::string * message = message_ptr.get();
          client.write(*message);
          std::cout << "enviada mensagem para filho " << threadId <<
            " (" << client.getLocalHost() << ", " <<
            client.getLocalPort() << ")" << std::endl;
        }
    }
  catch (lemoce::SocketException & ex)
    {
      client.close();
    }

  std::cout << "terminei a thread" << std::endl;
  (*done) = true;
}


int main(int argc, char *argv[])
{
  int MAX_BUFFER = 8192;
  int port = 7;

  if (argc == 2)
    {
      std::istringstream iss{argv[1]};
      iss >> port;
    }

  try
    {
      lemoce::ServerSocket server{port};

      server.listen();

      std::list<
        std::pair<std::shared_ptr<std::atomic<bool>>,
                  std::unique_ptr<std::thread>>> threads;
      std::thread manager{threadManager, std::ref(threads)};

      
      for(;;)
        {

          std::unique_ptr<lemoce::ClientSocket> client_ptr{new lemoce::ClientSocket{}};

          try
            {
              server.accept((*client_ptr));
            }
          catch (lemoce::SocketException & ex)
            {
              if (ex.getErrno() == EINTR)
                {
                  (*client_ptr).close();
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
              echoServerTask,
              std::move(client_ptr),
              done
            }};

          threads.push_back(std::make_pair(done, std::move(thread)));

        }
    }
  catch (lemoce::SocketException& ex)
    {
      std::cout << ex.getMessage() << std::endl;
    }
  
  return 0;
}
