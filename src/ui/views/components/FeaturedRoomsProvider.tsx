import { Room, StateEvent } from "@thirdroom/hydrogen-view-sdk";

import { useStateEvents } from "../../hooks/useStateEvents";
import { RepositoryEvents } from "../session/discover/DiscoverView";

export function useFeaturedRooms(repoRoom: Room) {
  const featuredRoomsMap = useStateEvents(repoRoom, RepositoryEvents.FeaturedRooms);
  return [...featuredRoomsMap].filter(([eventId, stateEvent]) => Object.keys(stateEvent.content).length > 0);
}

export function FeaturedRoomsProvider({
  room,
  children,
}: {
  room: Room;
  children: (featuredRooms: [string, StateEvent][]) => JSX.Element | null;
}) {
  const featuredRooms = useFeaturedRooms(room);
  return children(featuredRooms);
}