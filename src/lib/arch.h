#ifndef __ARCH_H__
#define __ARCH_H__

#include <string>

namespace lemoce
{

  void initializeSocketAPI();
  bool isClosed(int);
  int closeSocket(int);
  void treatSocketError(bool);
  std::string getMessageError(int);

}

#endif /* ARCH_H */

