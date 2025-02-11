/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * -------------
 */
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <thread>
#include <mutex>
#include "GrpcServer.h"
#include <proto/nfsService.grpc.pb.h>
#include <nfsServiceServer.h>

#define GRPCERROR(MESSAGE)                                                    \
        fprintf(stderr, "[%s:%d] %s: %s\n", __FILE__, __LINE__, (MESSAGE), \
                strerror(errno))
#define GRPCFATAL(MESSAGE) (GRPCERROR(MESSAGE), abort())

GrpcServer grpc_server;

GrpcServer::GrpcServer() : running_(false) {}

GrpcServer::~GrpcServer() {
    stop();
}

void GrpcServer::start(uint16_t port)
{
        const std::lock_guard<std::mutex> lock(mutex_);
        if (running_)
                GRPCFATAL("Already running");

	std::string server_address("0.0.0.0:" + std::to_string(port));

    	grpc::ServerBuilder builder;
    	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    	// Register your service here, e.g., GreeterServiceImpl
    	// example::GreeterServiceImpl service;
    	// builder.RegisterService(&service);
	GetClientIdService showClientService;
	builder.RegisterService(&showClientService);

    	server_ = builder.BuildAndStart();

    	running_ = true;
	server_thread_ = std::thread([this]() { server_->Wait(); });
}

void GrpcServer::stop()
{
        const std::lock_guard<std::mutex> lock(mutex_);
        if (running_) {
                running_ = false;
                server_->Shutdown();
		if (server_thread_.joinable()) {
			server_thread_.join();  // Wait for the server thread to finish
		}
        }
}

extern "C" {

void grpc__init(uint16_t port)
{
        static bool initialized = false;
        if (initialized)
                return;
        grpc_server.start(port);
        initialized = true;
}

} /* extern C */

