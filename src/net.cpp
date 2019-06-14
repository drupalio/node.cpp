/* -------------------------------------------------------------------------------
* Copyright (c) 2018, OLogN Technologies AG
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the OLogN Technologies AG nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL OLogN Technologies AG BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* -------------------------------------------------------------------------------*/

#include "../include/nodecpp/net_common.h"
#include "../include/nodecpp/socket_common.h"
#include "../include/nodecpp/server_common.h"

#include "infrastructure.h"
#include "../../src/tcp_socket/tcp_socket_base.h"


using namespace std;
using namespace nodecpp;
using namespace nodecpp::net;

thread_local nodecpp::net::UserHandlerClassPatterns<nodecpp::net::SocketBase::DataForCommandProcessing::UserHandlers> nodecpp::net::SocketBase::DataForCommandProcessing::userHandlerClassPattern;
thread_local nodecpp::net::UserHandlerClassPatterns<nodecpp::net::ServerBase::DataForCommandProcessing::UserHandlers> nodecpp::net::ServerBase::DataForCommandProcessing::userHandlerClassPattern;

SocketBase::SocketBase(NodeBase* node_, OpaqueSocketData& sdata) {node = node_; registerMeAndAssignSocket(sdata);}

void SocketBase::ref() {netSocketManagerBase->appRef(dataForCommandProcessing.index); }
void SocketBase::unref() { netSocketManagerBase->appUnref(dataForCommandProcessing.index); }
void SocketBase::resume() { netSocketManagerBase->appResume(dataForCommandProcessing.index); }
void SocketBase::pause() { netSocketManagerBase->appPause(dataForCommandProcessing.index); }
void SocketBase::reportBeingDestructed() { netSocketManagerBase->appReportBeingDestructed(dataForCommandProcessing.index); }

void SocketBase::destroy() { OSLayer::appDestroy(dataForCommandProcessing); }
void SocketBase::end() { OSLayer::appEnd(dataForCommandProcessing); }
bool SocketBase::write(const uint8_t* data, uint32_t size)
{
	_bytesWritten += size;
	return netSocketManagerBase->appWrite(dataForCommandProcessing, data, size);
}
bool SocketBase::write2(Buffer& b)
{
	_bytesWritten += b.size();
	return netSocketManagerBase->appWrite2(dataForCommandProcessing, b);
}
void SocketBase::registerMeAndAcquireSocket() {
	NODECPP_ASSERT( nodecpp::module_id, ::nodecpp::assert::AssertLevel::critical, this->node != nullptr );
	nodecpp::safememory::soft_ptr<SocketBase> p = myThis.getSoftPtr<SocketBase>(this);
	registerWithInfraAndAcquireSocket(this->node, p/*, netSocketManagerBase->typeIndexOfSocketO*/);
}
void SocketBase::registerMeAndAssignSocket(OpaqueSocketData& sdata) {
//	NODECPP_ASSERT( nodecpp::module_id, ::nodecpp::assert::AssertLevel::critical, this->node != nullptr );
	nodecpp::safememory::soft_ptr<SocketBase> p = myThis.getSoftPtr<SocketBase>(this);
	registerWithInfraAndAssignSocket(this->node, p/*, netSocketManagerBase->typeIndexOfSocketO*/, sdata);
}

SocketBase& SocketBase::setNoDelay(bool noDelay) { OSLayer::appSetNoDelay(dataForCommandProcessing, noDelay); return *this; }
SocketBase& SocketBase::setKeepAlive(bool enable) { OSLayer::appSetKeepAlive(dataForCommandProcessing, enable); return *this; }

void SocketBase::connect(uint16_t port, const char* ip) {
	dataForCommandProcessing.userHandlers.from(SocketBase::DataForCommandProcessing::userHandlerClassPattern.getPatternForApplying( std::type_index(typeid(*this))), this);
	connectSocket(this, ip, port);
}

#if 0
void SocketO::registerMeAndAcquireSocket() {
	NODECPP_ASSERT( nodecpp::module_id, ::nodecpp::assert::AssertLevel::critical, this->node != nullptr );
	nodecpp::safememory::soft_ptr<SocketO> p = myThis.getSoftPtr<SocketO>(this);
	registerWithInfraAndAcquireSocket(this->node, p/*, netSocketManagerBase->typeIndexOfSocketO*/);
}
void SocketO::registerMeAndAssignSocket(OpaqueSocketData& sdata) {
	NODECPP_ASSERT( nodecpp::module_id, ::nodecpp::assert::AssertLevel::critical, this->node != nullptr );
	nodecpp::safememory::soft_ptr<SocketO> p = myThis.getSoftPtr<SocketO>(this);
	registerWithInfraAndAssignSocket(this->node, p/*, netSocketManagerBase->typeIndexOfSocketO*/, sdata);
}

void Socket::registerMeAndAcquireSocket() {
	nodecpp::safememory::soft_ptr<Socket> p = myThis.getSoftPtr<Socket>(this);
	registerWithInfraAndAcquireSocket(this->node, p/*, netSocketManagerBase->typeIndexOfSocketL*/);
}
void Socket::registerMeAndAssignSocket(OpaqueSocketData& sdata) {
	nodecpp::safememory::soft_ptr<Socket> p = myThis.getSoftPtr<Socket>(this);
	registerWithInfraAndAssignSocket(this->node, p/*, netSocketManagerBase->typeIndexOfSocketL*/, sdata);}
void Socket::connect(uint16_t port, const char* ip) {
	dataForCommandProcessing.userHandlers.from(SocketBase::DataForCommandProcessing::userHandlerClassPattern.getPatternForApplying( std::type_index(typeid(*this))), this);
	connectSocket(this, ip, port);
}
#endif // 0

///////////////////////////////////////////////////////////////////////////////


void ServerBase::ref() { netServerManagerBase->appRef(dataForCommandProcessing.index); }
void ServerBase::unref() { netServerManagerBase->appUnref(dataForCommandProcessing.index); }
void ServerBase::reportBeingDestructed() { netServerManagerBase->appReportBeingDestructed(dataForCommandProcessing.index); }

void ServerBase::close()
{
	netServerManagerBase->appClose(dataForCommandProcessing.index);
}

void ServerBase::listen(uint16_t port, const char* ip, int backlog)
{
	nodecpp::safememory::soft_ptr<ServerBase> p = myThis.getSoftPtr<ServerBase>(this);
	dataForCommandProcessing.userHandlers.from(ServerBase::DataForCommandProcessing::userHandlerClassPattern.getPatternForApplying( std::type_index(typeid(*this))), this);
	netServerManagerBase->appListen(p, ip, port, backlog);
}

ServerBase::ServerBase() {
	nodecpp::safememory::soft_ptr<ServerBase> p = myThis.getSoftPtr<ServerBase>(this);
//	registerServer(this->node, p, netServerManagerBase->typeIndexOfServerO);
	registerServer(this->node, p);
}

//void ServerBase::registerServerByID(NodeBase* node, soft_ptr<net::ServerBase> t, int typeId) { registerServer(node, t, typeId); }
void ServerBase::registerServer(NodeBase* node, soft_ptr<net::ServerBase> t) { ::registerServer(node, t); }





