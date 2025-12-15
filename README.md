# peer2peer-file-distribution

A very simple peer-to-peer file destribution network that does not include a tracker server.

> This is just a toy project for practicing C language, so nuances from torrent protocols are ommited from my own implementations.

This implementation provides the basic functionality of a standard torrent DHT protocol that is mostly based on my understanding or higher level understanding of how it functions.

Each nodes are bootstrapped via a descriptor file that describes the file in question, as well as providing the node with a list of IP Addresses along with their ports to be boostrapped into the newly connected node.
The process will take in a parameter which is a path to the descriptor file, which should be then parsed and read by the processing node.

### Connections

The provided list of IP Addresses and Ports from the descriptor file are not direct connections to the nodes where the file in question exists, instead the requesting node propagates
to every neighboring nodes until it's own neighbors are able to return the list of `peers/nodes` that actually has the file through the use of a hash table.

For each node will contain their own list of peers in their own `peer_table` where the key to the list is the `hash_key` of the file, so if a node contains the list of `peers` that are all connected to the torrent
then that node returns the list of `peers` directly to the requesting node that initiated the request chain.

> Now you might be asking, "then where the hell are we supposed to find the known IPs and Ports for bootstrapping?" honestly I can't be bothered to create a tracker
> that will be providing any of those information about the distributed file to create descriptor file, this is just a torrent protocol
> that reads from a descriptor file which the user should manually populates the list IP addresses, if they want to use it on different hosts.
>
> And also this is not meant to be used alongside other torrent protocols duh.


The new node calls the `get_peers` function which will be the starting point of the chain of requests, but instead of backing tracking to every subsequent neighbors of a node that called the function,
the `initiating` node or new node will pass in its own `origin` which contains the initiating node's ip and port address as `src` which is the `source` of the `get_peers` chain and is then,
passed around by every propageted neighboring nodes.

A pseudo `stop-wait` protocol is implemented on the `custom RPC protocol` on top of `UDP` that would fire a request and wait for some time, if the socket is ready then
we know that the connection still exists then we can process the response from the peer else if at timeout then assumed that peer has disconnected and drop the request and move on (still need to implement a retry mechanism just in case).

So when a list of peers do exist on the current node, the `src` is then decoded from the `body` of an `rpc_message`, and then the current node calls `reply_rpc` directly to the node that initiated the chain.

Now assuming we've got all the peers that we need to starting distributing the file, first we need to ping each peers and see if any of them responds aka if they are connected or not then we can just connect to the ones that are online
and drop the ones that are not. a `join_peers` is then called subsequently after retrieving the peers, and then each peer within the same torrent would receive the request, store the new peer within the `peer_table` and set
it's `status` to `idle` since it has not declared to either `seed` or `leech` from any of its peers.




