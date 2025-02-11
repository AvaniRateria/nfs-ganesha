#include <iostream>
#include <src/third_party/grpc/include/thirdparty/grpcpp/grpcpp.h>
#include <proto/clientGetId.grpc.pb.h>

void GetClientIds() {
    // Create a channel to communicate with the server
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    std::unique_ptr<nfs::GetClientId::Stub> stub = nfs::GetClientId::NewStub(channel);

    // Create a request and response
    nfs::GetClientIdsRequest request;
    nfs::GetClientIdsResponse response;
    grpc::ClientContext context;

    // Make the gRPC call
    grpc::Status status = stub->GetClientIds(&context, request, &response);

    if (status.ok()) {
        // Print the list of client IDs from the response
        std::cout << "Client IDs: ";
        for (int i = 0; i < response.client_ids_size(); ++i) {
            std::cout << response.client_ids(i) << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "gRPC call failed: " << status.error_message() << std::endl;
    }
}
