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

#include <proto/nfsService.grpc.pb.h>  // Include the generated header file
#include "config.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include "gsh_list.h"
#include "abstract_mem.h"
#include "fsal.h"
#include "nfs_core.h"
#include "log.h"
#include "sal_functions.h"

extern hash_table_t* ht_confirmed_client_id;

class GetClientIdService final : public nfsService::GetClientId::Service {
public:
    grpc::Status GetClientIds(grpc::ServerContext* context,
                          const nfsService::GetClientIdsRequest* request,
                          nfsService::GetClientIdsResponse* response) override {

        std::vector<uint64_t> client_ids;
        
        for (uint32_t i = 0; i < ht_confirmed_client_id->parameter.index_size; ++i) {
            struct rbt_head* head_rbt = &(ht_confirmed_client_id->partitions[i].rbt);
            
            std::unique_lock<std::mutex> lock(ht_confirmed_client_id->partitions[i].ht_lock);
            
            struct rbt_node* pn;
            RBT_LOOP(head_rbt, pn) {
                struct hash_data* pdata = RBT_OPAQ(pn);
                struct nfs_client_id_t* pclientid = pdata->val.addr;
                uint64_t clientid = pclientid->cid_clientid;
                client_ids.push_back(clientid);  // Add the client ID to the list
                RBT_INCREMENT(pn);
            }
        }

        // Add the client IDs to the response
        for (auto& id : client_ids) {
            response->add_client_ids(id);  // Adds client ID to the repeated field
        }	    
	    return grpc::Status::OK;
    }
};
