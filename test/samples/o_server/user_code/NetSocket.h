// NetSocket.h : sample of user-defined code

#ifndef NET_SOCKET_H
#define NET_SOCKET_H
#include "../../../../include/nodecpp/common.h"


#include "../../../../include/nodecpp/socket_type_list.h"
#include "../../../../include/nodecpp/socket_t_base.h"
#include "../../../../include/nodecpp/server_t.h"
#include "../../../../include/nodecpp/server_type_list.h"

#include <functional>


using namespace std;
using namespace nodecpp;
using namespace fmt;

class MySampleTNode : public NodeBase
{
	struct Stats
	{
		uint64_t recvSize = 0;
		uint64_t sentSize = 0;
		uint64_t rqCnt;
		uint64_t connCnt = 0;
	};
	Stats stats;

	std::unique_ptr<uint8_t> ptr;
	size_t size = 64 * 1024;
	bool letOnDrain = false;

	using SocketIdType = int;
	using ServerIdType = int;

public:
	MySampleTNode()/* : srv( this ), srvCtrl( this )*/
	{
		srv = nodecpp::safememory::make_owning<ServerType>(this);
		srvCtrl = nodecpp::safememory::make_owning<CtrlServerType>(this);
		printf( "MySampleTNode::MySampleTNode()\n" );
	}

	virtual void main()
	{
		printf( "MySampleLambdaOneNode::main()\n" );
		ptr.reset(static_cast<uint8_t*>(malloc(size)));

#ifndef NET_CLIENT_ONLY
		srv->listen(2000, "127.0.0.1", 5);
		srvCtrl->listen(2001, "127.0.0.1", 5);
#endif // NO_SERVER_STAFF
	}

	// server socket
	void onCloseServerSocket(nodecpp::safememory::soft_ptr<nodecpp::net::SocketOUserBase<MySampleTNode,SocketIdType>> socket, bool hadError)
	{
		print("server socket: onCloseServerSocket!\n");
	}
	void onConnectServerSocket(nodecpp::safememory::soft_ptr<nodecpp::net::SocketOUserBase<MySampleTNode,SocketIdType>> socket) {
		print("server socket: onConnect!\n");
	}
	void onDataServerSocket(nodecpp::safememory::soft_ptr<nodecpp::net::SocketOUserBase<MySampleTNode,SocketIdType>> socket, Buffer& buffer) {
		if ( buffer.size() < 2 )
		{
			printf( "Insufficient data on socket idx = %d\n", *(socket->getExtra()) );
			socket->unref();
			return;
		}
//		print("server socket: onData for idx %d !\n", *(socket->getExtra()) );

		size_t receivedSz = buffer.begin()[0];
		if ( receivedSz != buffer.size() )
		{
			printf( "Corrupted data on socket idx = %d: received %zd, expected: %zd bytes\n", *(socket->getExtra()), receivedSz, buffer.size() );
			socket->unref();
			return;
		}

		size_t requestedSz = buffer.begin()[1];
		if ( requestedSz )
		{
			Buffer reply(requestedSz);
			//buffer.begin()[0] = (uint8_t)requestedSz;
			memset(reply.begin(), (uint8_t)requestedSz, requestedSz);
			socket->write(reply.begin(), requestedSz);
		}

		stats.recvSize += receivedSz;
		stats.sentSize += requestedSz;
		++(stats.rqCnt);
	}
	void onDrainServerSocket(nodecpp::safememory::soft_ptr<nodecpp::net::SocketOUserBase<MySampleTNode,SocketIdType>> socket) {
		print("server socket: onDrain!\n");
	}
	void onEndServerSocket(nodecpp::safememory::soft_ptr<nodecpp::net::SocketOUserBase<MySampleTNode,SocketIdType>> socket) {
		print("server socket: onEnd!\n");
		const char buff[] = "goodbye!";
		socket->write(reinterpret_cast<const uint8_t*>(buff), sizeof(buff));
		socket->end();
		//serverSockets.remove(*(socket->getExtra()));
		srv->removeSocket( socket );
	}
	void onErrorServerSocket(nodecpp::safememory::soft_ptr<nodecpp::net::SocketOUserBase<MySampleTNode,SocketIdType>> socket, nodecpp::Error&) {
		print("server socket: onError!\n");
	}
	void onAcceptedServerSocket(nodecpp::safememory::soft_ptr<nodecpp::net::SocketOUserBase<MySampleTNode,SocketIdType>> socket) {
		print("server socket: onAccepted!\n");
	}

	using SockTypeServerSocket = nodecpp::net::SocketN<MySampleTNode,SocketIdType,
		nodecpp::net::OnConnect<&MySampleTNode::onConnectServerSocket>,
		nodecpp::net::OnClose<&MySampleTNode::onCloseServerSocket>,
		nodecpp::net::OnData<&MySampleTNode::onDataServerSocket>,
		nodecpp::net::OnDrain<&MySampleTNode::onDrainServerSocket>,
		nodecpp::net::OnError<&MySampleTNode::onErrorServerSocket>,
		nodecpp::net::OnEnd<&MySampleTNode::onEndServerSocket>,
		nodecpp::net::OnAccepted<&MySampleTNode::onAcceptedServerSocket>
	>;

	void onCloseCtrlServerSocket(nodecpp::safememory::soft_ptr<nodecpp::net::SocketOUserBase<MySampleTNode,SocketIdType>> socket, bool hadError)
	{
		print("server socket: onCloseServerSocket!\n");
	}
	void onDataCtrlServerSocket(nodecpp::safememory::soft_ptr<nodecpp::net::SocketOUserBase<MySampleTNode,SocketIdType>> socket, Buffer& buffer) {

		size_t requestedSz = buffer.begin()[1];
		if ( requestedSz )
		{
			Buffer reply(sizeof(stats));
			stats.connCnt = srv->getSockCount();
			size_t replySz = sizeof(Stats);
			uint8_t* buff = ptr.get();
			memcpy( buff, &stats, replySz ); // naive marshalling will work for a limited number of cases
			//SockTypeServerCtrlSocket* sock = serverCtrlSockets.at(*(socket->getExtra()));
			//sock->write(buff, replySz);
			socket->write(buff, replySz);
		}
	}
	void onEndCtrlServerSocket(nodecpp::safememory::soft_ptr<nodecpp::net::SocketOUserBase<MySampleTNode,SocketIdType>> socket) {
		print("server socket: onEnd!\n");
		const char buff[] = "goodbye!";
		socket->write(reinterpret_cast<const uint8_t*>(buff), sizeof(buff));
		socket->end();
		//serverCtrlSockets.remove(*(socket->getExtra()));
		srvCtrl->removeSocket( socket );
	}
	using SockTypeServerCtrlSocket = nodecpp::net::SocketN<MySampleTNode,SocketIdType,
		nodecpp::net::OnConnect<&MySampleTNode::onConnectServerSocket>,
		nodecpp::net::OnClose<&MySampleTNode::onCloseCtrlServerSocket>,
		nodecpp::net::OnData<&MySampleTNode::onDataCtrlServerSocket>,
		nodecpp::net::OnDrain<&MySampleTNode::onDrainServerSocket>,
		nodecpp::net::OnError<&MySampleTNode::onErrorServerSocket>,
		nodecpp::net::OnEnd<&MySampleTNode::onEndCtrlServerSocket>,
		nodecpp::net::OnAccepted<&MySampleTNode::onAcceptedServerSocket>
	>;

	// server
private:
#if 0
	template<class Socket>
	class ServerSockets
	{
		struct ServerSock
		{
			size_t idx;
			size_t nextFree;
			std::unique_ptr<net::SocketBase> socket;
			ServerSock( net::SocketBase* sock, size_t idx_ ) : idx( idx_ ), nextFree( size_t(-1) ), socket( sock )  {}
		};
		size_t firstFree = size_t(-1);
		std::vector<ServerSock> serverSocks;
		size_t serverSockCount = 0;
	public:
		ServerSockets() {}
		void add( net::SocketBase* sock )
		{
			if ( firstFree != size_t(-1) )
			{
				NODECPP_ASSERT( nodecpp::module_id, ::nodecpp::assert::AssertLevel::critical, firstFree < serverSocks.size() );
				ServerSock& toUse = serverSocks[firstFree];
				NODECPP_ASSERT( nodecpp::module_id, ::nodecpp::assert::AssertLevel::critical, firstFree == toUse.idx );
				firstFree = toUse.nextFree;
				toUse.socket.reset( sock );
				*(static_cast<Socket*>(sock)->getExtra()) = toUse.idx;
			}
			else
			{
				size_t idx = serverSocks.size();
				serverSocks.emplace_back( sock, idx );
				*(static_cast<Socket*>(sock)->getExtra()) = idx;
			}
			++serverSockCount;
		}
		void remove( size_t idx )
		{
			NODECPP_ASSERT( nodecpp::module_id, ::nodecpp::assert::AssertLevel::critical, idx < serverSocks.size() );
			NODECPP_ASSERT( nodecpp::module_id, ::nodecpp::assert::AssertLevel::critical, *(at(idx)->getExtra()) == idx );
			ServerSock& toUse = serverSocks[idx];
			toUse.nextFree = firstFree;
			net::SocketBase* s = serverSocks[idx].socket.release();
			Socket* sockToDelete = static_cast<Socket*>(s);
			delete sockToDelete;
			firstFree = idx;
			--serverSockCount;
		}
		Socket* at(size_t idx)
		{
			NODECPP_ASSERT( nodecpp::module_id, ::nodecpp::assert::AssertLevel::critical, idx < serverSocks.size() );
			net::SocketBase* s = serverSocks[idx].socket.get();
			return static_cast<Socket*>(s);
		}
		void clear()
		{
			for ( size_t i=0; i<serverSocks.size(); ++i )
				serverSocks[i].socket.reset();
			serverSocks.clear();
		}
		size_t getServerSockCount() { return serverSockCount; }
	};
	//ServerSockets<SockTypeServerSocket> serverSockets;
#endif // 0
public:
	void onCloseServer(const ServerIdType* extra, bool hadError) {
		print("server: onCloseServer()!\n");
		//serverSockets.clear();
	}
	void onConnectionx(const ServerIdType* extra, net::SocketBase* socket) { 
		print("server: onConnection()!\n");
		//srv->unref();
		NODECPP_ASSERT( nodecpp::module_id, ::nodecpp::assert::AssertLevel::critical, socket != nullptr ); 
		//serverSockets.add( socket );
	}
	void onListeningx(const ServerIdType* extra, size_t id, nodecpp::net::Address addr) {print("server: onListening()!\n");}
	void onErrorServer(const ServerIdType* extra, Error& err) {print("server: onErrorServer!\n");}

	using ServerType = nodecpp::net::ServerN<MySampleTNode,SockTypeServerSocket,ServerIdType,
		nodecpp::net::OnConnectionSO<&MySampleTNode::onConnectionx>,
		nodecpp::net::OnCloseSO<&MySampleTNode::onCloseServer>,
		nodecpp::net::OnListeningSO<&MySampleTNode::onListeningx>,
		nodecpp::net::OnErrorSO<&MySampleTNode::onErrorServer>
	>;
//	ServerType srv;
	nodecpp::safememory::owning_ptr<ServerType> srv;

	// ctrl server
private:
	//ServerSockets<SockTypeServerCtrlSocket> serverCtrlSockets;

public:
	void onCloseServerCtrl(const ServerIdType* extra, bool hadError) {
		print("server: onCloseServerCtrl()!\n");
		//serverCtrlSockets.clear();
	}
	void onConnectionCtrl(const ServerIdType* extra, net::SocketBase* socket) { 
		print("server: onConnectionCtrl()!\n");
		//srv->unref();
		NODECPP_ASSERT( nodecpp::module_id, ::nodecpp::assert::AssertLevel::critical, socket != nullptr ); 
		//serverCtrlSockets.add( socket );
	}
	void onListeningCtrl(const ServerIdType* extra, size_t id, nodecpp::net::Address addr) {print("server: onListeninCtrlg()!\n");}
	void onErrorServerCtrl(const ServerIdType* extra, Error& err) {print("server: onErrorServerCtrl!\n");}

	using CtrlServerType = nodecpp::net::ServerN<MySampleTNode,SockTypeServerCtrlSocket,ServerIdType,
		nodecpp::net::OnConnectionSO<&MySampleTNode::onConnectionCtrl>,
		nodecpp::net::OnCloseSO<&MySampleTNode::onCloseServerCtrl>,
		nodecpp::net::OnListeningSO<&MySampleTNode::onListeningCtrl>,
		nodecpp::net::OnErrorSO<&MySampleTNode::onErrorServerCtrl>
	>;
//	CtrlServerType srvCtrl;
	nodecpp::safememory::owning_ptr<CtrlServerType> srvCtrl;


	using EmitterType = nodecpp::net::SocketTEmitter<net::SocketO, net::Socket>;
	using EmitterTypeForServer = nodecpp::net::ServerTEmitter<net::ServerO, net::Server>;
};

#endif // NET_SOCKET_H
