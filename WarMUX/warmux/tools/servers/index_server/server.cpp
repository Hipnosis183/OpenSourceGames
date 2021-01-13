/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <WSERVER_debug.h>
#include "server.h"
#include "client.h"

Server::Server(int port)
{
  DPRINT(INFO, "Starting server... listening on port %i",port);
  // Init the listening socket
  fd = socket(AF_INET, SOCK_STREAM, 0);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(port);

  int on = 1;
  if ( setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0 ) {
    PRINT_FATAL_ERROR;
  }

  if ( bind(fd, (struct sockaddr*) &address, sizeof(address)) == -1 ) {
    PRINT_FATAL_ERROR;
  }

  // 128 is the size of the connection queue
  // -> TODO : use a sysctl to get the max size accepted by the kernel
  // ( see 'man listen' )
  if (listen(fd, 128) == -1) {
    PRINT_FATAL_ERROR;
  }

  // Init the socket set:
  FD_ZERO(&sock_set);
  FD_SET(fd, &sock_set);
}

Server::~Server()
{
  close(fd);
}

int & Server::GetFD()
{
  return fd;
}

fd_set & Server::GetSockSet()
{
  return sock_set;
}

Client* Server::NewConnection()
{
  int client_fd;
  struct sockaddr_in client_address;
  socklen_t addr_size = sizeof(client_address);

  client_fd = accept(fd, (struct sockaddr*)&client_address, &addr_size);
  if( client_fd == -1 )
    {
      PRINT_ERROR;
      DPRINT(INFO, "Client rejected..");
      return NULL;
    }

  FD_SET(client_fd, &sock_set);

  unsigned int ip = client_address.sin_addr.s_addr;

  Client* client = new Client(client_fd, ip);
  DPRINT(CONN, "New connection opened by %i.%i.%i.%i (%i)",
         ip&0xFF, (ip>>8)&0xFF, (ip>>16)&0xFF, ip>>24, ip);
  return client;
}

void Server::CloseConnection(int client_fd)
{
  close(client_fd);
  FD_CLR(client_fd, &sock_set);
  DPRINT(CONN, "Connection closed.");
}

