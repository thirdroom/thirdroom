import { addComponent } from "bitecs";

import { ThirdRoomModule } from "../../plugins/thirdroom/thirdroom.game";
import { GameContext } from "../GameTypes";
import { getModule } from "../module/module.common";
import { isHost } from "./network.common";
import {
  NetworkModule,
  exitedNetworkedQuery,
  ownedPlayerQuery,
  GameNetworkState,
  newPeersQueue,
  getPeerIndex,
  tryGetPeerIndex,
} from "./network.game";
import { Networked, Relaying } from "./NetworkComponents";
import {
  serializeHostSnapshot,
  serializePeerEntered,
  serializeHostCommands,
  serializeEntityUpdates,
} from "./NetworkMessage";
import { enqueueReliable, enqueueReliableBroadcast, enqueueUnreliableBroadcast } from "./NetworkRingBuffer";

const sendUpdatesHost = (ctx: GameContext, network: GameNetworkState) => {
  /**
   * Send updates from host if:
   * - we have connected peers
   */

  const connectedToPeers = network.peers.length;
  if (!connectedToPeers) {
    return;
  }

  const haveNewPeers = newPeersQueue.length > 0;
  if (haveNewPeers) {
    const thirdroom = getModule(ctx, ThirdRoomModule);

    // create an avatar for each new player before serializing the host snapshot
    for (const peerId of newPeersQueue) {
      const avatar = thirdroom.replicators!.avatar.spawn(ctx);

      const peerIndex = Number(tryGetPeerIndex(network, peerId));
      Networked.authorIndex[avatar.eid] = peerIndex;

      addComponent(ctx.world, Relaying, avatar.eid);
      Relaying.for[avatar.eid] = peerIndex;
    }

    // newly created avatars will be picked up by networking queries and serialized in the host snapshot
    const hostSnapshot = serializeHostSnapshot(ctx, network);

    let peerId;
    while ((peerId = newPeersQueue.dequeue())) {
      const peerIndex = tryGetPeerIndex(network, peerId);

      // inform new peer(s) and all other peers of new avatar(s)
      // TODO: this is redundant for peers other than this new peer
      enqueueReliableBroadcast(network, hostSnapshot);

      // inform all peers of the new peer's info
      enqueueReliableBroadcast(network, serializePeerEntered(ctx, network, peerId, peerIndex));
    }
  }

  // send HostCommands message
  enqueueReliableBroadcast(network, serializeHostCommands(ctx, network));

  // send EntityUpdates message
  enqueueUnreliableBroadcast(network, serializeEntityUpdates(ctx, network));
};

const sendUpdatesClient = (ctx: GameContext, network: GameNetworkState) => {
  /**
   * Send updates from client if:
   * - we have peer connections
   * - we have a host connection
   * - host snapshot received
   */

  const connectedToPeers = network.peers.length;
  if (!connectedToPeers) {
    return;
  }

  const connectedToHost = network.hostId && network.peers.includes(network.hostId);
  if (!connectedToHost) {
    return;
  }

  const hostSnapshotReceived = ownedPlayerQuery(ctx.world).length > 0 && getPeerIndex(network, network.peerId);
  if (!hostSnapshotReceived) {
    return;
  }

  enqueueReliable(network, network.hostId, serializeEntityUpdates(ctx, network));
};

export function OutboundNetworkSystem(ctx: GameContext) {
  const network = getModule(ctx, NetworkModule);

  try {
    if (isHost(network)) {
      sendUpdatesHost(ctx, network);
    } else {
      sendUpdatesClient(ctx, network);
    }
  } catch (e) {
    console.error(e);
  }

  // delete networkId to entityId mapping
  const exited = exitedNetworkedQuery(ctx.world);
  for (let i = 0; i < exited.length; i++) {
    const eid = exited[i];
    network.networkIdToEntityId.delete(BigInt(Networked.networkId[eid]));
  }

  return ctx;
}