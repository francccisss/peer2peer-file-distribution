# testing

- assume a node where the array of `peers` a requester is looking for exists within it's `peer_table`
- also a assume a node that contains N number of neighbors it can look up to if the `peers`
  that the caller is looking for to the assumed node does not contain the bucket within its `peer_table`
- assume that this node will only listen and respond to requests and not iniate a request of its own.
- this node would also contain N active peers, within the same host using different ports
- the active peers will only listen to JOIN method calls by a caller and respond with either a success or failure status.

- the client node will connect to the assumed node, and send a get_peers as well as a join_peers rpc call



# script

the main file (assumed node), will create peers and store them within its own peer_table with the hash info, use `execv()` to create these active peers which are just nodes that will only listen in respond to join requests, and the peer process will take in a couple of arguments from the main program


